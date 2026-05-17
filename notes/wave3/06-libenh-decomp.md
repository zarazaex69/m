---
tags: [native-libs, decomp, speaker-recognition, biometrics, wav2lip, face, deepfake, voiceprint, diarization, tflite]
status: confirmed
sources:
  - work/apktool_arm64/lib/arm64-v8a/libEnhancementLibShared.so
  - native_diff/old/libEnhancementLibShared.so (26.15.3, 5717800 bytes)
  - native_diff/new/libEnhancementLibShared.so (26.16.0, 5655616 bytes)
related:
  - "[[09-native-libs]]"
  - "[[15-on-device-asr-kws-diarization]]"
  - "[[16-server-pushed-ml-models-in-calls]]"
---

# Глубокая декомпиляция libEnhancementLibShared.so (5.7 MB)

Инструменты: `rizin 0.8.2`, `radare2`, `rz-bin`, `strings`. Файл: arm64-v8a, NDK r27d, clang 18.0.4.

## 1. Полная карта vk::enh:: подпространств (80 классов)

```
vk::enh::AEC / AECFactory / AECQueue     — Acoustic Echo Cancellation
vk::enh::AGC / AGCFactory                 — Automatic Gain Control
vk::enh::Animoji / AnimojiFactory         — Face animation (Wav2Lip backend)
vk::enh::ASR / ASRFactory / ASRPrediction — Conformer-CTC ASR
vk::enh::AsrService                       — Full ASR pipeline (predict/align/punct/profanity/VAD/acoustic events)
vk::enh::AudioClassifier / AudioClassifierFactory / AudioClassifierContract
vk::enh::AudioBuffer / AudioBufferView / AudioMeta
vk::enh::AudioDelayBuffer / AudioDelayBufferFactory
vk::enh::AudioFilter / AudioFilterFactory
vk::enh::AudioMixer / AudioMixerFactory
vk::enh::BCResNetKWS / BCResNetKWSParams  — ❌ УДАЛЁН в 26.16.0
vk::enh::BCResNetExternalStateKWS / Params — ❌ УДАЛЁН в 26.16.0
vk::enh::Blob
vk::enh::Codec / CodecFactory
vk::enh::Config / ConfigInput
vk::enh::Context / Ctx / CtxObjectBase / CtxObjectImpl
vk::enh::CPUImage                         — Image container (fromPtr, clone, save, to(Device))
vk::enh::Df / DfParams                    — Deep Filtering (noise suppression, TFLite-based)
vk::enh::DiarizationEngine / DiarizationEngineFactory — Speaker diarization
vk::enh::dll / DllHandle                  — Dynamic library loader
vk::enh::Effect / EffectFactory
vk::enh::encrypt / decrypt                — Symmetric cipher for model/data blobs
vk::enh::EnhancementAlgorithm / EnhancementFactory / EnhancementMessage
vk::enh::FBankWrapper                     — Filter bank features
vk::enh::FeatureExtractor                 — ❌ УДАЛЁН в 26.16.0
vk::enh::FileStream
vk::enh::ForcedAligner / ForcedAlignerFactory
vk::enh::GTCRNTF / gtcrntfParams          — GTCRN (Gated Temporal Convolutional Recurrent Network) noise filter
vk::enh::Hook
vk::enh::Image                            — Image::fromPtr(ptr, w, h, channels, Format)
vk::enh::IntDecimator / IntDecimatorParams
vk::enh::KWS / KWSFactory / KwsBufferizator — ❌ УДАЛЁН в 26.16.0
vk::enh::MfccWrapper                      — MFCC features (Kaldi)
vk::enh::Nnet                             — Neural network wrapper
vk::enh::Pipeline / PipelineFactory
vk::enh::Profanity / ProfanityFactory     — Profanity filter (мат-фильтр)
vk::enh::Punct / PunctFactory             — Punctuation restoration
vk::enh::Resampler / ResamplerFactory
vk::enh::SpeakerRecognitionEngine / SpeakerRecognitionEngineFactory
vk::enh::SpeakerRecognitionVerifier / SpeakerRecognitionVerifierFactory
vk::enh::SRCResampler
vk::enh::SuperResolution / SuperResolutionFactory — Image/video upscaling
vk::enh::TfLite / TfLiteInterpreterWrapper / TfLiteModelWrapper / TfLiteTensorWrapper / TfLiteModelParams
vk::enh::Vfi / VfiFactory                 — Video Frame Interpolation
vk::enh::Wav2LipV1 / V3 / V4 / Params    — Lip-sync deepfake (audio→face animation)
vk::enh::WordpieceTokenizer               — BPE tokenizer for ASR
```

## 2. SpeakerRecognitionEngine / Verifier — декомпиляция

### Архитектура

Два отдельных класса:
- **SpeakerRecognitionEngine** — извлечение эмбеддингов (voiceprint) из аудио
- **SpeakerRecognitionVerifier** — сравнение эмбеддингов (верификация говорящего)

Оба создаются через Registry-паттерн (фабрика по строковому имени `algorithm_name` из Config):

```
SpeakerRecognitionEngineFactory::createAlgorithm(Config const&)  @ 0x00188f84
  → Config::getFieldString("algorithm_name")
  → Registry<string, shared_ptr<SpeakerRecognitionEngine>>::createClass(name, config)

SpeakerRecognitionVerifierFactory::createAlgorithm(Config const&) @ 0x0018903c
  → аналогично
```

### Где хранятся voiceprints

Voiceprints (эмбеддинги) **не сохраняются на диск внутри этой библиотеки**. Библиотека:
1. Принимает PCM-аудио через `c_enh_pipeline_accept_pcm`
2. Запускает pipeline через `c_enh_pipeline_run`
3. **Возвращает вектор float'ов** через `c_enh_pipeline_get_vec_vec_float` (@ 0x00126ce4, 700 bytes)

Это generic embedding extractor — Java-сторона получает `vec<vec<float>>` и решает, что с ним делать (отправить на сервер, сохранить в SharedPreferences, сравнить локально). Библиотека — stateless compute engine.

Строка `speaker_id` присутствует как config-поле — используется для атрибуции в diarization.

### encrypt/decrypt (@ 0x001302dc / 0x00130360)

Обе функции по 132 байта. Структура:
1. Выделяют 0xE0 байт на стеке (224 bytes — достаточно для AES-256 context)
2. Загружают **hardcoded ключ** из `.rodata` @ 0x83513 (16 байт бинарных данных: `6f 8c c4 b7 19 d4 0d 16 d1 fc b9 ba bb c4 7d 7e`)
3. Вызывают `fcn.0055c2c0` (инициализация cipher context)
4. Вызывают `fcn.0055c868` (собственно шифрование/дешифрование)
5. Перемещают результат в output vector

**Ключ зашит в бинарник** — это XOR или простой блочный шифр для обфускации .tflite моделей при хранении на диске. Не криптографическая защита.

## 3. c_enh_pipeline_get_vec_vec_float — generic embedding extractor

```c
// @ 0x00126ce4, size 700 bytes
// Signature (reconstructed):
int c_enh_pipeline_get_vec_vec_float(
    void* pipeline,          // pipeline handle
    const char* output_name, // string key (strlen called)
    float** out_data,        // output pointer
    size_t* out_count        // output size
);
```

Это единственный способ извлечь embeddings из pipeline. Java-сторона вызывает его после `c_enh_pipeline_run` для получения:
- Speaker embeddings (для SpeakerRecognitionEngine)
- Audio classifier probabilities
- Любых других float-векторов из pipeline

## 4. Face/Image Processing — Wav2Lip deepfake engine

### ЧТО ЭТО

**Wav2Lip** — это нейросеть для генерации реалистичного движения губ на видео по аудио. Три версии: V1, V3, V4. Это **deepfake-технология** для видеозвонков.

### Компоненты

- `vk::enh::Animoji` — базовый класс face-анимации
- `vk::enh::Wav2LipV1/V3/V4` — конкретные реализации lip-sync
- `vk::enh::animoji_messages::AnimojiMessage` — сообщения между компонентами
- `vk::enh::animoji_messages::SetInternalBufferMessage` — передача буфера кадров
- `cache_landmarks` — кэширование face landmarks между кадрами
- `vk::enh::CPUImage` — контейнер изображений (save, clone, to(Device))
- `vk::enh::Image::fromPtr(ptr, w, h, channels, Format)` — создание из raw pixels

### Обработка изображений

Библиотека содержит:
- **stb_image** (полный набор: PNG, JPEG, BMP, GIF, HDR, PSD, TGA, PIC)
- `CPUImage::save(const char*)` — сохранение изображений на диск
- `Image::fromPtr` — загрузка из raw pixel buffer
- `detection_postprocess` — TFLite Detection PostProcess (NMS, bounding boxes)

### TFLite Detection PostProcess

Строки из `tensorflow/lite/kernels/detection_postprocess.cc`:
```
detection_boxes->type
num_boxes
num_classes
max_detections
max_classes_per_detection
detections_per_class
nms_iou_threshold
nms_score_threshold
TFLite_Detection_PostProcess
```

Это **object detection** pipeline (SSD-style). В контексте Wav2Lip используется для **face detection** — нахождение лица в кадре перед применением lip-sync.

### Вывод по face/image

**Есть face detection** (TFLite Detection PostProcess) + **face landmarks** (`cache_landmarks`) + **lip-sync deepfake** (Wav2Lip V1/V3/V4). Это НЕ face recognition для идентификации — это face animation для видеозвонков (аватары/маски/lip-sync).

## 5. Liveness / Antispoofing / Deepfake detection

**НЕТ.** Строки `liveness`, `antispoofing`, `morph`, `deepfake` — **отсутствуют**. Библиотека **создаёт** deepfakes (Wav2Lip), но не **детектирует** их.

## 6. TFLite модели — что загружается

Библиотека содержит полный TFLite runtime (с XNNPACK delegate + NNAPI delegate). Модели загружаются из файлов по путям из Config:

Известные pipeline paths из strings:
```
/asr/conformer_ctc_bpe_128    — ASR модель (Conformer-CTC, BPE vocab 128)
/asr/pipeline                  — ASR pipeline config
/align/am_gmm                  — Forced aligner (GMM acoustic model)
/profanity/profanity_filter    — Мат-фильтр
/punct/puc                     — Punctuation model
/vad/vad_webrtc                — VAD (WebRTC-based)
```

Модели шифруются через `vk::enh::encrypt/decrypt` с hardcoded ключом при хранении на диске.

Config fields для моделей:
- `algorithm_name` — выбор реализации в Registry
- `model_params`, `model_rec_field`, `model_stride`
- `enable_xnnpack` — использовать XNNPACK delegate
- `batch_size`, `sample_rate`, `window_size`

## 7. Hardcoded URLs/hosts

**НЕТ hardcoded URLs** в native библиотеке. Единственные URL — ссылки на tensorflow.org в error messages и android.googlesource.com в compiler version string. Все реальные endpoints живут в Java-слое.

## 8. Diff 26.15.3 → 26.16.0

| Метрика | 26.15.3 | 26.16.0 | Δ |
|---------|---------|---------|---|
| Размер | 5,717,800 | 5,655,616 | −62 KB |
| Экспортов (FUNC) | 396 | 370 | −26 |

### Удалено в 26.16.0:
- `vk::enh::BCResNetKWS` (класс + конструктор + extractFeatures + doScoreData + 3× score)
- `vk::enh::BCResNetExternalStateKWS` (аналогичный набор)
- `vk::enh::BCResNetKWSParams` / `BCResNetExternalStateKWSParams`
- `vk::enh::FeatureExtractor` (spectrogram, extract, fft, buildHammingWindow)
- `vk::enh::buildMelBasis`
- `vk::enh::KwsBufferizator<float/int/short>::score`

### Сохранено без изменений в 26.16.0:
- ✅ SpeakerRecognitionEngine / Verifier
- ✅ DiarizationEngine
- ✅ AudioClassifier
- ✅ Wav2Lip V1/V3/V4 + Animoji
- ✅ ASR (Conformer-CTC)
- ✅ Profanity filter
- ✅ Df / GTCRNTF (noise suppression)
- ✅ SuperResolution
- ✅ VFI (Video Frame Interpolation)
- ✅ encrypt/decrypt
- ✅ Detection PostProcess (face detection)

### Добавлено в 26.16.0:
Ничего нового.

## 9. Emotion / Age / Gender / Child voice detection

**НЕТ.** Строки `emotion`, `age_`, `gender`, `child`, `kid`, `minor`, `young`, `adult`, `elderly`, `sentiment` — **отсутствуют** (кроме `gxbaby` — мусор из binary data).

Единственное исключение: `AVERAGE_POOL_2D` — это TFLite op, не label.

## 10. AudioClassifier — что классифицирует

`vk::enh::AudioClassifier` + `AudioClassifierContract` — generic audio event classifier.

C API:
```c
c_enh_audio_classifier_create(config)
c_enh_audio_classifier_predict(classifier, audio)
c_enh_audio_classifier_number_of_events_per_timestamp(classifier)
c_enh_audio_classifier_result_destroy(result)
```

Конкретные labels/categories **не зашиты в бинарник** — они приходят из .tflite модели, которая загружается с сервера. Это означает, что сервер может в любой момент подменить модель на классификатор чего угодно (speech/music/noise/baby_cry/gunshot/etc.) без обновления приложения.

## 11. Profanity Filter

`vk::enh::Profanity` + `AsrService::filterProfanity(AlignResult, Ctx)` — фильтрация мата в транскрипции. Работает post-ASR на уровне aligned words.

## 12. Ключевые находки (НЕ задокументированные ранее)

### 12.1 Wav2Lip = production deepfake engine в мессенджере

Три версии (V1, V3, V4) lip-sync deepfake **в production-сборке**. Принимает аудио + face landmarks → генерирует реалистичное движение губ. Включает face detection (TFLite Detection PostProcess) + landmark caching. Это не «фильтр» — это полноценный deepfake pipeline для подмены лица в видеозвонках.

### 12.2 Hardcoded encryption key для моделей

Ключ `6f 8c c4 b7 19 d4 0d 16 d1 fc b9 ba bb c4 7d 7e` (16 bytes) зашит в .rodata. Используется для encrypt/decrypt .tflite моделей. Любой, кто извлечёт этот ключ, может расшифровать все модели, скачанные приложением.

### 12.3 Voiceprints возвращаются в Java через generic float vector API

`c_enh_pipeline_get_vec_vec_float` — единственный выход для embeddings. Библиотека не хранит voiceprints — она возвращает float-вектор в Java, где решается: отправить на сервер, сохранить локально, или сравнить. Это означает, что voiceprints **могут утекать на сервер** без какого-либо контроля со стороны native-кода.

### 12.4 AudioClassifier с серверно-обновляемой моделью

Модель классификатора загружается из файла (путь из Config). Поскольку модели качаются с сервера (см. topic 16), сервер может в любой момент заменить модель на детектор любых звуков — без обновления APK.

### 12.5 SuperResolution + VFI = video enhancement pipeline

Upscaling + frame interpolation для видео. В контексте звонков — улучшение качества при плохом канале. Но в связке с Wav2Lip — потенциально для улучшения качества deepfake-видео.

### 12.6 Source tree path leak

```
/home/good/mainframer/webrtc4/src/modules/audio_processing/one-ann-audio-processing/...
```

Build machine: `good` (username), `mainframer` (CI system — Mainframer by Nickolay Kucheriaviy, remote build tool). Проект: `webrtc4` — четвёртая итерация кастомного WebRTC. Подпроект: `one-ann-audio-processing` — «one» = ONE (бренд мессенджера до ребрендинга в MAX), «ann» = artificial neural network.

## Итоговые ответы

| Вопрос | Ответ |
|--------|-------|
| Face/image biometrics? | **Нет face recognition/identification.** Есть face detection + landmarks для Wav2Lip deepfake. |
| Child voice detection? | **Нет.** Никаких age/gender/child labels. |
| Emotion detection? | **Нет.** |
| Где хранятся voiceprints? | **Нигде в native.** Возвращаются как `vec<vec<float>>` в Java через `c_enh_pipeline_get_vec_vec_float`. Java решает куда их деть. |
| Новое vs задокументированное? | Wav2Lip deepfake engine (3 версии), hardcoded encryption key, face detection pipeline — ранее не декомпилировались в деталях. |
