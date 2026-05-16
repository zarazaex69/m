---
tags: [protocol, messages, send, trace-id, tasks]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Tasks.java
related:
  - "[[205-tasks-proto]]"
  - "[[248-task-types]]"
---

# MsgSend + MsgSendCallback — отправка сообщений

## MsgSend

| Поле | Что |
|---|---|
| `chatId` / `chatServerId` | ID чата |
| `messageId` | ID сообщения |
| `userId` | ID пользователя |
| `requestId` | ID запроса |
| `traceId` | **ID трассировки** |
| `lastKnownDraftTime` | время последнего черновика |
| `notify` | уведомить |

## MsgSendCallback

| Поле | Что |
|---|---|
| `buttonPosition` | позиция кнопки |
| `buttonType` | тип кнопки |

## ChatPersonalConfig

| Поле | Что |
|---|---|
| `chatId` | ID чата |
| `hideNonContactBar` | скрыть плашку «не контакт» |
| `requestId` | ID запроса |

## ChatGroupMark

| Поле | Что |
|---|---|
| `chatId` | ID чата |
| `flagType` | тип флага |
| `enabled` | включён |
| `requestId` | ID запроса |

## ChatSubscribe

| Поле | Что |
|---|---|
| `chatServerId` | серверный ID чата |
| `subscribe` | подписаться/отписаться |
| `requestId` | ID запроса |

## Что важно

1. **`MsgSend.traceId`** — каждое отправленное сообщение имеет `traceId` для distributed tracing.

2. **`MsgSend.lastKnownDraftTime`** — время последнего черновика передаётся при отправке.

3. **`ChatPersonalConfig.hideNonContactBar`** — пользователь может скрыть плашку «не контакт» для конкретного чата.

## Сводка

`MsgSend`: chatId/messageId/userId/traceId/lastKnownDraftTime/notify. `ChatPersonalConfig`: hideNonContactBar. `ChatGroupMark`: flagType/enabled.
