---
tags: [audio, kws, keyword-spotter, microphone, proximity, surveillance, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/audio/KeywordSpotterManager.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/audio/MicrophoneManager.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/audio/ProximityTracker.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/audio/VideoTracker.java
related:
  - "[[15-on-device-asr-kws-diarization]]"
  - "[[380-ml-features-manager]]"
  - "[[382-conversation-stats]]"
---

# Audio SDK — KeywordSpotter, Microphone, Proximity

## KeywordSpotterManager

Управление KWS (Keyword Spotting) в звонках.

### KeywordSpotterParams

| Поле | Что |
|---|---|
| `isEnabled` | включён ли KWS |
| `filePath` | **путь к файлу модели KWS** |

### KeywordSpotterConfig

| Поле | Что |
|---|---|
| `turnOffInMs` | **через сколько мс выключить KWS** |

Метод: `setKeywordSpotterParams(params)` — установить параметры KWS.

## MicrophoneManager

| Метод | Что |
|---|---|
| `isMicEnabled()` | микрофон включён |
| `setMicEnabled(bool)` | включить/выключить микрофон |
| `registerAudioSampleCallback(delay, callback)` | **подписаться на аудио-сэмплы** |
| `removeAudioSampleCallback(callback)` | отписаться |

## ProximityTracker

| Метод | Что |
|---|---|
| `startTrackingProximity()` | начать отслеживание близости |
| `stopTrackingProximity()` | остановить |
| `getCanUseSpeaker()` | можно ли использовать динамик |

## VideoTracker

`preferSpeakerOverEarpiece()` — предпочитать динамик вместо наушника.

## Что важно

1. **`registerAudioSampleCallback(delay, callback)`** — подписка на аудио-сэмплы микрофона. Это означает, что SDK может получать сырые аудио-данные.

2. **`filePath` в KeywordSpotterParams** — путь к файлу модели KWS. Модель загружается с сервера (см. [[380-ml-features-manager]]).

3. **`turnOffInMs`** — сервер может задать время автовыключения KWS.

4. **`startTrackingProximity()`** — датчик близости используется в звонках (для переключения динамик/наушник).

## Сводка

`KeywordSpotterManager`: `setKeywordSpotterParams(isEnabled, filePath)`, `KeywordSpotterConfig(turnOffInMs)`. `MicrophoneManager`: `registerAudioSampleCallback(delay, callback)` — сырые аудио-сэмплы. `ProximityTracker`: startTracking/stopTracking.
