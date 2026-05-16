---
tags: [noise-suppression, ns, audio, server-control, surveillance, calls]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/audio/internal/NoiseSuppressionManagerImpl.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/audio/NoiseSuppressionManager.java
related:
  - "[[380-ml-features-manager]]"
  - "[[394-audio-sdk-kws-mic-proximity]]"
  - "[[395-keyword-spotter-impl]]"
---

# NoiseSuppressionManagerImpl — подавление шума

`NoiseSuppressionManagerImpl` — управление подавлением шума (NS) в звонках.

## setNoiseSuppressorParams

| Параметр | Что |
|---|---|
| `serversideBasic` | **серверное базовое NS** |
| `serversideAnn` | **серверное ANN NS** (нейросетевое) |
| `clientsidePlatform` | клиентское платформенное NS |
| `clientsideAnn` | **клиентское ANN NS** (нейросетевое) |
| `enhancerKind` | тип энхансера |
| `filePath` | **путь к файлу модели NS** |
| `inputSampleRate` | частота дискретизации входа |
| `outputSampleRate` | частота дискретизации выхода |
| `logTimings` | логировать тайминги |
| `fallbackStutterCountMillis` | порог заикания для fallback |
| `fallbackTimeLimitMillis` | лимит времени fallback |
| `fallbackTimeframeMillis` | таймфрейм fallback |
| `onNoiseSuppressorDisabledDueToStutter` | callback при отключении из-за заикания |

## Что важно

1. **`serversideBasic`/`serversideAnn`** — NS может работать на сервере. Это означает, что аудио передаётся на сервер для обработки.

2. **`clientsideAnn`** — нейросетевое NS на устройстве. Модель загружается с сервера (см. [[380-ml-features-manager]], `ns_1`).

3. **`filePath`** — путь к файлу модели NS. Модель загружается с сервера.

4. **`onNoiseSuppressorDisabledDueToStutter`** — callback при отключении NS из-за заикания. Это означает, что NS может автоматически отключаться.

## Сводка

`NoiseSuppressionManagerImpl.setNoiseSuppressorParams(serversideBasic, serversideAnn, clientsidePlatform, clientsideAnn, filePath, ...)`. Серверное NS (`serversideAnn`) означает передачу аудио на сервер.
