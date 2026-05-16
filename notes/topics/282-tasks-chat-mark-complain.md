---
tags: [protocol, tasks, chat-mark, chat-complain, read-status, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Tasks.java
related:
  - "[[205-tasks-proto]]"
  - "[[281-tasks-assets-suspend-bot]]"
---

# ChatMark + ChatComplain Tasks

## ChatMark — отметка чата как прочитанного

| Поле | Что |
|---|---|
| `chatId` / `chatServerId` | ID чата |
| `messageId` | **ID сообщения** |
| `mark` | **отметка** |
| `setAsUnread` | **отметить как непрочитанное** |
| `isReadReaction` | **прочитана ли реакция** |
| `awaitChatInCache` | ждать чат в кэше |
| `requestId` | ID запроса |

## ChatComplain — жалоба на чат

| Поле | Что |
|---|---|
| `chatId` | ID чата |
| `complaint` | **текст жалобы** |
| `requestId` | ID запроса |

## Что важно

1. **`ChatMark.isReadReaction`** — отдельная отметка прочтения реакции. Сервер знает, прочитал ли пользователь реакцию на своё сообщение.

2. **`ChatMark.setAsUnread`** — пользователь может отметить чат как непрочитанный.

3. **`ChatMark.messageId`** — отметка привязана к конкретному сообщению. Сервер знает, до какого сообщения прочитан чат.

4. **`ChatComplain.complaint`** — текст жалобы на чат.

## Сводка

`ChatMark`: chatId/messageId/mark/setAsUnread/isReadReaction. `ChatComplain`: chatId/complaint.
