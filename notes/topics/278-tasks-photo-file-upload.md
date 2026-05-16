---
tags: [protocol, tasks, file-upload, photo-upload, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Tasks.java
related:
  - "[[205-tasks-proto]]"
  - "[[277-tasks-video-warm-history]]"
---

# PhotoUpload + FileUpload Tasks

## PhotoUpload

| Поле | Что |
|---|---|
| `file` | **путь к файлу** |
| `chatId` / `messageId` | ID чата/сообщения |
| `crop` | обрезка |
| `profile` | **для профиля** |
| `attachLocalId` | локальный ID вложения |
| `requestId` | ID запроса |

## FileUpload

| Поле | Что |
|---|---|
| `file` / `originalFile` | **пути к файлам** |
| `fileName` | **имя файла** |
| `fileId` / `audioId` | ID файла/аудио |
| `attachType` | тип вложения |
| `chatId` / `messageId` | ID чата/сообщения |
| `crop` | обрезка |
| `profile` | для профиля |
| `lastUpdatedFile` / `lastUpdatedOriginalFile` | время изменения |
| `attachLocalId` | локальный ID вложения |
| `requestId` | ID запроса |

## Что важно

1. **`FileUpload.file`/`originalFile`** — два пути: обработанный и оригинальный файл.

2. **`FileUpload.fileName`** — имя файла передаётся на сервер. Сервер знает имена загружаемых файлов.

3. **`FileUpload.lastUpdatedFile`/`lastUpdatedOriginalFile`** — время изменения файлов. Это метаданные файловой системы.

## Сводка

`PhotoUpload`: file/chatId/crop/profile. `FileUpload`: file/originalFile/fileName/fileId/attachType/lastUpdatedFile.
