---
tags: [protocol, ws, notif-mark, notif-attach, read-status, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/dyb.java
  - work/jadx_base/sources/defpackage/yxb.java
related:
  - "[[20-ws-protocol-opcodes]]"
  - "[[290-notif-message-ws]]"
---

# NOTIF_MARK + NOTIF_ATTACH WS — уведомления о прочтении и вложениях

## NOTIF_MARK — уведомление о прочтении

| Поле | Что |
|---|---|
| `chat_id` | ID чата |
| `mark` | **отметка прочтения** |
| `unread` | непрочитанные |
| `userId` | **ID пользователя** |

## NOTIF_ATTACH — уведомление об обновлении вложения

| Поле | Что |
|---|---|
| `fileId` | ID файла |
| `audioId` | ID аудио |
| `videoId` | ID видео |
| `error` | ошибка |

## Что важно

1. **`NOTIF_MARK.userId`** — сервер уведомляет, кто именно прочитал сообщение. Это read receipts.

2. **`NOTIF_ATTACH`** — сервер уведомляет об обновлении статуса вложения (загрузка завершена, ошибка и т.д.).

## Сводка

`NOTIF_MARK`: chat_id/mark/unread/userId (read receipts). `NOTIF_ATTACH`: fileId/audioId/videoId/error.
