# 02 — Deep Reverse: libEnhancementLibShared.so

## Резюме

`libEnhancementLibShared.so` (VK, namespace `vk::enh`) — это полноценный on-device ML-фреймворк для обработки аудио/видео в реальном времени. Помимо уже задокументированных ASR/KWS/Diarization/Profanity, библиотека содержит:

1. **Speaker Recognition Engine + Verifier** — полноценная голосовая биометрия (идентификация + верификация)
2. **Deep Features (Df)** — модуль извлечения аудио-эмбеддингов (voice fingerprints)
3. **AudioClassifier** — классификатор акустических событий с multi-class detection
4. **`getAcousticEvents()`** — функция детекции акустических событий из ASR-потока
5. **`c_enh_pipeline_get_vec_vec_float`** — C API для извлечения векторов признаков (эмбеддингов) из pipeline

---

## 1. Speaker Recognition: Engine + Verifier (голосовая биометрия)

### Экспорты (присутствуют в обеих версиях 26.15.3 и 26.16.0):

```
vk::enh::SpeakerRecognitionEngineFactory::createAlgorithm(const Config&)
vk::enh::SpeakerRecognitionVerifierFactory::createAlgorithm(const Config&)
vk::enh::Registry<..., SpeakerRecognitionEngine>::createClass(...)
vk::enh::Registry<..., SpeakerRecognitionVerifier>::createClass(...)
```

### Анализ

Два отдельных компонента:
- **SpeakerRecognitionEngine** — извлечение speaker embeddings (x-vector / d-vector стиль). Использует Kaldi FBank/MFCC features + TFLite neural network.
- **SpeakerRecognitionVerifier** — сравнение эмбеддингов (verification: "это тот же человек?"). Классическая схема enroll → verify.

Строка `speaker_id` (line 4054) находится рядом с `framestep`, `inter_cache` и `DiarizationEngine::createClass` — это конфигурационные ключи для diarization, которая использует speaker embeddings для разделения говорящих.

### Surveillance-импликация

SpeakerRecognitionEngine + Verifier — это **полноценная голосовая биометрия**:
- Может создавать voiceprint (embedding) каждого участника звонка
- Может верифицировать личность по голосу
- Работает on-device, но результаты (embeddings) могут передаваться на сервер через `c_enh_pipeline_get_vec_vec_float`
- **НЕ удалена в 26.16.0** (в отличие от KWS)

---

## 2. Deep Features (Df) — модуль извлечения эмбеддингов

### Экспорты:

```
vk::enh::Df::accept(const vector<float>&)
vk::enh::Df::accept(const float*, size_t)
vk::enh::Df::accept(const short*, size_t)
vk::enh::Df::acceptMany(const vector<vector<float>>&)
vk::enh::Df::finalize()
vk::enh::Df::finalizePcm()
vk::enh::Df::handleMessage(EnhancementMessage)
vk::enh::Df::size()
vk::enh::DfParams (Deserializable config)
```

### Анализ

`Df` (Deep Features) — это `EnhancementAlgorithm`, который:
1. Принимает сырой аудио-поток (PCM int16 или float)
2. Извлекает deep feature vectors (эмбеддинги)
3. Результат доступен через `c_enh_pipeline_get_vec_vec_float` — **вектор векторов float**

Это универсальный экстрактор эмбеддингов. В зависимости от загруженной TFLite-модели может быть:
- Speaker embedding (x-vector/d-vector)
- Audio scene embedding
- Emotion embedding
- Language ID embedding

### Ключевой момент

`c_enh_pipeline_get_vec_vec_float` — это **C API для извлечения произвольных feature vectors из pipeline**. Модели загружаются с сервера (см. topic 16 — серверно-задаваемый URL). Сервер может в любой момент подменить модель на ту, которая извлекает другой тип эмбеддингов.

---

## 3. AudioClassifier — детектор акустических событий

### Экспорты:

```
c_enh_audio_classifier_create
c_enh_audio_classifier_destroy
c_enh_audio_classifier_predict
c_enh_audio_classifier_result_destroy
c_enh_audio_classifier_number_of_events_per_timestamp
vk::enh::AudioClassifierFactory::createAlgorithm(const Config&)
vk::enh::AudioClassifierContract (destructor)
vk::enh::Holder<AudioClassifier, AudioClassifierFactory>
```

### Конфигурационные строки:

```
detections_per_class
nms_score_threshold
nms_iou_threshold
num_detections_per_class > 0
num_classes > 0
num_classes_with_background
```

### Анализ

AudioClassifier — это **multi-class audio event detector**:
- Использует TFLite модель
- Поддерживает NMS (Non-Maximum Suppression) — значит, детектирует **множественные события** одновременно
- `number_of_events_per_timestamp` — возвращает количество обнаруженных событий на каждый временной шаг
- `detections_per_class` — количество детекций по каждому классу

Типичные классы для таких моделей (AudioSet/YAMNet-стиль):
- Речь / не-речь
- Крик / плач ребёнка
- Выстрелы / взрывы
- Сирены
- Стук клавиатуры
- Разбитое стекло
- Музыка

**Модель загружается с сервера** — сервер определяет, какие именно классы событий детектируются.

### Surveillance-импликация

AudioClassifier + `getAcousticEvents()` позволяют:
- Детектировать **любые акустические события** во время звонка
- Классифицировать окружающую обстановку (дом/улица/офис/транспорт)
- Обнаруживать специфические звуки (крик, выстрел, сирена)
- Результаты привязаны к `AlignResult` (временные метки) и `AudioBuffer`

---

## 4. `getAcousticEvents()` — скрытый детектор событий в ASR-потоке

### Сигнатура:

```cpp
vk::enh::AsrService::getAcousticEvents(
    const AlignResult&,
    const AudioBuffer&,
    const vector<Timestamp>&,
    const Ctx&
) const
```

### Анализ

Эта функция вызывается **внутри ASR pipeline** и:
1. Получает результат выравнивания (AlignResult — слова с временными метками)
2. Получает сырой аудио-буфер
3. Получает вектор временных меток
4. Возвращает **акустические события** (не слова, а звуки!)

Рядом в коде:
- `AsrService::getVadEvents()` — VAD-события (голос/тишина)
- `AsrService::filterProfanity()` — фильтрация мата
- `AsrService::collapseCompoundWords()` — обработка слов

**Это параллельный канал**: пока ASR распознаёт речь, `getAcousticEvents` детектирует **не-речевые звуки** в том же аудио-потоке.

---

## 5. Diff 26.15.3 → 26.16.0

### Удалено (только KWS):
```
- vk::enh::BCResNetExternalStateKWS (5 методов)
- vk::enh::BCResNetKWS (5 методов)
```

### НЕ удалено (остаётся в 26.16.0):
- ✅ SpeakerRecognitionEngine + SpeakerRecognitionVerifier
- ✅ DiarizationEngine
- ✅ AudioClassifier
- ✅ Df (Deep Features)
- ✅ getAcousticEvents()
- ✅ Profanity
- ✅ ASR (conformer_ctc_bpe_128)
- ✅ Pipeline с get_vec_vec_float
- ✅ Animoji/Wav2Lip
- ✅ GTCRNTF (noise suppression)
- ✅ Все 22 фабрики алгоритмов

### Вывод по diff

Удаление KWS — **косметическое**. Вся инфраструктура голосовой биометрии, классификации событий и извлечения эмбеддингов **осталась нетронутой**.

---

## 6. Полная архитектура модулей (22 фабрики)

| # | Factory | Назначение | Surveillance risk |
|---|---------|-----------|-------------------|
| 1 | ASRFactory | Распознавание речи | 🔴 Транскрипция |
| 2 | AudioClassifierFactory | Классификация звуков | 🔴 Детекция событий |
| 3 | CodecFactory | Кодек аудио | ⚪ |
| 4 | EnhancementFactory | Базовый алгоритм | ⚪ |
| 5 | ForcedAlignerFactory | Выравнивание слов | 🟡 Временные метки |
| 6 | PipelineFactory | Цепочка обработки | 🔴 Оркестрация |
| 7 | PunctFactory | Пунктуация | ⚪ |
| 8 | ResamplerFactory | Ресемплинг | ⚪ |
| 9 | AGCFactory | Automatic Gain Control | ⚪ |
| 10 | DiarizationEngineFactory | Разделение говорящих | 🔴 Кто говорит |
| 11 | **SpeakerRecognitionEngineFactory** | **Извлечение voiceprint** | 🔴🔴 **Биометрия** |
| 12 | **SpeakerRecognitionVerifierFactory** | **Верификация по голосу** | 🔴🔴 **Биометрия** |
| 13 | AECFactory | Echo Cancellation | ⚪ |
| 14 | SuperResolutionFactory | Улучшение видео | ⚪ |
| 15 | VfiFactory | Video Frame Interpolation | ⚪ |
| 16 | ProfanityFactory | Детекция мата | 🟡 Контент-фильтр |
| 17 | AnimojiFactory | Анимодзи/Wav2Lip | ⚪ |
| 18 | EffectFactory | Аудио-эффекты | ⚪ |
| 19 | AudioFilterFactory | Аудио-фильтры | ⚪ |
| 20 | AudioMixerFactory | Микширование | ⚪ |
| 21 | AudioDelayBufferFactory | Буфер задержки | ⚪ |
| 22 | KWSFactory | Keyword Spotting | 🔴 (удалён в 26.16.0) |

---

## 7. Feature Extraction Stack (Kaldi + TFLite)

Библиотека содержит полный Kaldi feature extraction stack:
- `vk::enh::FBankWrapper` — Filter Bank features (стандарт для speaker recognition)
- `vk::enh::MfccWrapper` — MFCC features (стандарт для ASR)
- `vk::enh::add_deltas()` — дельта-коэффициенты
- `vk::enh::apply_cmvn()` — Cepstral Mean and Variance Normalization
- `vk::enh::buildMelBasis()` — Mel-фильтры

Конфигурация:
```
mfcc_opts.frame_opts.frame_length_ms
frame_opts.frame_shift_ms
mel_opts.high_freq
mel_opts.htk_mode
frame_opts.window_type
mfcc_opts.raw_energy
mfcc_opts.use_energy
```

Это **production-grade** feature extraction для speaker verification систем.

---

## 8. DumpSource — точки перехвата аудио

Java-enum `org.webrtc.DumpSource`:
```java
IN_ENTER_PROCESSING(10)    // Вход в обработку (сырой микрофон)
IN_AFTER_NS(20)            // После шумоподавления
IN_AFTER_ANIMOJI(21)       // После Animoji
IN_EXIT_PROCESSING(30)     // Выход из обработки
OUT_ENTER_PROCESSING(1000) // Входящий аудио (от собеседника)
OUT_EXIT_PROCESSING(1010)  // Выход входящего аудио
```

`PeerConnectionFactory.submitDumpRequest(path, duration, sources, callback)` — позволяет **дампить аудио** из любой точки pipeline в файл. Это production API, не debug.

---

## 9. NativeDoubleArrayConsumer — обратный канал данных

KWS и Animoji используют `NativeDoubleArrayConsumer.Consumer` — callback из нативного кода в Java, передающий `Double[]`. Для KWS это confidence scores. Для SpeakerRecognition/Df — это может быть **embedding vector**, передаваемый обратно в Java для отправки на сервер.

---

## 10. Ключевые строки-индикаторы

| Строка | Контекст | Значение |
|--------|----------|----------|
| `speaker_id` | Рядом с DiarizationEngine | ID говорящего в diarization |
| `inter_cache` | Config key | Кэш промежуточных эмбеддингов |
| `framestep` | Config key | Шаг кадра для feature extraction |
| `detections_per_class` | AudioClassifier | Multi-class event detection |
| `nms_score_threshold` | AudioClassifier | Порог детекции |
| `num_classes` | AudioClassifier | Количество классов событий |
| `/asr/conformer_ctc_bpe_128` | ASR model path | Conformer CTC модель |
| `/profanity/profanity_filter` | Profanity config | Фильтр мата |
| `EMBEDDING_LOOKUP` | TFLite ops | Операция поиска эмбеддингов |
| `msgpack` | Serialization | Сериализация данных (эмбеддингов?) |

---

## 11. Surveillance Chain (реконструкция)

```
Микрофон → AudioBuffer
    ├── ASR Pipeline → текст + AlignResult
    │       ├── filterProfanity() → мат-события → сервер
    │       └── getAcousticEvents() → акустические события → сервер
    ├── SpeakerRecognitionEngine → speaker embedding (voiceprint)
    │       └── SpeakerRecognitionVerifier → match/no-match
    ├── DiarizationEngine → speaker_id per segment
    ├── AudioClassifier → event classes per timestamp
    ├── Df (Deep Features) → generic embeddings
    └── KWS (удалён в 26.16.0) → keyword confidence → сервер
    
Pipeline output: c_enh_pipeline_get_vec_vec_float → vector<vector<float>>
    → Java через NativeDoubleArrayConsumer или прямой JNI
    → отправка на сервер (apptracer / analytics / signaling)
```

---

## 12. Что НЕ найдено

- ❌ Явных строк `watermark`, `steganography`, `audio watermark`
- ❌ Явных строк `beamforming`, `source_localization`, `DOA` (direction of arrival)
- ❌ Явных строк `age_estimation`, `gender_estimation`, `emotion_detection`, `language_id`
- ❌ Явных строк `source_separation` (кроме diarization)

**Однако**: все эти функции могут быть реализованы через **серверно-загружаемые TFLite модели** + generic `Df` (Deep Features) extractor + `AudioClassifier`. Архитектура намеренно generic — конкретное поведение определяется моделью, а модель приходит с сервера.

---

## Выводы

1. **SpeakerRecognitionEngine + Verifier = голосовая биометрия on-device**. Не удалена в 26.16.0. Может создавать voiceprint каждого участника звонка.

2. **Df (Deep Features) = универсальный экстрактор эмбеддингов**. В зависимости от серверной модели может извлекать speaker embeddings, emotion features, language ID — что угодно.

3. **AudioClassifier = multi-class event detector**. Детектирует произвольные акустические события (классы определяются серверной моделью). NMS + multi-detection = может одновременно детектировать речь + фоновые звуки.

4. **getAcousticEvents() работает параллельно с ASR** — пока идёт транскрипция, параллельно детектируются не-речевые звуки.

5. **Удаление KWS — дымовая завеса**. Вся остальная surveillance-инфраструктура (22 фабрики, speaker biometrics, event classification, embedding extraction) осталась без изменений.

---

## Источники

- `/home/reverser/max/findings/native/libEnhancementLibShared.exports.txt` (1021 экспортов)
- `/home/reverser/max/findings/native/libEnhancementLibShared.strings.txt` (18396 строк)
- `/home/reverser/max/native_diff/old_enh_exports.txt` vs `new_enh_exports.txt`
- `/home/reverser/max/work/jadx_base/sources/org/webrtc/PeerConnectionFactory.java`
- `/home/reverser/max/work/jadx_base/sources/org/webrtc/DumpSource.java`
- `/home/reverser/max/work/jadx_base/sources/org/webrtc/NativeDoubleArrayConsumer.java`
- `/home/reverser/max/work/jadx_base/sources/ru/ok/android/externcalls/sdk/ml/MLFeaturesManagerImpl.java`
- `/home/reverser/max/work/jadx_base/sources/ru/ok/android/externcalls/sdk/ml/model/MLFeatureType.java`
- `/home/reverser/max/work/jadx_base/sources/ru/ok/android/externcalls/sdk/audio/KeywordSpotterManagerImpl.java`
