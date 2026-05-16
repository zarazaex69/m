---
tags: [database, folder-chats, message-uploads, notifications-marks, schema]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/sk.java
  - work/jadx_base/sources/defpackage/t54.java
related:
  - "[[216-local-db-tables]]"
  - "[[243-chat-folder-db]]"
---

# folder_and_chats + message_uploads + notifications_read_marks DB

## folder_and_chats

| Поле | Что |
|---|---|
| `chatId` | ID чата |
| `folderId` | **ID папки** |

Связывает чаты с папками (many-to-many).

## message_uploads

| Поле | Что |
|---|---|
| `path` | **путь к файлу** |
| `last_modified` | время изменения |
| `upload_type` | тип загрузки |
| `message_id` / `chat_id` / `attach_id` | ID |
| `video_quality` | качество видео |
| `video_start_trim_position` / `video_end_trim_position` | позиции обрезки |
| `mute` | без звука |

## notifications_read_marks

| Поле | Что |
|---|---|
| `chat_id` | ID чата |
| `mark` | **отметка прочтения** |

## Что важно

1. **`folder_and_chats`** — один чат может быть в нескольких папках.

2. **`message_uploads.path`** — локальный путь к файлу. Это путь на устройстве.

3. **`notifications_read_marks`** — отметки прочтения уведомлений. Отдельная таблица от `presence`.

## Сводка

`folder_and_chats`: chatId/folderId. `message_uploads`: path/upload_type/video_quality/trim. `notifications_read_marks`: chat_id/mark.
