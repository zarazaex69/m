---
tags: [protocol, tasks, location-stop, cancel-reaction, chat-photo]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Tasks.java
related:
  - "[[203-live-location]]"
  - "[[282-tasks-chat-mark-complain]]"
---

# LocationStop + MsgCancelReaction + ChangeChatPhoto Tasks

## LocationStop — остановка live-геолокации

| Поле | Что |
|---|---|
| `chatId` | ID чата |
| `messageId` | **ID сообщения с геолокацией** |
| `requestId` | ID запроса |

## MsgCancelReaction — отмена реакции

| Поле | Что |
|---|---|
| `chatId` / `chatServerId` | ID чата |
| `messageId` / `messageServerId` | ID сообщения |
| `requestId` | ID запроса |

## ChangeChatPhoto

| Поле | Что |
|---|---|
| `chatId` | ID чата |
| `file` | **путь к файлу** |
| `lastModified` | время изменения |
| `crop` | обрезка |
| `requestId` | ID запроса |

## Что важно

1. **`LocationStop.messageId`** — остановка live-геолокации привязана к конкретному сообщению.

2. **`MsgCancelReaction`** — отмена реакции без указания конкретной реакции (в отличие от `MsgReact`).

## Сводка

`LocationStop`: chatId/messageId. `MsgCancelReaction`: chatId/messageId. `ChangeChatPhoto`: chatId/file/lastModified/crop.
