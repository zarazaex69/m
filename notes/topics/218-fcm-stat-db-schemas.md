---
tags: [database, fcm, push, analytics, schema]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/crc.java
  - work/jadx_base/sources/defpackage/yqc.java
related:
  - "[[19-fcm-push-payload]]"
  - "[[216-local-db-tables]]"
---

# FCM + stat_events DB схемы

## fcm_notifications

| Поле | Что |
|---|---|
| `chat_id` / `message_id` | PK |
| `type` | тип push |
| `chat_title` / `sender_user_name` / `sender_user_id` | отправитель |
| `time` | время |
| `text` | **текст уведомления** |
| `push_id` | ID push |
| `event_key` | ключ события |
| `large_image_url` | URL большого изображения |
| `fire_m` | время срабатывания |
| `has_any_error` | ошибки |
| `url` | URL |
| `bmd` | BMD данные |

## fcm_notifications_history

| Поле | Что |
|---|---|
| `chat_id` | PK |
| `last_notify_msg_id` | последнее уведомлённое сообщение |

## fcm_notifications_analytics

| Поле | Что |
|---|---|
| `push_id` | ID push |
| `chat_id` / `msg_id` | PK |
| `analytics_status` | статус аналитики |
| `suid` | ID пользователя |
| `content_length` | длина контента |
| `sent_time` / `fcm_sent_time` / `received_time` / `created_time` | временные метки |
| `push_type` | тип push |
| `event_key` | ключ события |

## stat_events

| Поле | Что |
|---|---|
| `id` | PK |
| `timestamp` | время |
| `entry` | BLOB — сериализованное событие |

## Что важно

1. **`fcm_notifications_analytics`** — 5 временных меток для каждого push: `sent_time`/`fcm_sent_time`/`received_time`/`created_time`/`time`. Полный lifecycle push-уведомления.

2. **`text`** в `fcm_notifications` — текст уведомления хранится локально.

3. **`stat_events`** — локальный буфер статистики (BLOB). Отправляется пакетами.

## Сводка

`fcm_notifications_analytics`: 5 временных меток push lifecycle. `stat_events`: BLOB-буфер статистики. `fcm_notifications`: текст/отправитель/url/bmd.
