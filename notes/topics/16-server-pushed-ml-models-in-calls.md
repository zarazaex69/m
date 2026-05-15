---
tags: [surveillance, server-control, ml, calls, kws, native-libs]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/ml/**
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/audio/KeywordSpotterManagerImpl.java
  - findings/native/libEnhancementLibShared.exports.txt
related:
  - "[[15-on-device-asr-kws-diarization]]"
  - "[[09-native-libs]]"
  - "[[03-pms-server-flags]]"
---

# Серверно-управляемые ML-модели в звонках (KWS / NS / DownloadService)

В дополнение к [[15-on-device-asr-kws-diarization|on-device ASR/KWS-runtime в `libEnhancementLibShared.so`]]: модели для этого runtime **скачиваются с сервера в рантайме**, не лежат в APK.

## Конфиг-формат

`ru/ok/android/externcalls/sdk/ml/config/MLFeatureConfigProviderBase.java`:

```java
public static final String URL_KEY      = "url";
public static final String CHECKSUM_KEY = "checksum"; // имя ключа Companion-а
public static final String ENABLED_KEY  = "enabled";

public MLFeatureConfig parseConfig(String config) {
    JSONObject jSONObject = new JSONObject(config);
    return new MLFeatureConfig(
        jSONObject.getString(URL_KEY),
        jSONObject.getString(CHECKSUM_KEY),
        JsonExtKt.getBooleanOrDefault(jSONObject, ENABLED_KEY, false)
    );
}
```

То есть сервер присылает JSON `{ url, checksum, enabled }` и клиент по нему качает модель.

## Серверные ключи

`ru/ok/android/externcalls/sdk/api/RemoteSettings.java`:

```java
public static final String KEY_WORDSPOTTER_CONFIG = "android.wordspotter.config";

// перечень всех конфигурационных ключей со стороны сервера:
"android.dump.bitrate"
"android.rating.limits"
"android.wordspotter.config"
"android.p2prelay.config"
"android.mlfeatures.ws_0"             // конфиг модели KWS, версия ws_0
NSFeatureConfigProvider.getRemoteKey() // конфиг модели Noise Suppression
```

Текущая версия KWS-модели: `ws_0` (`KwsFeatureDelegate.CURRENT_WS_MODEL_VERSION = "ws_0"`). Версия — часть имени ключа: `android.mlfeatures.%s`. Перевести клиент на новую модель = выкатить новый ключ `android.mlfeatures.ws_1` и в `KwsFeatureDelegate` подменить `CURRENT_WS_MODEL_VERSION` (это поле, обновляемое в новой версии клиента) или добавить ws_1 в whitelist на сервере для текущей версии.

## Файлы на устройстве

`KwsFeatureDelegate`:

```java
private static final String CONFIG_FILE_EXT       = "cfg";
public  static final String CURRENT_WS_MODEL_VERSION = "ws_0";
private static final String MODEL_FILE_EXT        = "tflite";

new ModelSpec(["tflite", "cfg"], 0L, ...)
```

То есть качаются **два файла**: `*.tflite` (нейросеть) + `*.cfg` (конфиг, видимо threshold-ы и mapping слов на индексы выхода).

## Скачивание

`ru/ok/android/externcalls/sdk/net/DownloadService.java` (импл `DownloadService$Impl`):

- HTTP GET по URL из конфига → `HttpURLConnection.getInputStream()`.
- URL валидируется через `Patterns.WEB_URL.matcher(...).matches()` (стандартный Android-паттерн URL — не whitelisting домена, любой URL подойдёт).
- Считается `MessageDigest` (алгоритм из `FileValidationConfig.getHashAlgorithm()`).
- Сравнивается с `expectedChecksum` из конфига.
- При несовпадении: `throw new RuntimeException("Downloaded model is corrupted")`.

Что это значит на практике:

- Содержимое модели целостно (checksum-pinning).
- Откуда тянуть модель — **в JSON-конфиге, который приходит с сервера**, никакой привязки к домену в коде нет.
- `enabled=false` → модель не используется. `enabled=true` → используется. Управляется одним ключом конфига.

Этот же `DownloadService` используется и для NS (Noise Suppression) — `NSFeatureDelegate.java`. То есть и шумоподавление в звонках — серверно-управляемый ML-плагин.

## Java-сторона KWS

`ru/ok/android/externcalls/sdk/audio/KeywordSpotterManagerImpl.java`:

```java
public final class KeywordSpotterManagerImpl implements KeywordSpotterManager {
    public static final String LOG_TAG = "KeywordSpotterManagerImpl";

    // сообщения в логе:
    //   "Wordspotter control config ready: ..."
    //   "Turning wordspotter off"
    //   "Error getting wordspotter control config"

    private final NativeDoubleArrayConsumer.Consumer consumer;
    // ...
}
```

И `KeywordSpotterConfig` имеет поле `getTurnOffInMs()` — модель сама себя выключает по таймеру. То есть стандартный сценарий: «включить wordspotter на N мс, потом выключить».

Конфиг wordspotter-а содержит как минимум `turnOffInMs`. Параметры (что детектировать) задаются через `KeywordSpotterParams` и через саму модель.

`org.webrtc.NativeDoubleArrayConsumer.Consumer` — нативный consumer score-ов из WebRTC pipeline. То есть:

```
[mic raw audio]  →  [WebRTC AudioDeviceModule]  →  [vk::enh::* aug pipeline]
                    →  [KWS model probability stream]
                    →  [NativeDoubleArrayConsumer]
                    →  Java callback
```

Аудио наружу (на сервер) при этом не уходит. KWS-результат — это вектор вероятностей по классам ключевых слов. Какие классы — определяется содержимым модели (`*.tflite + *.cfg`).

## Скептический разбор

1. Сам по себе keyword-spotter в звонке — нормальная UX-фича (например, voice-команды типа «Алиса/Окей Гугл» или voice-mute). Производители аудиокодеков (Krisp, Dolby Voice, OpenAI) делают похожее.
2. Что **не** нормально:
    - Модель KWS не лежит в APK, а тянется по URL из серверного конфига. Клиент не пиннится к домену.
    - В UI приложения нет видимой настройки «включить voice-команды» или списка известных hot-words. (Я не нашёл соответствующих строк локализации; нужно дополнительно проверить полный `strings.xml`.)
    - Поле `enabled` контролируется чужой стороной, и при `enabled=true` модель скачивается без явного действия пользователя при первом подключении к звонку.
    - Канал доставки модели (HTTP, любой URL) — общий механизм, который доставлять может что угодно `.tflite`-формата.
3. Что я **не подтвердил**:
    - Не нашёл фактического URL по умолчанию (нужно ловить конфиг с сервера в рантайме либо смотреть бэкап preference-ов после регистрации).
    - Не нашёл место, где KWS-callback приводит к отправке события на сервер (`ConversationKwsStat` ведёт статистику срабатываний, но это статистика, а не payload). Этот вопрос требует отдельного разбора `ConversationKwsStat` и `MLFeaturesManagerImpl`.

## Близкие сущности

- `MLFeaturesManagerImpl` — менеджер всех ML-фич в звонке.
- `MLFeatureType` — enum типов фич (KWS, NS, …).
- `MLFeatureDelegate` (абстрактный) и `KwsFeatureDelegate`, `NSFeatureDelegate`.
- `ru/ok/android/externcalls/sdk/ml/stage/DownloadStage.java`, `UnzipStage.java`, `SaveNewModelInfoStage.java` — pipeline установки модели.
- `MLDownloadStat` — телеметрия скачиваний.
- `AvailableMLFeatureInfo`, `MLModelCheckResult` — структуры состояния.

## Что это в сумме

[[09-native-libs|libEnhancementLibShared.so]] не содержит модель сам по себе; он — runtime, в который сервер докладывает «слой» обучения. KWS-runtime в звонках MAX:

- не имеет видимого UI-переключателя;
- скачивает модель по URL из серверного конфига `android.mlfeatures.ws_0` после JSON-команды `enabled=true`;
- пишет статистику срабатываний (`ConversationKwsStat`, `MLDownloadStat`).

Связанное: [[03-pms-server-flags]] — `calls-sdk-log-audio` («WIRETAP»), `calls-sdk-disable-pipeline`, `calls-sdk-dnt-disable-audio`. Канал серверного контроля над аудио в звонке — широкий.
