---
tags: [protocol, tasks, file-download, share-preview, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Tasks.java
related:
  - "[[205-tasks-proto]]"
  - "[[273-tasks-video-chat-ops]]"
---

# FileDownloadCmd + MsgSharePreview + ChatPinSetVisibility Tasks

## FileDownloadCmd

| Поле | Что |
|---|---|
| `fileId` | ID файла |
| `fileName` | **имя файла** |
| `chatId` / `messageId` | ID чата/сообщения |
| `attachLocalId` | локальный ID вложения |
| `requestId` | ID запроса |

## MsgSharePreview

| Поле | Что |
|---|---|
| `messageId` | ID сообщения |
| `text` | **текст превью** |
| `requestId` | ID запроса |

## ChatPinSetVisibility

| Поле | Что |
|---|---|
| `chatServerId` | ID чата |
| `show` | показать/скрыть закреплённое |
| `requestId` | ID запроса |

## Что важно

1. **`FileDownloadCmd.fileName`** — имя файла передаётся при команде загрузки. Сервер знает, какие файлы загружает пользователь.

2. **`MsgSharePreview.text`** — текст превью при шаринге сообщения.

3. **`ChatPinSetVisibility.show`** — пользователь может скрыть/показать закреплённое сообщение.

## Сводка

`FileDownloadCmd`: fileId/fileName/chatId/messageId. `MsgSharePreview`: messageId/text. `ChatPinSetVisibility`: chatServerId/show.
