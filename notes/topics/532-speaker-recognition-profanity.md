---
tags: [critical, speaker-recognition, voice-biometrics, profanity, native, surveillance, undocumented]
status: verified
severity: critical
sources:
  - findings/native/libEnhancementLibShared.exports.txt
  - findings/native/libEnhancementLibShared.strings.txt
  - native_diff/old_enh_exports.txt
  - native_diff/new_enh_exports.txt
related:
  - "[[229-lib-enhancement-exports]]"
  - "[[15-on-device-asr-kws-diarization]]"
  - "[[380-ml-features-manager]]"
  - "[[531-wiretap-chain-collect-debug-dump]]"
  - "[[28-vk-enh-decrypt-key]]"
  - "[[16-server-pushed-ml-models-in-calls]]"
---

# 532. SpeakerRecognition + ProfanityFactory + Undocumented Algorithms

## Суть

В `libEnhancementLibShared.so` скомпилированы и экспортированы **22 алгоритмические фабрики**, из которых ранее были задокументированы только ASR, KWS, Diarization, Enhancement, NS, Animoji. Критические недокументированные:

- **SpeakerRecognitionEngine** — идентификация говорящего по голосу (1:N)
- **SpeakerRecognitionVerifier** — верификация личности по голосу (1:1)
- **ProfanityFactory** — детекция мата/обсценной лексики в речи

## Экспортированные символы

```
0x188f84  vk::enh::SpeakerRecognitionEngineFactory::createAlgorithm(Config&)
0x18903c  vk::enh::SpeakerRecognitionVerifierFactory::createAlgorithm(Config&)
```

Строки в `.rodata`:
```
speaker_id                          (рядом с DiarizationEngine::createClass)
/profanity/profanity_filter         (путь модели)
AsrService::filterProfanity         (метод фильтрации)
```

## Полный список 22 алгоритмических фабрик

| # | Factory | Назначение | Документировано ранее |
|---|---------|-----------|---------------------|
| 1 | ASRFactory | Распознавание речи | ✅ topic 15 |
| 2 | AudioClassifierFactory | Классификация аудио-событий | ❌ |
| 3 | AudioDelayBufferFactory | Буферизация аудио | ❌ |
| 4 | AudioFilterFactory | Фильтрация аудио | ❌ |
| 5 | AudioMixerFactory | Микширование аудио | ❌ |
| 6 | AECFactory | Эхоподавление | ❌ |
| 7 | AGCFactory | Автоматическая регулировка усиления | ❌ |
| 8 | AnimojiFactory | Animoji/аватар | ✅ topic 15 |
| 9 | CodecFactory | Аудио-кодек | ❌ |
| 10 | DiarizationEngineFactory | Диаризация (кто когда говорил) | ✅ topic 15 |
| 11 | EffectFactory | Аудио-эффекты | ❌ |
| 12 | EnhancementFactory | Улучшение аудио | ✅ topic 15 |
| 13 | **ForcedAlignerFactory** | Привязка слов к таймкодам | ❌ |
| 14 | KWSFactory | Детекция ключевых слов | ✅ topic 70 |
| 15 | PipelineFactory | Обработка pipeline | ❌ |
| 16 | **ProfanityFactory** | Детекция мата в речи | ❌ |
| 17 | PunctFactory | Предсказание пунктуации | ✅ topic 15 |
| 18 | ResamplerFactory | Ресемплинг аудио | ❌ |
| 19 | **SpeakerRecognitionEngineFactory** | Идентификация по голосу (1:N) | ❌ |
| 20 | **SpeakerRecognitionVerifierFactory** | Верификация по голосу (1:1) | ❌ |
| 21 | **SuperResolutionFactory** | Нейросетевое улучшение аудио | ❌ |
| 22 | VfiFactory | Интерполяция видео-кадров | ❌ |

## SpeakerRecognition — детали

### Архитектура
- Использует тот же Registry-паттерн, что и все алгоритмы: `Factory::createAlgorithm(Config&)`
- `Config` — объект конфигурации, потенциально server-pushed (как для KWS/NS через `RemoteSettings`)
- Результаты сериализуются через `msgpack` (компактный бинарный формат)
- Модели шифруются через `vk::enh::encrypt()`/`decrypt()` (AES-128, ключ в `.rodata`)

### Engine vs Verifier
- **Engine** (1:N) — «кто из базы голосов это говорит?» — требует базу voiceprint-ов
- **Verifier** (1:1) — «это тот человек, за кого себя выдаёт?» — требует один эталонный voiceprint

### Интеграция в звонковый pipeline
- `libEnhancementLibShared.so` загружается через `dlopen` из `libjingle_peerconnection_so.so`
- Получает сырые аудио-сэмплы через `MicrophoneManager.registerAudioSampleCallback()`
- Работает параллельно с ASR, KWS, Diarization на том же аудио-потоке
- Активация через `Config` — тот же механизм, что `android.mlfeatures.ws_0` для KWS

### Связь с Diarization
`speaker_id` строка находится рядом с `DiarizationEngine::createClass` — вероятно, SpeakerRecognition используется для **именования** сегментов диаризации (не просто «спикер 1/2», а конкретная идентификация).

## ProfanityFactory — детали

- Модель: `/profanity/profanity_filter` (путь в строках)
- Метод: `AsrService::filterProfanity` — интегрирован в ASR-pipeline
- Работает на результатах ASR (текст) или на аудио напрямую
- Потенциально связан с `bad_call_detected_by_audio_spotter` (KWS) — но для мата, не ключевых слов

## Статус в 26.16.0

**SpeakerRecognition — БЕЗ ИЗМЕНЕНИЙ.** Обе фабрики на месте.

Удалено только:
- `BCResNetKWS` (реализация KWS)
- `BCResNetExternalStateKWS`
- `FeatureExtractor` (для KWS)

Оставлено:
- `KWSFactory` (фабрика без реализации)
- `SpeakerRecognitionEngineFactory` ✅
- `SpeakerRecognitionVerifierFactory` ✅
- `DiarizationEngineFactory` ✅
- `ProfanityFactory` ✅

## Импликации

В сочетании с [[531-wiretap-chain-collect-debug-dump]]:

1. Сервер может **молча записать звонок** (collect-debug-dump)
2. Сервер может **идентифицировать говорящего по голосу** (SpeakerRecognition)
3. Сервер может **транскрибировать с атрибуцией** (AsrOnline + Diarization)
4. Сервер может **детектировать мат** (ProfanityFactory)
5. Сервер может **привязать слова к таймкодам** (ForcedAligner)
6. Модели загружаются с сервера — «что детектируется» обновляется без релиза

Это полный стек **голосовой биометрической идентификации и анализа содержания речи**, встроенный в production-мессенджер.
