---
tags: [webapp, screen-capture, qr-code, jsbridge, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/bnk.java
  - work/jadx_base/sources/defpackage/skk.java
  - work/jadx_base/sources/defpackage/agk.java
related:
  - "[[419-webapp-js-events]]"
  - "[[421-webapp-request-phone]]"
---

# WebApp ScreenCapture + CodeReader

## WebAppSetupScreenCaptureBehavior

Мини-приложение управляет захватом экрана.

### Запрос

`WebAppSetupScreenCaptureBehaviorRequest(requestId, isScreenCaptureEnabled)`

### Ответ

`WebAppSetupScreenCaptureBehaviorResponse(requestId, isScreenCaptureEnabled)`

`ScreenCaptureBehavior(isEnabled: bool)` — включить/выключить захват экрана.

Реализация использует `FLAG_SECURE` — флаг Android для запрета скриншотов.

## WebAppOpenCodeReader

QR/штрих-код сканер для мини-приложений.

### Ответ

`WebAppOpenCodeReaderResponse(requestId, value)` — результат сканирования.

## Что важно

1. **`WebAppSetupScreenCaptureBehavior(isEnabled=false)`** — мини-приложение может запретить скриншоты через `FLAG_SECURE`.

2. **`WebAppOpenCodeReader`** — мини-приложение может запустить QR-сканер и получить результат.

3. Мини-приложение может как разрешить, так и запретить захват экрана.

## Сводка

`WebAppSetupScreenCaptureBehavior(isEnabled)` → `FLAG_SECURE`. `WebAppOpenCodeReader` → `response(requestId, value)`.
