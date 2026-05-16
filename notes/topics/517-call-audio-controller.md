---
tags: [bluetooth, audio, calls, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/kf4.java
related:
  - "[[394-audio-sdk-kws-mic-proximity]]"
  - "[[446-call-service-impl]]"
---

# kf4 — CallAudioController (Bluetooth аудио в звонках)

`kf4` — контроллер аудио-устройств в звонках. Управляет Bluetooth-гарнитурами.

## Методы

| Метод | Что |
|---|---|
| `b(w90)` | `requestBluetoothAudio(device)` или `setAudioRoute` |
| `c()` | `setSpeakerEnabled(true)` → `setAudioRoute(8)` |
| `getAvailableAudioDevices()` | список доступных аудио-устройств |
| `getCurrentDevice()` | текущее аудио-устройство |

## Аудио-маршруты (CallAudioState)

| Маска | Что |
|---|---|
| `1` | EARPIECE |
| `2` | BLUETOOTH |
| `4` | WIRED_HEADSET |
| `8` | SPEAKER |

## Что важно

1. **`BluetoothAdapter.getDefaultAdapter().getRemoteDevice(address)`** — подключение к Bluetooth-устройству по адресу.

2. **`getSupportedBluetoothDevices()`** — список поддерживаемых Bluetooth-устройств.

3. **`setAudioRoute(8)`** — переключение на динамик.

## Сводка

`kf4`: Bluetooth audio routing в звонках. `requestBluetoothAudio(device)` / `setAudioRoute(EARPIECE/BLUETOOTH/WIRED_HEADSET/SPEAKER)`.
