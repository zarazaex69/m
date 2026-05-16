---
tags: [database, organizations, informer-banner, notifications-tracker, tasks, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/crc.java
related:
  - "[[427-db-additional-tables]]"
  - "[[430-db-presence-phones-messages]]"
---

# DB — дополнительные таблицы (organizations, informer_banner, tasks, notifications_tracker)

## organizations

```sql
CREATE TABLE `organizations` (
  `id` INTEGER NOT NULL PRIMARY KEY,
  `name` TEXT NOT NULL,
  `description` TEXT,
  `parentId` INTEGER,
  `folderTemplateId` INTEGER,
  `updateTime` INTEGER NOT NULL,
  `iconUrl` TEXT
)
```

Организации с иерархией (parentId).

## informer_banner

```sql
CREATE TABLE `informer_banner` (
  `id` TEXT NOT NULL,
  `title` TEXT NOT NULL,
  `settings` INTEGER NOT NULL DEFAULT 0,
  `description` TEXT,
  `priority` INTEGER NOT NULL,
  `repeat` INTEGER NOT NULL,
  `rerun` INTEGER NOT NULL,
  `animoji_id` INTEGER,
  `url` TEXT,
  `type` INTEGER NOT NULL,
  `click_time` INTEGER NOT NULL DEFAULT 0,
  `show_time` INTEGER NOT NULL DEFAULT 0,
  `close_time` INTEGER NOT NULL
)
```

Информационные баннеры с URL, приоритетом, временем показа/клика/закрытия.

## notifications_tracker_messages

```sql
CREATE TABLE `notifications_tracker_messages` (
  `chat_id` INTEGER NOT NULL,
  `message_id` INTEGER NOT NULL,
  `time` INTEGER NOT NULL,
  `fcm` INTEGER,
  `drop_reason` TEXT,
  PRIMARY KEY(`message_id`, `chat_id`)
)
```

Трекинг уведомлений: fcm (флаг FCM), drop_reason (причина отброса).

## tasks

```sql
CREATE TABLE `tasks` (
  `id` INTEGER NOT NULL PRIMARY KEY,
  `type` INTEGER NOT NULL,
  `status` INTEGER NOT NULL,
  `fails_count` INTEGER NOT NULL,
  `depends_request_id` INTEGER NOT NULL,
  `dependency_type` INTEGER NOT NULL,
  `data` BLOB NOT NULL,
  `created_time` INTEGER NOT NULL
)
```

Очередь задач с зависимостями и счётчиком ошибок.

## chat_folder

Папки чатов: id/title/order/emoji/filters/isHiddenForAllFolder/elements/filterSubjects/widgets/options/favorites.

## Что важно

1. **`informer_banner.url`** — баннеры содержат URL. Сервер может показывать произвольные баннеры с ссылками.

2. **`informer_banner.click_time`/`show_time`/`close_time`** — сервер знает, когда пользователь видел/кликал/закрывал баннер.

3. **`notifications_tracker_messages.drop_reason`** — причина отброса уведомления логируется.

4. **`tasks.fails_count`** — количество неудачных попыток выполнения задачи.

## Сводка

`organizations(id/name/parentId/folderTemplateId)`. `informer_banner(url/priority/click_time/show_time/close_time)`. `notifications_tracker_messages(fcm/drop_reason)`. `tasks(type/status/fails_count/data)`.
