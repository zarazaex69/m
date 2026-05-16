---
tags: [database, schema, local-storage, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/
related:
  - "[[215-messages-db-schema]]"
  - "[[06-contacts]]"
---

# Локальная БД — полный список таблиц

SQLite БД приложения. 35+ таблиц.

## Все таблицы

| Таблица | Что |
|---|---|
| `messages` / `temp_messages` | сообщения (см. [[215-messages-db-schema]]) |
| `chats` | чаты |
| `chat_folder` / `folder_and_chats` | папки чатов |
| `contacts` / `phones` | контакты и телефоны |
| `profile` | профиль пользователя |
| `presence` | статус присутствия |
| `tasks` | фоновые задачи (см. [[205-tasks-proto]]) |
| `uploads` / `message_uploads` / `draft_uploads` | загрузки |
| `video_conversions` | конвертации видео |
| `stickers` / `sticker_sets` / `favorite_stickers` / `favorite_sticker_sets` / `temp_stickers` | стикеры |
| `animoji` / `animoji_set` | анимодзи |
| `fcm_notifications` / `fcm_notifications_analytics` / `fcm_notifications_history` | FCM уведомления |
| `notifications_read_marks` / `notifications_tracker_messages` | отметки уведомлений |
| `informer_banner` | информационные баннеры |
| `comments` / `message_comments` | комментарии |
| `reactions_section` | секции реакций |
| `organizations` | организации |
| `complain_reasons` | причины жалоб |
| `draft_uploads` | черновики загрузок |
| `recent` | недавние |
| `saved_msg_chat` | сохранённые сообщения |
| `selected_mentions` | выбранные упоминания |
| `stat_events` | статистические события |
| `metrics` | метрики |
| `battery` | данные батареи |
| `webapp_biometry` | **биометрия мини-приложений** |
| `access_granted` / `access_requested` | разрешения доступа |
| `analytics_status` | статус аналитики |

## Что особенно важно

1. **`webapp_biometry`** — отдельная таблица для биометрии мини-приложений. Хранит, каким мини-приложениям разрешена биометрическая аутентификация.

2. **`fcm_notifications_history`** — история FCM уведомлений хранится локально.

3. **`stat_events`** — локальный буфер статистических событий перед отправкой.

4. **`battery`** — данные батареи хранятся в БД.

5. **`presence`** — статус присутствия хранится локально.

## Сводка

35+ таблиц SQLite. Ключевые: `webapp_biometry`, `fcm_notifications_history`, `stat_events`, `battery`, `presence`, `tasks`.
