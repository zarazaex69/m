---
tags: [protocol, ws, notif-msg-delayed, bot-commands, complain-sync, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/mel.java
related:
  - "[[20-ws-protocol-opcodes]]"
  - "[[292-notif-msg-delete-folders-ws]]"
---

# NOTIF_MSG_DELAYED + BOT_INFO + COMPLAIN_REASONS_GET WS

## NOTIF_MSG_DELAYED — уведомление об отложенном сообщении

| Поле | Что |
|---|---|
| `messageIds[]` | ID сообщений |
| `chat_id` | ID чата |
| `lastDelayedUpdateTime` | **время последнего обновления** |
| `updateTypeId` | тип обновления |
| `userId` | ID пользователя |
| `message` | сообщение |

## BOT_INFO — информация о боте

| Поле | Что |
|---|---|
| `commands` | **команды бота** |
| `contact` | контакт бота |
| `startMessage` | стартовое сообщение |

## COMPLAIN_REASONS_GET — причины жалоб

| Поле | Что |
|---|---|
| `complainSync` | синхронизация жалоб |
| `complains` | **список причин жалоб** |

## Что важно

1. **`NOTIF_MSG_DELAYED.lastDelayedUpdateTime`** — время последнего обновления отложенного сообщения.

2. **`BOT_INFO.commands`** — список команд бота. Сервер передаёт команды при открытии чата с ботом.

3. **`COMPLAIN_REASONS_GET.complains`** — список причин жалоб. Сервер управляет причинами жалоб.

## Сводка

`NOTIF_MSG_DELAYED`: messageIds[]/chat_id/lastDelayedUpdateTime/updateTypeId. `BOT_INFO`: commands/contact/startMessage. `COMPLAIN_REASONS_GET`: complainSync/complains.
