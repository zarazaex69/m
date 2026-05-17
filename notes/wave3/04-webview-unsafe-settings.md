---
tags: [critical, webview, ssl-bypass, mixed-content, server-control, pmskey, security]
status: verified
severity: critical
sources:
  - work/jadx_base/sources/defpackage/xyc.java
  - work/jadx_base/sources/defpackage/syc.java
  - work/jadx_base/sources/defpackage/ahk.java
  - work/jadx_base/sources/defpackage/ppk.java
  - work/jadx_base/sources/defpackage/esd.java
  - work/jadx_base/sources/defpackage/ri9.java
  - work/jadx_base/sources/defpackage/hgk.java
  - work/jadx_base/sources/one/me/chatmedia/viewer/VideoWebViewScreen.java
  - work/jadx_base/sources/one/me/webview/FaqWebViewWidget.java
related:
  - "[[534-server-webview-js-injection]]"
  - "[[03-pms-server-flags]]"
  - "[[10-webapp-jsbridge]]"
---

# WebView Unsafe Settings — полная карта

## Сводная таблица всех WebView-инстансов

| # | Класс/Экран | WebView-тип | JS | FileAccess | ContentAccess | MixedContent | Debugging | SSL bypass | JS Bridge |
|---|---|---|---|---|---|---|---|---|---|
| 1 | `ahk.java` (WebAppRootScreen — мини-аппы) | `v9g` (extends `vyc`) | ✅ true | ❌ false | default | default | ❌ false | ✅ **PmsKey** | `WebViewHandler`, `AndroidPerf`, `PrivateWebViewHandler` |
| 2 | `VideoWebViewScreen` (просмотр видео в чате) | `vyc` | ✅ true | ❌ false | ✅ **true** | ⚠️ **0 (ALWAYS_ALLOW)** | ❌ false | ✅ **PmsKey** | нет |
| 3 | `FaqWebViewWidget` (FAQ/помощь) | `vyc` | ✅ true | default | default | default | ❌ false | нет (nk6 client) | нет |
| 4 | `syc.onCreateWindow` (popup из мини-аппы) | `vyc` | **наследует** | **default (true!)** | **default (true!)** | **default** | **не задан** | ✅ **PmsKey** (через xyc) | **нет, но parent имеет** |
| 5 | `zpk` → `xpk` (рендер форматированного текста) | `xpk` (extends WebView) | **не задан** | **default** | **default** | **default** | **не задан** | нет | нет |

## 🔴 Находка 1: Server-Controlled SSL Certificate Bypass через PmsKey

**Файлы:** `xyc.java:97-119`, `ri9.java:81,162`, `ppk.java:258`, `esd.java:263`

```java
// ri9.java:81 — PmsKey определяет поведение
this.B0 = new bbi(i5, "web_app:ssl_check", bool, this.e, zjf.a(Boolean.class));

// ri9.java:162 — getter
public final boolean S() {
    return ((Boolean) this.B0.x(this, f1[15])).booleanValue();
}

// xyc.java:97-119 — WebViewClient
public void onReceivedSslError(WebView webView, SslErrorHandler sslErrorHandler, SslError sslError) {
    if (!((opk) this.b).i()) {   // i() → ri9.S() → PmsKey "web_app:ssl_check"
        sslErrorHandler.cancel();  // нормальный путь: отклонить
        // ... логирование ...
    } else {
        sslErrorHandler.proceed();  // BYPASS: принять ЛЮБОЙ сертификат!
    }
}
```

**Суть:** Сервер через PmsKey `web_app:ssl_check` может **удалённо включить принятие любых SSL-сертификатов** для всех WebView мини-приложений и VideoWebViewScreen. Это не DevMenu-флаг — это серверно-управляемый параметр из 334 PmsKey. Сервер может адресно включить его для конкретного пользователя → MITM на весь WebView-трафик без предупреждений.

**Scope:** Все WebView, использующие `xyc` как WebViewClient (мини-аппы + VideoWebViewScreen).

**Отличие от wave1/06:** В wave1 описан `isDisableWebAppSsl` как DevMenu-переключатель. Здесь — **серверный PmsKey**, который сервер может включить удалённо без участия пользователя.

---

## 🔴 Находка 2: `onCreateWindow` — popup WebView без security hardening

**Файл:** `syc.java:67-73`

```java
public final boolean onCreateWindow(WebView webView, boolean z, boolean z2, Message message) {
    vyc vycVar = new vyc(webView.getContext(), 14);
    vycVar.setWebViewClient(new xyc(this, vycVar));
    ((WebView.WebViewTransport) (message != null ? message.obj : null)).setWebView(vycVar);
    message.sendToTarget();
    return true;
}
```

**Проблема:** При `window.open()` из мини-аппы создаётся **новый WebView без каких-либо настроек безопасности:**
- `setAllowFileAccess` — **не вызван** → default = `true` (до API 30) / `false` (API 30+)
- `setAllowContentAccess` — **не вызван** → default = `true`
- `setJavaScriptEnabled` — **не вызван** → наследует от parent? Нет, это новый инстанс
- `setWebContentsDebuggingEnabled` — **не вызван**
- Нет `WebChromeClient` → нет контроля `onShowFileChooser`
- Нет JS bridge → но `setAllowContentAccess(true)` + `content://` URI = чтение локальных файлов

**Вектор:** Мини-аппа вызывает `window.open("content://...")` → popup WebView с `allowContentAccess=true` может читать content providers приложения.

---

## 🔴 Находка 3: VideoWebViewScreen — `MIXED_CONTENT_ALWAYS_ALLOW`

**Файл:** `VideoWebViewScreen.java:241-244`

```java
vycVar.getSettings().setJavaScriptEnabled(true);
vycVar.getSettings().setAllowContentAccess(true);
vycVar.getSettings().setAllowFileAccess(false);
vycVar.getSettings().setMixedContentMode(0);  // MIXED_CONTENT_ALWAYS_ALLOW
```

**Проблема:** `setMixedContentMode(0)` = `MIXED_CONTENT_ALWAYS_ALLOW` — HTTPS-страница может загружать HTTP-ресурсы без предупреждений. В сочетании с `setAllowContentAccess(true)` и `setJavaScriptEnabled(true)`:
- Видео-страница может подгружать скрипты по HTTP → инъекция JS при MITM
- `content://` доступ открыт → JS может читать content providers
- SSL bypass через PmsKey тоже работает (используется `xyc` + `esd implements opk`)

---

## 🟡 Находка 4: JS-инъекция на каждую страницу через `onPageCommitVisible`

**Файл:** `xyc.java:26-36`

```java
public void onPageCommitVisible(WebView webView, String str) {
    if (webView != null && (strA = ((o8f) ((jai) this.c).getValue()).a(webView.getContext(), czc.a)) != null) {
        webView.evaluateJavascript(strA, new wyc(0));
    }
}
```

Ресурс `czc.a` = `k2f.webview_share` (ID 2131886137) → файл `res/raw/webview_share.js`.

Содержимое: полифилл Web Share API, который вызывает `WebViewHandler.resolveShare()` — передаёт **файлы** (до 3MB) из веб-страницы в нативный код через `byte[]`. Это не уязвимость сама по себе, но:
- Инъектируется в **каждую** страницу, загруженную в WebView мини-аппы
- Даёт мини-аппе возможность передавать произвольные бинарные данные в нативный код
- В сочетании с серверной JS-инъекцией (topic 534) — сервер может вызвать `resolveShare` с произвольными данными

---

## 🟡 Находка 5: `AndroidPerf` JS Bridge — FCP tracking с traceId

**Файл:** `hgk.java`

```java
@JavascriptInterface
public final void trackFcp(long j) {
    // Отправляет First Contentful Paint timing с traceId на сервер
    hokVar.f.h(new ebd(str2, k4g.c(new u6d("fcp", Long.valueOf(j)))));
}
```

Экспонируется как `window.AndroidPerf.trackFcp()`. Сам по себе безвреден, но:
- Подтверждает, что **каждая мини-аппа** имеет доступ к JS bridge
- `traceId` привязывает timing-данные к конкретной сессии пользователя
- Инъектируется автоматически через `web_view_fcp_track.js`

---

## Полная карта @JavascriptInterface

| Класс | Метод | Bridge Name | Что делает |
|---|---|---|---|
| `upk` | `postEvent(String, String)` | `WebViewHandler` | Отправляет события из мини-аппы в нативный код (non-private) |
| `upk` | `resolveShare(String, byte[], String, String)` | `WebViewHandler` | Передаёт файлы (до 3MB binary) из веб-страницы |
| `i6e` | `postEvent(String, String)` | `PrivateWebViewHandler` | Привилегированные события (NFC, биометрия, телефон) |
| `hgk` | `trackFcp(long)` | `AndroidPerf` | First Contentful Paint timing |

---

## Полная карта evaluateJavascript()

| Файл | Контекст | Что инъектируется |
|---|---|---|
| `xyc.java:32` | `onPageCommitVisible` | `webview_share.js` — полифилл Web Share API |
| `ghk.java:99` | Серверное событие `bik` | `WebApp.sendEvent(name, data)` / `PrivateWebApp.sendEvent(name, data)` — **произвольный JS от сервера** |
| `nhk.java:74,79` | После share dialog | `window.navigator.__share__receive()` / `(abort)` |
| `igk.java:33` | Корутина | Произвольная строка JS (переменная `this.X`) |

---

## shouldOverrideUrlLoading — валидация

| Файл | Класс | Валидация |
|---|---|---|
| `xyc.java:124` | WebApp WebViewClient | Делегирует в `opk.y(Uri)` — **зависит от реализации** |
| `nk6.java:20` | FaqWebViewWidget | Проверяет scheme (`mailto`, `max://`, `https`) + MIME type. Пропускает `max://` deeplinks |

**Проблема в nk6:** Схема `max://` обрабатывается через `FaqWebViewWidget.c1(uri)` — FAQ-страница может триггерить deeplinks приложения.

---

## Итого: что НЕ было задокументировано ранее

1. **PmsKey `web_app:ssl_check` как серверный SSL bypass** — wave1/06 описывал DevMenu-переключатель, но не серверный PmsKey. Это **серверно-управляемый** MITM на WebView.
2. **`onCreateWindow` popup без security settings** — полностью новая находка. Child WebView с `allowContentAccess=true` по default.
3. **`VideoWebViewScreen` с `MIXED_CONTENT_ALWAYS_ALLOW` + `allowContentAccess(true)`** — полностью новая находка. HTTP-ресурсы в HTTPS-контексте + content:// доступ.
4. **`nk6` пропускает `max://` deeplinks из FAQ WebView** — FAQ-страница может триггерить произвольные deeplinks.
5. **`igk.java` — evaluateJavascript с произвольной строкой** — корутина выполняет JS из переменной `this.X`, источник которой — вызывающий код.
