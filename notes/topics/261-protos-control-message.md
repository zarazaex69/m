---
tags: [protocol, control-message, protos, schema, chat-events]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Protos.java
related:
  - "[[206-protos-schemas]]"
  - "[[256-protos-chat-schema]]"
---

# Protos.Control — системные сообщения чата

`Protos.Control` — системные (служебные) сообщения в чате.

## Типы событий

`NEW`(1), `ADD`(2), `REMOVE`(3), `LEAVE`(4), `TITLE`(5), `ICON`(6), `HELLO`(7), `SYSTEM`(8), `JOIN_BY_LINK`(9), `PIN`(10), `BOT_STARTED`(11), `UNKNOWN`(0).

## Типы чатов

`CHAT_TYPE`(1), `CHANNEL_TYPE`(2), `GROUP_CHAT_TYPE`(3), `DIALOG`(4), `UNKNOWN_TYPE`(0).

## Поля

| Поле | Что |
|---|---|
| `event` | тип события |
| `chatType` | тип чата |
| `userId` | **ID пользователя** |
| `userIds[]` | **ID пользователей** |
| `pinnedMessageId` / `pinnedMessageServerId` | закреплённое сообщение |
| `showHistory` | показывать историю |

## Что важно

1. **`JOIN_BY_LINK`(9)** — системное сообщение при входе по ссылке. Сервер знает, что пользователь вошёл по ссылке.

2. **`BOT_STARTED`(11)** — системное сообщение при запуске бота.

3. **`HELLO`(7)** — приветственное системное сообщение.

4. **`showHistory`** — при добавлении пользователя можно показать/скрыть историю.

## Сводка

`Protos.Control`: 12 типов событий (NEW/ADD/REMOVE/LEAVE/TITLE/ICON/HELLO/SYSTEM/JOIN_BY_LINK/PIN/BOT_STARTED). Поля: event/chatType/userId/userIds[]/pinnedMessageId/showHistory.
