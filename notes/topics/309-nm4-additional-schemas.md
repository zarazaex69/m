---
tags: [protocol, database, parser, video-upload, fcm-notification, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/nm4.java
related:
  - "[[308-nm4-contact-presence-parser]]"
  - "[[218-fcm-stat-db-schemas]]"
---

# nm4 — дополнительные схемы (видео, FCM, загрузки)

Дополнение к [[308-nm4-contact-presence-parser]]. `nm4` парсит несколько типов данных.

## Чат (id/server_id/data)

| Поле | Что |
|---|---|
| `id` | локальный ID |
| `server_id` | серверный ID |
| `data` | BLOB данных |

## Загрузка видео

| Поле | Что |
|---|---|
| `last_modified` | время изменения |
| `upload_type` | тип загрузки |
| `message_id` / `chat_id` / `attach_id` | ID |
| `video_quality` | **качество видео** |
| `video_start_trim_position` / `video_end_trim_position` | **позиции обрезки** |
| `mute` | без звука |

## FCM уведомление

| Поле | Что |
|---|---|
| `chat_id` / `message_id` | ID |
| `type` | тип |
| `chat_title` | заголовок чата |
| `sender_user_name` / `sender_user_id` | отправитель |
| `time` | время |
| `text` | **текст** |
| `push_id` / `event_key` | ID push |
| `large_image_url` | URL изображения |
| `fire_m` | время срабатывания |
| `has_any_error` | ошибки |
| `bmd` | BMD данные |

## Что важно

1. **`text`** в FCM уведомлении — текст сообщения хранится в БД уведомлений.

2. **`video_start_trim_position`/`video_end_trim_position`** — позиции обрезки видео хранятся в БД.

## Сводка

`nm4` парсит: чаты(id/server_id/data), загрузки видео(quality/trim positions), FCM уведомления(text/sender/push_id/bmd).
