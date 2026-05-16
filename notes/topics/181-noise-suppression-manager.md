---
tags: [calls, noise-suppression, server-control, ann, webrtc]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/audio/NoiseSuppressionManager.java
related:
  - "[[160-update-media-modifiers]]"
  - "[[179-conversation-interface]]"
  - "[[45-calls-sdk-pmskey-cluster]]"
---

# NoiseSuppressionManager — детальное управление шумоподавлением

`NoiseSuppressionManager.setNoiseSuppressorParams(...)` — детальная настройка шумоподавления.

## Параметры

| Параметр | Что |
|---|---|
| `serversideBasic` | серверное базовое шумоподавление |
| `serversideAnn` | **серверное ANN шумоподавление** |
| `clientsidePlatform` | клиентское платформенное шумоподавление |
| `clientsideAnn` | **клиентское ANN шумоподавление** |
| `enhancerKind` | тип энхансера (WebRTC) |
| `filePath` | путь к файлу модели |
| `inputSampleRate` | входной sample rate |
| `outputSampleRate` | выходной sample rate |
| `fallbackTimeLimitMillis` | лимит времени до fallback |
| `fallbackStutterCountMillis` | счётчик заиканий для fallback |
| `fallbackTimeframeMillis` | временной фрейм для fallback |
| `logTimings` | логировать тайминги |
| `onNoiseSuppressorDisabledDueToStutter` | callback при отключении из-за заиканий |

## Что важно

1. **`serversideAnn` и `clientsideAnn`** — два уровня ANN (нейросетевого) шумоподавления: серверное и клиентское. Это дополнение к `update-media-modifiers` (см. [[160-update-media-modifiers]]).

2. **`filePath`** — путь к файлу модели шумоподавления. Это может быть ML-модель, загруженная с сервера (аналогично KWS/NS из [[16-server-pushed-ml-models-in-calls]]).

3. **`fallback`** параметры — автоматическое отключение шумоподавления при проблемах с производительностью.

4. **`calls-android-ns`** PmsKey (см. [[45-calls-sdk-pmskey-cluster]]) — server-gated включение шумоподавления. `NoiseSuppressionManager` — детальная реализация.

## Сводка

`NoiseSuppressionManager` — 4 режима шумоподавления: serverside/clientside × basic/ANN. `filePath` — ML-модель. Fallback при проблемах с производительностью.
