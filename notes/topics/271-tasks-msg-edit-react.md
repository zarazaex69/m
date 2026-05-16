---
tags: [protocol, tasks, msg-edit, msg-react, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Tasks.java
related:
  - "[[205-tasks-proto]]"
  - "[[270-tasks-draft-msgview-assets]]"
---

# MsgEdit + MsgReact Tasks — редактирование и реакции

## MsgEdit

| Поле | Что |
|---|---|
| `chatId` / `chatServerId` | ID чата |
| `messageId` / `messageServerId` | ID сообщения |
| `text` | новый текст |
| `editAttaches` | редактировать вложения |
| `oldText` | **старый текст** |
| `oldAttaches` | **старые вложения** |
| `oldElements` | **старые элементы** |
| `oldStatus` | **старый статус** |
| `requestId` | ID запроса |

## MsgReact

| Поле | Что |
|---|---|
| `chatId` / `chatServerId` | ID чата |
| `messageId` / `messageServerId` | ID сообщения |
| `reaction` | **реакция** (emoji/стикер) |
| `reactionType` | тип реакции |
| `requestId` | ID запроса |

## Что важно

1. **`MsgEdit.oldText`/`oldAttaches`/`oldElements`** — при редактировании сообщения клиент отправляет **старое содержимое** на сервер. Сервер хранит историю редактирования.

2. **`MsgReact.reaction`** — конкретная реакция (emoji или стикер) передаётся на сервер.

## Сводка

`MsgEdit`: text/editAttaches/oldText/oldAttaches/oldElements/oldStatus. `MsgReact`: reaction/reactionType.
