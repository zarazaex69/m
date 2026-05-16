---
tags: [video, screen-capture, camera, display-layout, calls, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/video/ScreenCaptureManager.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/video/CameraManager.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/video/DisplayLayoutSender.java
related:
  - "[[23-camera-mic-screen-entry-points]]"
  - "[[371-media-projection-service]]"
  - "[[390-client-capabilities]]"
---

# Video SDK — ScreenCapture, Camera, DisplayLayout

## ScreenCaptureManager

| Метод | Что |
|---|---|
| `isScreenCaptureEnabled()` | захват экрана включён |
| `setScreenCaptureEnabled(enabled, isFastScreenShareEnabled)` | включить/выключить захват экрана |
| `setAudioCaptureEnabled(enabled)` | **включить/выключить захват аудио** при screen share |

## CameraManager

| Метод | Что |
|---|---|
| `isCameraEnabled()` | камера включена |
| `setCameraEnabled(bool)` | включить/выключить камеру |
| `getNumberOfCameras()` | количество камер |
| `isCapturingFromFrontCamera()` | фронтальная камера |
| `switchCamera(params)` | переключить камеру |

## DisplayLayoutSender

Отправляет информацию о расположении видео-окон участников на экране.

| Метод | Что |
|---|---|
| `sendDisplayLayouts(items)` | отправить список `ConversationDisplayLayoutItem` |
| `applyFilter(condition)` | фильтр отправки |

## Что важно

1. **`setAudioCaptureEnabled`** — при screen share можно захватывать аудио системы.

2. **`isFastScreenShareEnabled`** — быстрый режим screen share.

3. **`sendDisplayLayouts`** — клиент отправляет серверу информацию о том, как расположены видео-окна участников на экране. Это означает, что сервер знает, кого пользователь видит на экране.

## Сводка

`ScreenCaptureManager`: setScreenCaptureEnabled/setAudioCaptureEnabled. `CameraManager`: setCameraEnabled/switchCamera. `DisplayLayoutSender.sendDisplayLayouts(items)` — сервер знает расположение видео-окон.
