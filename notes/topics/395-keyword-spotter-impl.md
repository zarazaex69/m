---
tags: [kws, keyword-spotter, wordspotter, server-control, surveillance, audio, removed-in-26.16.0]
status: removed-in-26.16.0
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/audio/internal/KeywordSpotterConfigProviderImpl.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/audio/KeywordSpotterManagerImpl.java
related:
  - "[[394-audio-sdk-kws-mic-proximity]]"
  - "[[380-ml-features-manager]]"
  - "[[382-conversation-stats]]"
  - "[[384-externcalls-sdk-config]]"
  - "[[530-version-26.16.0-diff]]"
---

> ## ⚠️ В MAX 26.16.0 — УДАЛЕНО
>
> `KeywordSpotterManagerImpl`, `KeywordSpotterConfigProvider`, `setKeywordSpotterParams`, `bad_call_detected_by_audio_spotter`, нативный `BCResNetKWS::score(...)` — всё ликвидировано. См. [[530-version-26.16.0-diff]]. Заметка сохранена для исторической полноты по 26.15.3.
>
> ---


# KeywordSpotterManagerImpl — детали KWS

`KeywordSpotterManagerImpl` — реализация KWS (Keyword Spotting) в звонках.

## Конфиг с сервера

Remote key: **`android.wordspotter.config`** (JSON).

Поле: `turn_off_in_ms` — через сколько мс выключить KWS после включения.

## Логика

1. При инициализации подписывается на `android.wordspotter.config` с сервера
2. При получении конфига: если `turnOffInMs != null` → планирует автовыключение
3. При `setKeywordSpotterParams(isEnabled, filePath)`:
   - Создаёт `a49(isEnabled, filePath, consumer)` — нативный KWS
   - Если включён: планирует `scheduleTurnOff(turnOffInMs - elapsed)`
4. При срабатывании KWS: `consumer$lambda$3` → `ConversationKwsStat.onKeyword(confidence)` → `bad_call_detected_by_audio_spotter(confidence)` на сервер

## Что важно

1. **`android.wordspotter.config`** — сервер полностью контролирует KWS: включение, выключение, таймаут.

2. **`turn_off_in_ms`** — сервер задаёт время работы KWS. Это означает, что KWS может быть включён только на определённое время.

3. **`a49(isEnabled, filePath, consumer)`** — нативный KWS с callback. Callback вызывается при каждом срабатывании.

4. **`scheduleTurnOff`** — автовыключение KWS через заданное время. Время рассчитывается с учётом уже прошедшего времени звонка.

5. **Цепочка**: KWS срабатывает → `onKeyword(confidence)` → `bad_call_detected_by_audio_spotter(confidence)` → сервер.

## Сводка

`KeywordSpotterManagerImpl`: конфиг `android.wordspotter.config` → `{turn_off_in_ms}`. `setKeywordSpotterParams(isEnabled, filePath)` → нативный KWS → `onKeyword(confidence)` → `bad_call_detected_by_audio_spotter` на сервер.
