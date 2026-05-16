---
tags: [calls, stats, errors, negotiation, audio, telemetry]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/stat/negotiation/NegotiationErrorStat.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/stat/audio/AudioErrorStat.java
related:
  - "[[163-conversation-stats]]"
---

# NegotiationErrorStat и AudioErrorStat — статистика ошибок

## NegotiationErrorStat

`onError(ssb error)` — отправляет событие с именем из `toStatName(error.a)` и SDP в JSON.

Содержит `SessionDescription.sdp` — SDP при ошибке negotiation. Это означает, что при ошибке WebRTC negotiation сервер получает полный SDP (Session Description Protocol), который включает кодеки, ICE candidates, и другие параметры сессии.

## AudioErrorStat

`report(aa0 event)` — событие `audio_error` с `string_value = event.a + ":" + event.b + ":" + event.c`.

Три поля ошибки аудио: тип, код, описание.

## Что важно

1. **SDP при ошибке negotiation** — сервер получает полный SDP при каждой ошибке WebRTC negotiation. SDP содержит детальную информацию о конфигурации медиа-сессии.

2. **`audio_error`** — сервер знает о каждой аудио-ошибке во время звонка.

3. Оба компонента входят в `ConversationStats` (см. [[163-conversation-stats]]).

## Сводка

`NegotiationErrorStat` — SDP при ошибке negotiation. `AudioErrorStat` — `audio_error` с тремя полями ошибки.
