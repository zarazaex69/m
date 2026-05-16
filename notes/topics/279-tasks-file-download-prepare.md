---
tags: [protocol, tasks, file-download, prepare-upload, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Tasks.java
related:
  - "[[278-tasks-photo-file-upload]]"
  - "[[205-tasks-proto]]"
---

# FileDownload + PrepareFileUpload Tasks

## FileDownload

| Поле | Что |
|---|---|
| `fileId` / `audioId` / `stickerId` / `mp4GifId` | ID медиа |
| `attachId` | ID вложения |
| `fileName` | **имя файла** |
| `url` | **URL загрузки** |
| `messageId` | ID сообщения |
| `place` | **место** (откуда загружается) |
| `notCopyVideoToGallery` | **не копировать видео в галерею** |
| `notifyProgress` | уведомлять о прогрессе |
| `checkAutoloadConnection` | проверить автозагрузку |
| `invalidateCount` | счётчик инвалидации |
| `requestId` | ID запроса |

## PrepareFileUpload

| Поле | Что |
|---|---|
| `uri` | **URI файла** |
| `fileName` | **имя файла** |
| `messageId` | ID сообщения |
| `attachLocalId` | локальный ID вложения |
| `requestId` | ID запроса |

## Что важно

1. **`FileDownload.notCopyVideoToGallery`** — сервер/пользователь может запретить копирование видео в галерею.

2. **`FileDownload.place`** — сервер знает, откуда загружается файл (чат, профиль, и т.д.).

3. **`PrepareFileUpload.uri`** — URI файла перед загрузкой. Это content:// URI Android.

## Сводка

`FileDownload`: fileId/fileName/url/place/notCopyVideoToGallery. `PrepareFileUpload`: uri/fileName/messageId.
