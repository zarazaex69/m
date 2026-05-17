---
tags: [critical, webapp, screen-capture, flag-secure, jsbridge, server-control, surveillance]
status: verified
severity: high
sources:
  - work/jadx_base/sources/defpackage/xjd.java
  - work/jadx_base/sources/defpackage/pkk.java
  - work/jadx_base/sources/defpackage/bnk.java
  - work/jadx_base/sources/one/me/webapp/rootscreen/WebAppRootScreen.java
related:
  - "[[534-server-webview-js-injection]]"
  - "[[419-webapp-js-events]]"
  - "[[422-webapp-screen-capture-qr]]"
---

# 541. Мини-аппа может отключить FLAG_SECURE → скриншоты всего приложения

## Суть

JS-bridge команда `setup_screen_capture_behavior` позволяет мини-аппе (серверному веб-контенту) **снять защиту от скриншотов** (`FLAG_SECURE`) со всего окна приложения. После этого любое приложение на устройстве (или сам MAX через MediaProjection) может захватить содержимое экрана.

## Механизм

### JS-bridge вызов:
```javascript
// Мини-аппа вызывает:
WebApp.sendEvent("setup_screen_capture_behavior", {
    requestId: "...",
    isScreenCaptureEnabled: true  // СНЯТЬ защиту
});
```

### Обработка (xjd.java:186-191):
```java
if (event instanceof bnk) {
    boolean enabled = ((bnk) event).c;  // isScreenCaptureEnabled
    ijkVar.e1 = enabled;
    ((ugj) ijkVar.I0.getValue()).s(enabled);  // записывает в SharedPrefs
}
```

### Применение (u7.java):
```java
if (getBoolean("app.pinLock.screenshotEnabled", true)) {
    getWindow().clearFlags(FLAG_SECURE);  // СКРИНШОТЫ РАЗРЕШЕНЫ
} else {
    getWindow().addFlags(FLAG_SECURE);    // СКРИНШОТЫ ЗАПРЕЩЕНЫ
}
```

## Что это даёт

Когда `FLAG_SECURE` снят:
1. **Скриншоты** — любое приложение может сделать скриншот MAX (включая чаты, сообщения, контакты)
2. **Screen recording** — запись экрана захватывает содержимое MAX
3. **Screen sharing** — при демонстрации экрана видно содержимое MAX
4. **MediaProjection** — программный захват экрана работает
5. **Recents/Overview** — превью приложения в списке задач показывает реальный контент

## Цепочка удалённой активации

```
Сервер MAX
    │
    │  Открывает мини-аппу через deeplink/push (TamtamSpam)
    ▼
WebAppRootScreen загружает URL с сервера
    │
    ▼
JS в мини-аппе: WebApp.sendEvent("setup_screen_capture_behavior", {isScreenCaptureEnabled: true})
    │
    ▼
FLAG_SECURE снят → экран MAX доступен для захвата
    │
    ▼
Другое приложение (или сам MAX через MediaProjection) делает скриншот
```

## Автоматическое поведение при открытии мини-аппы

`WebAppRootScreen.onAttach()` (строка 585) — при открытии мини-аппы `FLAG_SECURE` автоматически управляется. Это означает, что сам факт открытия мини-аппы может изменить защиту скриншотов.

## Статус в 26.16.0

Без изменений.
