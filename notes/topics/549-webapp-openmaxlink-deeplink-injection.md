---
tags: [high, webview, js-bridge, deeplink-injection, no-whitelist, webapp, miniapp]
status: verified
severity: high
sources:
  - work/jadx_base/sources/defpackage/ahk.java
  - work/jadx_base/sources/defpackage/upk.java
  - work/jadx_base/sources/defpackage/ijk.java
  - work/jadx_base/sources/defpackage/bek.java
  - work/jadx_base/sources/defpackage/udk.java
  - work/jadx_base/sources/defpackage/wik.java
  - work/jadx_base/sources/defpackage/uhk.java
related:
  - "[[10-webapp-jsbridge]]"
  - "[[13-deeplinks-idp]]"
  - "[[534-server-webview-js-injection]]"
  - "[[541-miniapp-disable-flag-secure]]"
  - "[[550-webapp-exc-user-click-bypass]]"
---

# 549. WebAppOpenMaxLink — JS-bridge метод без URL whitelist (deeplink injection)

## TL;DR

JS-bridge метод `WebAppOpenMaxLink` принимает **произвольный URL** от мини-аппа и передаёт его во внутренний deeplink-роутер MAX (`:link-intercept`) **без какой-либо проверки**: нет whitelist доменов, нет regex по схемам, нет проверки структуры URL. Любая мини-апа, загруженная в `WebAppRootScreen`, может программно навигировать пользователя по любому deeplink приложения. Боты из серверного списка `webapp-exc` могут это делать **без user-click** (см. [[550-webapp-exc-user-click-bypass]]).

Сравнение: в Telegram эквивалент `openTelegramLink` ограничен доменом `t.me`. В MAX — никакого ограничения.

## 1. Регистрация bridge

`ahk.java` (контроллер `WebAppRootScreen`) регистрирует три JavaScript interface:

```java
v9gVar.addJavascriptInterface(new upk(webAppRootScreen4.s1()), "WebViewHandler");
v9gVar.addJavascriptInterface(new hgk(webAppRootScreen4.D0), "AndroidPerf");
if (webAppRootScreen4.s1().b1) {
    v9gVar.addJavascriptInterface(new i6e(webAppRootScreen4.s1()), "PrivateWebViewHandler");
}
```

`WebViewHandler` (`upk`) — **публичный bridge, доступный всем мини-аппам**. Среди его методов — `postEvent(name, data)`.

## 2. Маршрут события `WebAppOpenMaxLink`

```
JS:  WebViewHandler.postEvent("WebAppOpenMaxLink", '{"url":"max://..."}')
        ↓
Java: upk.postEvent → ijk.A(name, data, isPrivate=false)
        ↓
      gz8 (JsBridgeFactory) → bek (delegate для links group)
        ↓
      udk (action: WebAppOpenMaxLink → wik)
        ↓
      uhk.invoke(url, webappChatId)
        ↓
      внутренний роутер :link-intercept
```

## 3. Код

### `bek.java` (links delegate)

```java
public Object a(String name, String dataJson) {
    if ("WebAppOpenMaxLink".equals(name)) {
        return new udk(...).invoke(dataJson);  // ВЫЗОВ → udk
    }
    if ("WebAppOpenLink".equals(name)) {
        return new vdk(...).invoke(dataJson);  // отдельная цепочка
    }
    // ...
}
```

### `udk.java` (parser + dispatcher)

```java
public Object invoke(String json) {
    JSONObject obj = new JSONObject(json);
    String url = obj.getString("url");          // ← мини-апа передаёт URL
    // !! НЕТ ПРОВЕРКИ url !!
    return wik.invoke(url, webappChatId);
}
```

### `wik.java` → `uhk.java`

```java
// wik
public Object invoke(String url, long chatId) {
    Uri uri = Uri.parse(url);
    Uri withChatId = uri.buildUpon()
        .appendQueryParameter("webappChatId", String.valueOf(chatId))
        .build();
    return uhk.handle(withChatId);
}

// uhk
public Object handle(Uri uri) {
    // Передача в :link-intercept (внутренний deeplink-роутер)
    Intent intent = new Intent(":link-intercept");
    intent.setData(uri);
    context.startActivity(intent);
    // ...
}
```

**Нигде между `udk.invoke()` и `uhk.handle()` нет валидации URL.** Что бы мини-апа ни передала — оно дойдёт до deeplink-роутера приложения.

## 4. Что может передать мини-апа

`:link-intercept` — это внутренний обработчик, реагирующий на широкий набор схем. На вход можно подать:

| URL | Эффект |
|---|---|
| `max://chat/<chatId>` | Открыть чат |
| `max://profile/<userId>` | Открыть профиль |
| `max://settings/...` | Перейти в настройки |
| `https://max.ru/:auth?externalCallback=1` | Использовать MAX как Identity Provider (см. [[13-deeplinks-idp]]) |
| `https://max.ru/<любой_внутренний_путь>` | Перейти по внутреннему web-маршруту |
| `max://botadd/<botId>` | Добавить бота |
| `max://invite/<token>` | Принять invite |
| любая deeplink-команда из 386 заметок | да, любая |

Конкретный список deeplinks внутри `:link-intercept` обширный — он покрывает практически всю навигацию приложения. См. [[13-deeplinks-idp]] для частичного списка.

## 5. User-click requirement

`ijk.A()` для событий `L1 = {WebAppMaxShare, WebAppShare, WebAppDownloadFile, WebAppOpenLink, WebAppOpenMaxLink}` требует, чтобы за последние **3000 ms был зарегистрирован клик** пользователя в WebView:

```java
if (L1.contains(name)) {
    if (System.currentTimeMillis() - lastUserClickMs > 3000L) {
        // блокировка
    }
}
```

**Но** есть исключение: боты из серверного списка `webapp-exc` от этой проверки **освобождены**. См. [[550-webapp-exc-user-click-bypass]].

Это означает: «доверенные» (по серверной отметке) боты могут вызвать `WebAppOpenMaxLink` без клика пользователя — то есть полностью программно.

## 6. Сценарий злоупотребления

### Базовый случай (любой бот, обычный пользователь):

1. Пользователь открыл мини-апу любого бота
2. Мини-апа дожидается, пока пользователь кликнет в зоне WebView (любой клик считается)
3. В течение 3 секунд после клика — `WebViewHandler.postEvent("WebAppOpenMaxLink", '{"url":"https://max.ru/:auth?externalCallback=1&clientId=...&...")`
4. Пользователь оказывается на странице IDP-авторизации MAX, может «не заметив» подтвердить вход в стороннее приложение

### Усиленный случай (бот в `webapp-exc`):

1. Пользователь открывает мини-апу бота, добавленного в серверный whitelist `webapp-exc`
2. Мини-апа НЕ ждёт клика — сразу вызывает `postEvent("WebAppOpenMaxLink", ...)` со специальным URL
3. Без какого-либо подтверждения пользователь перенаправляется на:
   - чат с конкретным пользователем (с возможностью pre-fill сообщения через query)
   - внутреннюю веб-страницу с автологином
   - любой внутренний роут, который раскрывает чувствительные данные

### Через server-injection (`534`):

Сервер может через JS-injection ([[534-server-webview-js-injection]]) выполнить произвольный JS в любой мини-апе. В сочетании с этим topic'ом — сервер может **из любой загруженной мини-аппы** выполнить deeplink-навигацию пользователя без его участия.

## 7. Сравнение: `WebAppOpenMaxLink` vs `WebAppOpenLink`

| Метод | Что делает | Валидация URL | Защита |
|---|---|---|---|
| `WebAppOpenLink` | `Intent.ACTION_VIEW` (открывает во внешнем приложении) | **нет** | user-click 3s |
| `WebAppOpenMaxLink` | `:link-intercept` (внутренний роутер MAX) | **нет** | user-click 3s |

`WebAppOpenLink` менее опасен (открывает внешнее приложение, пользователь видит браузер/picker). `WebAppOpenMaxLink` опаснее — пользователь часто не отличает «навигация внутри MAX по deeplink» от «обычная навигация», поэтому атака менее заметна.

## 8. Сравнение с Telegram WebApp API

Telegram Bot API:
```
WebApp.openTelegramLink(url):
  url MUST start with https://t.me/
  иначе method.error раскрывает ошибку
```

MAX:
```
WebAppOpenMaxLink(url):
  url MAY be anything Uri.parse() accepts
  ни schema check, ни host check
```

Это **архитектурный отход от референса** — Telegram WebApp специально валидирует домен. MAX это убрал (или никогда не реализовывал).

## 9. Что нужно для исправления

Минимум:
1. Whitelist схем: `https://max.ru/`, `max://`
2. Внутри `https://max.ru/` — whitelist путей (не пускать в `:auth`, `:settings`)
3. Логирование/телеметрия по подозрительным URL для аудита

Этого нет ни в 26.15.3, ни в 26.16.0.

## 10. Риск-оценка

**HIGH**

Почему HIGH:
- Полное отсутствие валидации URL
- Доступно всем мини-аппам без специальных привилегий
- В сочетании с `webapp-exc` (550) или server-injection (534) — exploit без участия пользователя
- Целевые deeplinks могут раскрывать sensitive данные или выполнять авторизацию

Почему не CRITICAL:
- Минимальная защита через user-click для обычных ботов (3s окно)
- Нет прямого доступа к данным, только навигация
- Эксплуатация требует, чтобы пользователь открыл конкретную мини-апу

## 11. Статус в 26.16.0

Без изменений. Тот же `bek.java` → `udk.java` → `wik.java` → `uhk.java` chain, без добавления валидации.
