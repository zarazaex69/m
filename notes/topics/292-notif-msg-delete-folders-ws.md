---
tags: [protocol, ws, notif-msg-delete, notif-folders, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/dq0.java
related:
  - "[[20-ws-protocol-opcodes]]"
  - "[[291-notif-mark-attach-ws]]"
---

# NOTIF_MSG_DELETE + NOTIF_FOLDERS WS

## NOTIF_MSG_DELETE — уведомление об удалении сообщений

| Поле | Что |
|---|---|
| `chat_id` | ID чата |
| `messageIds[]` | **ID удалённых сообщений** |
| `messages` | сообщения |
| `chat` | данные чата |
| `ttl` | TTL |

## NOTIF_FOLDERS — уведомление об изменении папок

| Поле | Что |
|---|---|
| `folders` | **папки** |
| `banners` | баннеры |
| `showTime` | время показа |
| `updateTime` | время обновления |

## Что важно

1. **`NOTIF_MSG_DELETE.messageIds[]`** — сервер уведомляет клиент об удалении конкретных сообщений.

2. **`NOTIF_FOLDERS.banners`** — папки могут содержать баннеры. Сервер управляет баннерами в папках.

## Сводка

`NOTIF_MSG_DELETE`: chat_id/messageIds[]/messages/chat/ttl. `NOTIF_FOLDERS`: folders/banners/showTime/updateTime.
