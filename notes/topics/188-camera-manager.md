---
tags: [calls, camera, server-control, video]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/video/CameraManager.java
related:
  - "[[158-change-media-settings]]"
  - "[[179-conversation-interface]]"
---

# CameraManager — управление камерой в звонках

`CameraManager` — управление камерой во время звонка.

## Методы

| Метод | Что |
|---|---|
| `getNumberOfCameras()` | количество камер |
| `isCameraEnabled()` | включена ли камера |
| `isCapturingFromFrontCamera()` | фронтальная ли камера |
| `setCameraEnabled(bool)` | включить/выключить камеру |
| `switchCamera(cameraParams)` | переключить камеру |

## Что важно

1. **`setCameraEnabled(false)`** — может быть вызвано через `change-media-settings` (см. [[158-change-media-settings]]) — принудительное выключение камеры администратором.

2. **`isCapturingFromFrontCamera()`** — сервер знает, использует ли пользователь фронтальную или заднюю камеру.

3. **`getNumberOfCameras()`** — количество камер устройства. Это часть device fingerprint.

## Сводка

`CameraManager` — `getNumberOfCameras`/`isCameraEnabled`/`isCapturingFromFrontCamera`/`setCameraEnabled`/`switchCamera`.
