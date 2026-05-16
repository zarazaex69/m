---
tags: [calls, audio, screen-capture, video-render, speaker, proximity]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/audio/CallsAudioManager.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/video/ScreenCaptureManager.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/video/VideoRenderManager.java
related:
  - "[[179-conversation-interface]]"
  - "[[183-media-mute-participant-states]]"
---

# CallsAudioManager + ScreenCaptureManager + VideoRenderManager

## CallsAudioManager

Управление аудио-устройствами в звонке.

**State**: `IDLE` / `DIALING` / `RINGING` / `CONVERSATION`

**AudioDeviceType**: включает `isHeadsetDevice()` — определяет, является ли устройство гарнитурой.

| Метод | Что |
|---|---|
| `changeStateAsync(state, ...)` | изменить состояние |
| `getCurrentDevice()` | текущее аудио-устройство |
| `setAudioDeviceAsync(device, ...)` | выбрать устройство |
| `setAudioDeviceTypeAsync(type, ...)` | выбрать тип устройства |
| `setSpeakerEnabledAsync(enabled, byVideoTurnedOn, ...)` | включить громкоговоритель |
| `isHeadsetConnected()` | подключена ли гарнитура |
| `hasBluetoothHeadsetAsync(...)` | есть ли Bluetooth-гарнитура |
| `requestAudioFocusAsync()` | запросить аудио-фокус |
| `notifyBluetoothPermissionGranted()` | уведомить о разрешении Bluetooth |

**Listeners**: `OnAudioDeviceInfoChangeListener` / `OnAudioDeviceListChangeListener` / `OnMuteListener`

`OnMuteListener`: `onMute(bool)` / `onMutedForever()` — постоянное заглушение.

**Builder**: `setTrackProximityWhenSpeakerEnabled(bool)` — отслеживать proximity-сенсор при включённом громкоговорителе.

## ScreenCaptureManager

| Метод | Что |
|---|---|
| `isScreenCaptureEnabled()` | включён ли screenshare |
| `setScreenCaptureEnabled(enabled, isFastScreenShareEnabled)` | включить screenshare |
| `setAudioCaptureEnabled(enabled)` | **захват системного аудио** при screenshare |

`isFastScreenShareEnabled` — быстрый режим screenshare.

## VideoRenderManager

| Метод | Что |
|---|---|
| `getCallRenderer()` | рендерер звонка |
| `getEglBaseContext()` | EGL контекст |
| `getRenderers(participantId)` | рендереры участника |
| `setRenderers(key, renderers)` | установить рендереры |

## Что важно

1. **`onMutedForever()`** — постоянное заглушение. Это отдельное состояние от обычного mute.

2. **`setAudioCaptureEnabled(true)`** при screenshare — захват системного аудио устройства. Это означает, что при демонстрации экрана может захватываться и системный звук.

3. **`setTrackProximityWhenSpeakerEnabled`** — proximity-сенсор отслеживается при включённом громкоговорителе (автоматическое переключение при поднесении к уху).

## Сводка

`CallsAudioManager` — управление аудио-устройствами, `onMutedForever`, proximity tracking. `ScreenCaptureManager` — screenshare + `setAudioCaptureEnabled` (захват системного аудио). `VideoRenderManager` — рендереры видео участников.
