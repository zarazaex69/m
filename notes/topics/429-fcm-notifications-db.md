---
tags: [database, fcm, notifications, analytics, push, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/crc.java
related:
  - "[[19-fcm-push-payload]]"
  - "[[427-db-additional-tables]]"
---

# fcm_notifications + fcm_notifications_analytics — схемы таблиц

## fcm_notifications

```sql
CREATE TABLE `fcm_notifications` (
  `chat_id` INTEGER NOT NULL,
  `message_id` INTEGER NOT NULL,
  `type` TEXT NOT NULL,
  `chat_title` TEXT,
  `sender_user_name` TEXT,
  `sender_user_id` INTEGER NOT NULL,
  `time` INTEGER NOT NULL,
  `text` TEXT NOT NULL,
  `push_id` INTEGER NOT NULL,
  `event_key` TEXT,
  `large_image_url` TEXT DEFAULT NULL,
  `fire_m` INTEGER NOT NULL DEFAULT 0,
  `has_any_error` INTEGER NOT NULL
)
```

## fcm_notifications_analytics

```sql
CREATE TABLE `fcm_notifications_analytics` (
  `push_id` INTEGER NOT NULL,
  `chat_id` INTEGER NOT NULL,
  `msg_id` INTEGER NOT NULL,
  `analytics_status` INTEGER NOT NULL,
  `suid` INTEGER,
  `content_length` INTEGER NOT NULL,
  `sent_time` INTEGER,
  `event_key` TEXT,
  `fcm_sent_time` INTEGER NOT NULL,
  `received_time` INTEGER NOT NULL,
  `push_type` TEXT NOT NULL,
  `time` INTEGER NOT NULL,
  `created_time` ...
)
```

## fcm_notifications_history

```sql
CREATE TABLE `fcm_notifications_history` (
  `chat_id` INTEGER NOT NULL,
  `last_notify_msg_id` INTEGER NOT NULL,
  PRIMARY KEY(`chat_id`)
)
```

## Что важно

1. **`text`** в `fcm_notifications` — текст уведомления хранится в БД.

2. **`sender_user_name`/`sender_user_id`** — отправитель хранится в БД.

3. **`fcm_sent_time`/`received_time`** — время отправки FCM и получения. Позволяет измерить задержку доставки.

4. **`analytics_status`** — статус аналитики уведомления.

5. **`content_length`** — длина контента уведомления.

6. **`suid`** — session user ID.

## Сводка

`fcm_notifications`: chat_id/message_id/type/sender/text/push_id/event_key. `fcm_notifications_analytics`: push_id/analytics_status/fcm_sent_time/received_time/content_length. `fcm_notifications_history`: last_notify_msg_id per chat.
