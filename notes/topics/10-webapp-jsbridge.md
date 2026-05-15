# WebApp / mini-apps: JS-bridge может вернуть MSISDN абонента через Mobile ID

## Сериализуемый запрос/ответ

`defpackage/snk.java` (сериализатор для Kotlinx Serialization):

```java
ptd ptdVar = new ptd(
    "one.me.webapp.domain.jsbridge.delegates.private.WebAppVerifyMobileIdRequest",
    snkVar, 2);
ptdVar.k("requestId", false);
ptdVar.k(MLFeatureConfigProviderBase.URL_KEY, false);   // "url"
descriptor = ptdVar;
```

`defpackage/unk.java`:

```java
public final class unk {
    public final String a;     // requestId
    public final String b;     // url
    ...
}
```

`defpackage/vnk.java`:

```java
ptd ptdVar = new ptd(
    "one.me.webapp.domain.jsbridge.delegates.private.WebAppVerifyMobileIdResponse",
    vnkVar, 4);
ptdVar.k("requestId", false);
ptdVar.k("statusCode", false);
ptdVar.k("headers", false);
ptdVar.k("data", false);
```

`defpackage/zij.java`:

```java
zij zijVar = new zij("VERIFY_MOBILE_ID", 0);
public final String c() { return "WebAppVerifyMobileId"; }
public final String d() { return "verify_mobile_id"; }
public final boolean e() { return true; }   // private = true
```

То есть в JS-bridge MAX зарегистрирован метод `verify_mobile_id` (помечен **private**, то есть «вызывать могут только официальные / системные мини-апы»). Контракт:

- WebApp посылает: `{requestId, url}` — URL у оператора (`hhe.mts.ru`, `idgw.mobileid.mts.ru`, `mobileid.megafon.ru`, `he-mc.tele2.ru`, `he-mc.t2.ru`, `balance.beeline.ru`).
- Нативный код MAX делает запрос на этот URL **со своей мобильной сети** (это критично — иначе HE не сработает).
- В ответ возвращает мини-апе: `{requestId, statusCode, headers (Map), data}`.

## Что это значит

Это **прокси-канал «дай мне MSISDN абонента» для мини-апок**. Алгоритм:

1. Мини-апа в MAX запрашивает у клиента: «сходи на http://hhe.mts.ru/.../msisdn — это header enrichment endpoint у моего оператора, и отдай ответ».
2. MAX делает HTTP-запрос **по cleartext** — он специально whitelisted в `network_security_config`.
3. Оператор оборудованием в своей сети дописывает в headers этот самый MSISDN/IMSI/customer-id.
4. MAX возвращает мини-апе всё, включая `headers` целиком.
5. Мини-апа парсит и узнаёт номер телефона пользователя — **без отдельного диалога согласия в системе**.

В коде сервиса **нет видимого explicit consent prompt** для `verify_mobile_id` (метод приватный — то есть для системных мини-апок без подтверждения). То есть «доверенная» мини-апа (например, гос-сервис, рекламный партнёр, банк) может молча получить MSISDN.

## Другие JS-bridge методы

`@JavascriptInterface` найдено в трёх классах:

```java
// defpackage/i6e.java — публичный bridge
@JavascriptInterface
public final void postEvent(String str, String str2) {
    this.a.A(str, str2, true);     // <- 3-й параметр true = "trusted"
}

// defpackage/upk.java — общий bridge
@JavascriptInterface
public final void postEvent(String str, String str2) {
    this.a.A(str, str2, false);    // <- 3-й параметр false
}

@JavascriptInterface
public final void resolveShare(String str, byte[] bArr, String str2, String str3) {
    ...
}

// defpackage/hgk.java — performance
@JavascriptInterface
public final void trackFcp(long j) {  // first contentful paint
    ...
}
```

`postEvent(String, String)` — общий канал «пинг от WebApp в нативный код». Через него, скорее всего, и проходят все остальные методы (включая `verify_mobile_id`, NFC HCE setup, и т.п.) с разными `event` строками. То есть `@JavascriptInterface` экспонирован минимально, всё внутри — JSON-роутинг.

## NFC HCE через bridge

См. `notes/topics/07-nfc-hce-webapp.md`. WebApp через JS-bridge кладёт байты в `jxb.d`, после этого `WebAppNfcService` отдаёт их NFC-терминалу при SELECT.

## webapp-phone-hash

PMS-флаг `webapp-phone-hash` (`useWebAppPhoneHash`) — серверно включаемый режим, при котором мини-апам отдаётся **хеш номера телефона** пользователя как стабильный идентификатор (фингерпринт между запусками). Это в дополнение к Mobile ID — теперь без HE можно ассоциировать пользователя между мини-апами.

## webapp-push-open и webapp-pr и webapp-exc

- `webapp-push-open` (`webAppPushOpenEnabled`) — мини-апу можно открывать прямо из push-уведомления, минуя главный экран.
- `webapp-pr` — `webapp-pr-bridge`, отдельный набор API.
- `webapp-exc` (`webAppGestureExceptions: long[]`) — список ID мини-апок, которые исключены из жестового UI (т.е. перехватывают свайпы себе). Через серверное обновление этого списка можно дать конкретной мини-апе полный контроль над жестами.

## Запрет VPN для мини-апок

`one/me/webapp/util/WebAppHttpClient$WebAppHasVpnException` — отдельный exception, который кидается, когда HTTP-клиент мини-апы видит TRANSPORT_VPN. То есть **мини-апы внутри MAX в принципе не работают через VPN**: при наличии VPN-туннеля исполнение HTTP-запросов из них валится `WebAppHasVpnException`. Это форсит пользователя выключать VPN, чтобы воспользоваться, например, государственной мини-апой.

## Кратко для статьи

> «У MAX внутри есть платформа мини-приложений. Из неё, через приватный JS-bridge `verify_mobile_id`, мини-апа просит нативный клиент MAX выполнить HTTP-запрос на оператор-ный Mobile ID/HE-эндпоинт; в ответ получает headers и data в чистом виде. На пути к оператору запрос идёт по открытому HTTP (домены whitelisted в `network_security_config`), оператор подставляет в заголовки идентификатор абонента, и мини-апа узнаёт MSISDN. Этот же клиент HTTP в мини-апах кидает `WebAppHasVpnException` при активном VPN — то есть мини-апы намеренно отказываются работать через VPN. Плюс серверный флаг `webapp-phone-hash` отдаёт мини-апам хеш телефона как стабильный фингерпринт, и `webapp-push-open` позволяет открывать мини-апу прямо из push.»
