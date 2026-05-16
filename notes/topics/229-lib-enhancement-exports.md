---
tags: [native, asr, kws, speaker-recognition, diarization, surveillance]
status: confirmed
sources:
  - findings/native/libEnhancementLibShared.exports.txt
related:
  - "[[15-on-device-asr-kws-diarization]]"
  - "[[70-keyword-spotter-in-calls]]"
  - "[[181-noise-suppression-manager]]"
---

# libEnhancementLibShared.so — ключевые нативные компоненты

Дополнение к [[15-on-device-asr-kws-diarization]]. Подтверждённые нативные компоненты.

## ASR (Automatic Speech Recognition)

| Символ | Что |
|---|---|
| `c_enh_asr_create` | создать ASR |
| `c_enh_asr_predict` | предсказание (одно) |
| `c_enh_asr_predict_many` | предсказание (пакет) |
| `c_enh_asr_destroy` | уничтожить ASR |
| `vk::enh::ASRFactory::createAlgorithm` | фабрика ASR |
| `vk::enh::AsrService::calcConfidence` | вычислить уверенность |
| `vk::enh::AsrService::collapseCompoundWords` | объединить составные слова |
| `vk::enh::AsrService::fillPunct` | расставить пунктуацию |

## KWS (Keyword Spotter)

| Символ | Что |
|---|---|
| `vk::enh::KWSFactory::createAlgorithm` | фабрика KWS |
| `vk::enh::BCResNetKWS::score` | оценка (float/int/short) |
| `vk::enh::BCResNetKWS::extractFeatures` | извлечение признаков |
| `vk::enh::BCResNetKWS::computeProbs` | вычисление вероятностей |
| `vk::enh::KwsBufferizator::score` | буферизованная оценка |

**BCResNet** — Broadcasted Residual Network. Это нейросетевая архитектура для KWS.

## Denoise (шумоподавление)

| Символ | Что |
|---|---|
| `c_enh_denoise_create` | создать |
| `c_enh_denoise_accept_pcm` / `accept_norm_float` | принять аудио |
| `c_enh_denoise_finalize_pcm` / `finalize_float` | финализировать |

## Speaker Recognition

| Символ | Что |
|---|---|
| `SpeakerRecognitionEngineFactory::createAlgorithm` | **движок распознавания говорящего** |
| `SpeakerRecognitionVerifierFactory::createAlgorithm` | **верификатор говорящего** |

## Что важно

1. **`SpeakerRecognitionEngine`** и **`SpeakerRecognitionVerifier`** — два отдельных компонента для распознавания и верификации говорящего. Это **идентификация личности по голосу**.

2. **`BCResNetKWS`** — конкретная нейросетевая архитектура KWS. Broadcasted Residual Network — эффективная модель для on-device KWS.

3. **`AsrService::calcConfidence`** — ASR возвращает уверенность в распознавании.

4. **`AsrService::fillPunct`** — ASR расставляет пунктуацию в транскрипции.

## Сводка

`libEnhancementLibShared.so`: ASR (predict/confidence/punctuation), KWS (BCResNetKWS), Denoise, **SpeakerRecognitionEngine+Verifier** (идентификация по голосу).
