---
tags: [kws, keyword-spotting, calls, server-control, surveillance, ml]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/audio/KeywordSpotterManagerImpl.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/audio/internal/KeywordSpotterConfigProviderImpl.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/audio/KeywordSpotterManager.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/ml/delegate/KwsFeatureDelegate.java
related:
  - "[[15-on-device-asr-kws-diarization]]"
  - "[[16-server-pushed-ml-models-in-calls]]"
  - "[[69-remote-settings-calls-sdk]]"
---

# KeywordSpotter в звонках — server-controlled KWS с turnOffInMs

В дополнение к [[15-on-device-asr-kws-diarization]] (on-device KWS в `libEnhancementLibShared.so`) и [[16-server-pushed-ml-models-in-calls]] (server-pushed KWS модели) — здесь детали управления KWS во время звонков.

## Архитектура

`KeywordSpotterManagerImpl` — менеджер KWS во время звонка. Получает конфиг через `KeywordSpotterConfigProvider` из `RemoteSettings` (ключ `android.wordspotter.config`).

`KeywordSpotterConfig` — конфиг с единственным полем:
- `turnOffInMs` — через сколько миллисекунд **автоматически выключить KWS**

`KeywordSpotterConfigProviderImpl.parseConfig(String config)` — парсит JSON: `{"turnOffIn": <ms>}`.

## Что это значит

1. **Сервер через `android.wordspotter.config` задаёт, через сколько времени KWS выключается** во время звонка. Если `turnOffInMs=null` — KWS работает всё время звонка.

2. **KWS работает во время звонков** — это подтверждает, что keyword spotting активен не только в фоне (как в [[15-on-device-asr-kws-diarization]]), но и во время активных звонков.

3. **`NativeDoubleArrayConsumer.Consumer`** — поле в `KeywordSpotterManagerImpl`. Это callback, через который нативный KWS-pipeline передаёт вероятности обнаружения ключевых слов в Java. Это тот самый `org.webrtc.NativeDoubleArrayConsumer`, упомянутый в NEXT STEPS исходного промпта.

4. **`ConversationKwsStat`** — статистика KWS во время звонка. Сервер получает статистику срабатываний KWS.

## Связь с моделью

`KwsFeatureDelegate.CURRENT_WS_MODEL_VERSION = "ws_0"` — текущая версия KWS-модели. Ключ для загрузки модели: `android.mlfeatures.ws_0`. Это подтверждает topic 16: модель загружается по ключу `android.mlfeatures.ws_0` из PmsKey.

## ConversationKwsStat — что именно отправляется

`ConversationKwsStat.onKeyword(float confidence)` — вызывается при срабатывании KWS. Отправляет аналитическое событие:

- Событие: `bad_call_detected_by_audio_spotter`
- Значение: `confidence * 100` (процент уверенности)
- Атрибут: `string_value = "не слышу"`

Это означает: KWS в звонках настроен на детекцию фразы **«не слышу»** (или похожей). При обнаружении этой фразы с определённой уверенностью клиент отправляет аналитическое событие `bad_call_detected_by_audio_spotter` с уровнем уверенности.

Это **не** произвольный keyword spotter — это конкретная фраза для детекции плохого качества звонка. Пользователь говорит «не слышу», KWS это детектирует, и клиент автоматически логирует событие «плохой звонок».

Скептический разбор: это UX-фича для автоматического детекта проблем со звонком, а не слежка. Но факт остаётся: KWS слушает аудио во время звонков и реагирует на конкретные слова.

## Сводка

KWS активен во время звонков. Сервер через `android.wordspotter.config` задаёт `turnOffInMs` — через сколько выключить KWS. При `null` — KWS работает всё время звонка. Вероятности срабатываний передаются через `NativeDoubleArrayConsumer` в Java и логируются в `ConversationKwsStat`.
