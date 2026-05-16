---
tags: [protocol, ws, message-update, reactions-changed, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/fbj.java
related:
  - "[[291-notif-mark-attach-ws]]"
  - "[[20-ws-protocol-opcodes]]"
---

# UpdateMessageEvent — событие обновления сообщения

`UpdateMessageEvent` (`fbj`) — событие обновления сообщения, используемое в `NOTIF_MSG_REACTIONS_CHANGED` и `NOTIF_MSG_YOU_REACTED`.

## Поля

| Поле | Что |
|---|---|
| `chatId` | ID чата |
| `messageId` | **ID сообщения** |
| `reactionsChanged` | **изменились ли реакции** |

## Что важно

1. **`reactionsChanged`** — флаг изменения реакций. Сервер уведомляет, изменились ли реакции на сообщение.

2. Используется для `NOTIF_MSG_REACTIONS_CHANGED` (121) и `NOTIF_MSG_YOU_REACTED` (122).

## Сводка

`UpdateMessageEvent`: chatId/messageId/reactionsChanged. Используется в NOTIF_MSG_REACTIONS_CHANGED и NOTIF_MSG_YOU_REACTED.
