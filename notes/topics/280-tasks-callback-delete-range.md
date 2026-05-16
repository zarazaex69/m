---
tags: [protocol, tasks, msg-callback, bot, inline-keyboard, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Tasks.java
related:
  - "[[205-tasks-proto]]"
  - "[[279-tasks-file-download-prepare]]"
---

# MsgSendCallback + MsgDeleteRange Tasks

## MsgSendCallback — callback от inline-кнопок

| Поле | Что |
|---|---|
| `messageId` | ID сообщения |
| `callbackId` | **ID callback** |
| `payload` | **payload кнопки** |
| `buttonType` | тип кнопки |
| `buttonPosition.row` / `buttonPosition.column` | **позиция кнопки** |
| `timestamp` | время |
| `requestId` | ID запроса |

## MsgDeleteRange — удаление диапазона сообщений

| Поле | Что |
|---|---|
| `chatId` | ID чата |
| `startTime` / `endTime` | **временной диапазон** |
| `itemTypeId` | тип элемента |
| `requestId` | ID запроса |

## Что важно

1. **`MsgSendCallback.payload`** — payload кнопки передаётся боту. Это данные, которые бот получает при нажатии кнопки.

2. **`MsgSendCallback.buttonPosition`** — сервер знает, на какую именно кнопку нажал пользователь (row/column).

3. **`MsgDeleteRange.startTime`/`endTime`** — удаление всех сообщений в временном диапазоне.

## Сводка

`MsgSendCallback`: messageId/callbackId/payload/buttonType/buttonPosition(row/column). `MsgDeleteRange`: chatId/startTime/endTime.
