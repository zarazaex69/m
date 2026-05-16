---
tags: [protocol, tasks, chat-hide, chat-group-mark, confirm-present]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Tasks.java
related:
  - "[[262-protos-present-gift]]"
  - "[[284-tasks-external-video-congrats]]"
---

# ChatHide + ChatGroupMark + ConfirmPresent Tasks

## ChatHide — скрытие чата

| Поле | Что |
|---|---|
| `chatId` / `chatServerId` | ID чата |
| `requestId` | ID запроса |

## ChatGroupMark — флаги группы

| Поле | Что |
|---|---|
| `chatId` | ID чата |
| `flagType` | **тип флага** |
| `enabled` | включён/выключен |
| `requestId` | ID запроса |

## ConfirmPresent — подтверждение подарка

| Поле | Что |
|---|---|
| `presentId` / `metadataId` | ID подарка |
| `chatId` / `chatServerId` | ID чата |
| `messageId` / `messageServerId` | ID сообщения |
| `accept` | **принять/отклонить** |
| `asPrivate` | **приватно** |
| `requestId` | ID запроса |

## Что важно

1. **`ChatGroupMark.flagType`** — тип флага группы. Это может быть «важный», «архив», и т.д.

2. **`ConfirmPresent.asPrivate`** — подарок можно принять приватно (не показывать другим).

## Сводка

`ChatHide`: chatId. `ChatGroupMark`: chatId/flagType/enabled. `ConfirmPresent`: presentId/accept/asPrivate.
