---
tags: [screen-capture, media-projection, foreground-service, calls, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/android/calls/MediaProjectionService.java
related:
  - "[[23-camera-mic-screen-entry-points]]"
  - "[[101-calls-sdk-managers]]"
---

# MediaProjectionService — захват экрана в звонках

`MediaProjectionService` — Foreground Service для захвата экрана (screen sharing) в звонках.

## Методы

| Метод | Что |
|---|---|
| `onCreate()` | `startForeground(12, notification)` |
| `onDestroy()` | остановка |
| `onBind(intent)` | биндинг |

## Что важно

1. **`startForeground(12, ...)`** — notification ID 12. Сервис запускается как foreground service при захвате экрана.

2. **`getAccessor().c(60)`** — обращение к accessor с параметром 60. Вероятно, это MediaProjection token.

3. Сервис используется в звонках для screen sharing.

## Сводка

`MediaProjectionService.onCreate()` → `startForeground(12, notification)`. Foreground Service для захвата экрана в звонках.
