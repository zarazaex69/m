---
tags: [audio-transcription, telemetry, surveillance, message-transcription]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/tvi.java
related:
  - "[[409-audio-video-transcription]]"
  - "[[472-log-controller-event-types]]"
  - "[[471-log-controller]]"
---

# tvi — Audio Transcription Analytics

`tvi` — аналитика транскрипции аудио/видео сообщений.

## Событие `AUDIO_TRANSCRIPTION.transcription_result`

| Поле | Что |
|---|---|
| `message_id` | **ID сообщения** |
| `media_id` | **ID медиа** |
| `message_type` | **тип сообщения** (аудио/видео) |
| `result_type` | **тип результата** (успех/ошибка) |
| `duration` | **длительность** медиа |
| `waiting_time` | **время ожидания** транскрипции |
| `source_id` | **ID источника** |
| `source_type` | **тип источника** |

## Что важно

1. **`waiting_time`** — время ожидания транскрипции = `System.currentTimeMillis() - startTime`. Сервер знает, сколько времени занимает транскрипция.

2. **`result_type`** — тип результата. Сервер знает, успешно ли прошла транскрипция.

3. **`message_id`/`media_id`** — сервер знает, какое конкретное сообщение было транскрибировано.

4. **`source_id`/`source_type`** — источник транскрипции.

## Сводка

`tvi`: `AUDIO_TRANSCRIPTION.transcription_result` → `{message_id, media_id, message_type, result_type, duration, waiting_time, source_id, source_type}`.
