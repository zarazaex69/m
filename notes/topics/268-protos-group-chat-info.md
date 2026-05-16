---
tags: [protocol, group-chat, protos, schema, permissions, premium]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Protos.java
related:
  - "[[256-protos-chat-schema]]"
  - "[[266-protos-chat-options]]"
---

# Protos.GroupChatInfo + GroupOptions — групповые чаты

## GroupChatInfo

| Поле | Что |
|---|---|
| `groupId` | ID группы |
| `isAnswered` | отвечено ли |
| `isCustomTitle` | кастомный заголовок |
| `isImportant` | важный |
| `isMember` | является ли участником |
| `isModerator` | является ли модератором |
| `messagingPermissions` | **права на отправку сообщений** |

**messagingPermissions**: `DISABLED`(0), `MEMBERS`(1), `ALL`(2).

## GroupOptions

| Поле | Что |
|---|---|
| `groupPremium` | **группа с Premium** |

## Что важно

1. **`messagingPermissions`** — сервер контролирует, кто может отправлять сообщения в группе.

2. **`groupPremium`** — Premium-группы. Это платная функция.

3. **`isModerator`** — статус модератора хранится в структуре чата.

## Сводка

`GroupChatInfo`: groupId/isAnswered/isImportant/isMember/isModerator/messagingPermissions(DISABLED/MEMBERS/ALL). `GroupOptions`: groupPremium.
