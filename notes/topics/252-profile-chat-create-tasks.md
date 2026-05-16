---
tags: [protocol, profile, chat-create, tasks]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Tasks.java
related:
  - "[[205-tasks-proto]]"
  - "[[249-msg-send-chat-tasks]]"
---

# Profile + ChatCreate Tasks

## Profile Task

| Поле | Что |
|---|---|
| `firstName` / `lastName` | имя/фамилия |
| `description` | описание |
| `link` | ссылка профиля |
| `photoId` / `photoToken` | фото |
| `avatarType` | тип аватара |
| `crop` | обрезка фото |
| `requestId` | ID запроса |

## ChatCreate Task

| Поле | Что |
|---|---|
| `chatId` / `cid` | ID чата |
| `chatType` | **тип чата** |
| `groupId` | ID группы |
| `subjectId` / `subjectType` | **субъект** (бот, пользователь и т.д.) |
| `startPayload` | **payload для старта** |
| `requestId` | ID запроса |

## Что важно

1. **`ChatCreate.startPayload`** — payload при создании чата. Это может быть deeplink-параметр для ботов.

2. **`ChatCreate.subjectType`** — тип субъекта чата. Позволяет создавать чаты с ботами, каналами и т.д.

3. **`Profile.link`** — ссылка профиля (username). Передаётся при обновлении профиля.

## Сводка

`Profile`: firstName/lastName/description/link/photoId/photoToken/avatarType/crop. `ChatCreate`: chatId/chatType/groupId/subjectId/subjectType/startPayload.
