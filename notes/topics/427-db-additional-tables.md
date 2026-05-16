---
tags: [database, battery, metrics, presence, fcm, analytics, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/crc.java
  - work/jadx_base/sources/defpackage/geb.java
related:
  - "[[426-webapp-biometry-db]]"
  - "[[230-db-schema-messages]]"
---

# Дополнительные таблицы БД — battery, metrics, fcm_notifications, stat_events

Дополнительные таблицы SQLite, не вошедшие в предыдущие топики.

## battery

```sql
CREATE TABLE `battery` (
  `id` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  `sliceTime` INTEGER NOT NULL,
  `utime` INTEGER NOT NULL,
  `stime` INTEGER NOT NULL,
  `batteryCapacity` INTEGER NOT NULL,
  `instantAmperage` INTEGER NOT NULL
)
```

**Срезы батареи**: время, utime/stime (CPU time), ёмкость, мгновенный ток.

## metrics

```sql
CREATE TABLE `metrics` (
  `traceId` TEXT NOT NULL PRIMARY KEY,
  `metricName` TEXT NOT NULL,
  `lastUpdatedTime` INTEGER NOT NULL,
  `spanAndPropertiesDump` BLOB NOT NULL
)
```

Метрики производительности с traceId.

## fcm_notifications / fcm_notifications_analytics / fcm_notifications_history

Три таблицы для FCM уведомлений: текущие, аналитика, история.

## stat_events

Статистические события.

## Полный список таблиц (38)

WorkerQueueItem, animoji, animoji_set, **battery**, chat_folder, chats, comments, complain_reasons, contacts, draft_uploads, favorite_sticker_sets, favorite_stickers, **fcm_notifications**, **fcm_notifications_analytics**, **fcm_notifications_history**, folder_and_chats, informer_banner, message_comments, message_uploads, messages, **metrics**, notifications_read_marks, notifications_tracker_messages, organizations, phones, presence, profile, reactions_section, recent, saved_msg_chat, selected_mentions, **stat_events**, sticker_sets, stickers, tasks, uploads, video_conversions, **webapp_biometry**

## Что важно

1. **`battery`** — срезы батареи с CPU time (utime/stime). Сервер контролирует интервал через PmsKey `battery-slice-interval`.

2. **`fcm_notifications_analytics`** — аналитика FCM уведомлений хранится локально.

3. **`stat_events`** — статистические события хранятся локально перед отправкой.

4. **`metrics`** — метрики производительности с `spanAndPropertiesDump` (BLOB).

## Сводка

38 таблиц. Новые: `battery(sliceTime/utime/stime/batteryCapacity/instantAmperage)`, `metrics(traceId/metricName/spanAndPropertiesDump)`, `fcm_notifications_analytics`, `stat_events`.
