---
tags: [video-messages, onelog, telemetry, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/etj.java
related:
  - "[[31-onelog-event-categories]]"
  - "[[71-call-onelog-events]]"
---

# VIDEO_MESSAGE OneLog события

`defpackage/etj.java` — аналитика видеосообщений через OneLog категория `VIDEO_MESSAGE`.

## Операции

| Операция | Что |
|---|---|
| `video_message_start_recording` | начало записи видеосообщения |
| `video_message_delete` | удаление видеосообщения |
| `video_message_hands_free_mode_on` | включение режима hands-free |
| `video_message_error` | ошибка при записи/отправке |

## Атрибуты

| Атрибут | Что |
|---|---|
| `local_message_id` | локальный ID сообщения |
| `message_id` | серверный ID сообщения |
| `source_type` | тип источника (чат/диалог/канал) |
| `source_id` | ID источника (chat_id) |
| `reason` | причина (для delete/error) |
| `startType` | тип старта записи (1 или 2) |

## Что важно

Сервер получает: когда пользователь начал запись видеосообщения, в каком чате (`source_id`), удалил ли его, и с какой ошибкой. `source_id` — это chat_id, то есть сервер знает, в каком конкретном чате пользователь записывал видеосообщение.

## Сводка

4 операции VIDEO_MESSAGE в OneLog. Сервер знает каждое начало записи видеосообщения с chat_id и каждое удаление.
