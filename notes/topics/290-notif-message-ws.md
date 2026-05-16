---
tags: [protocol, ws, notif-message, message-notification, ttl, invisible]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/e0c.java
related:
  - "[[20-ws-protocol-opcodes]]"
  - "[[215-messages-db-schema]]"
---

# NOTIF_MESSAGE WS — уведомление о новом сообщении

`NOTIF_MESSAGE` (опкод 109) — серверное уведомление о новом/изменённом сообщении.

## Поля

| Поле | Что |
|---|---|
| `chat_id` | ID чата |
| `message` | **сообщение** |
| `chat` | данные чата |
| `mark` | отметка прочтения |
| `unread` | непрочитанные |
| `ttl` | **TTL сообщения** |
| `url` | URL |
| `prevMessageId` | **ID предыдущего сообщения** |
| `invisible` | **невидимое сообщение** |

## Что важно

1. **`invisible`** — сообщение может быть невидимым. Это служебные сообщения, не отображаемые в UI.

2. **`ttl`** — TTL сообщения передаётся в уведомлении. Сервер управляет временем жизни.

3. **`prevMessageId`** — ID предыдущего сообщения. Используется для поддержания порядка.

## Сводка

`NOTIF_MESSAGE`: chat_id/message/chat/mark/unread/ttl/url/prevMessageId/invisible.
