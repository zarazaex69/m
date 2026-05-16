---
tags: [bluetooth, sco, calls, audio, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/audio/internal/impl/CallsBluetoothManager.java
related:
  - "[[517-call-audio-controller]]"
  - "[[394-audio-sdk-kws-mic-proximity]]"
---

# CallsBluetoothManager — Bluetooth SCO в звонках

`CallsBluetoothManager` — управление Bluetooth SCO аудио в звонках.

## Константы

| Константа | Значение |
|---|---|
| `BLUETOOTH_SCO_TIMEOUT_MS` | **2500 мс** |
| `MAX_SCO_CONNECTION_ATTEMPTS` | **3** |

## Состояния

### HeadsetState

`None` → `Unavailable` → `Available(headset, connectionState)`

### ConnectionState

`Disconnected` → `Connected(deviceName, audioState)`

### SCOAudioState

`Disconnected` → `Connecting(attempt)` → `Connected` → `Disconnecting`

## Broadcast Actions

| Action | Что |
|---|---|
| `ACTION_CONNECTION_STATE_CHANGED` | подключение/отключение гарнитуры |
| `ACTION_AUDIO_STATE_CHANGED` | изменение состояния SCO аудио |

## Что важно

1. **`BLUETOOTH_SCO_TIMEOUT_MS = 2500`** — таймаут SCO соединения 2.5 секунды.

2. **`MAX_SCO_CONNECTION_ATTEMPTS = 3`** — максимум 3 попытки подключения.

3. **`getBluetoothDeviceName`** — получает имя Bluetooth-устройства (alias на Android 11+).

4. **`isBluetoothScoAvailableOffCall()`** — проверяет доступность SCO вне звонка.

## Сводка

`CallsBluetoothManager`: SCO timeout=2500ms, max attempts=3. States: None/Unavailable/Available(Connected/Disconnected). Actions: CONNECTION_STATE_CHANGED/AUDIO_STATE_CHANGED.
