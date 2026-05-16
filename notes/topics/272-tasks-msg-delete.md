---
tags: [protocol, tasks, msg-delete, complaint, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Tasks.java
related:
  - "[[205-tasks-proto]]"
  - "[[271-tasks-msg-edit-react]]"
---

# MsgDelete Task — удаление сообщений

## MsgDelete

| Поле | Что |
|---|---|
| `chatId` / `chatServerId` | ID чата |
| `messagesId[]` / `messagesServerId[]` | **ID удаляемых сообщений** |
| `forMe` | **только для меня** |
| `complaint` | **жалоба при удалении** |
| `itemTypeId` | тип элемента |
| `notDeleteMessageFromDb` | не удалять из БД |
| `requestId` | ID запроса |

## Что важно

1. **`forMe`** — удаление только для себя или для всех. Сервер знает, удалил ли пользователь сообщение только для себя.

2. **`complaint`** — при удалении можно одновременно подать жалобу. Это означает, что удаление + жалоба — одна операция.

3. **`notDeleteMessageFromDb`** — флаг «не удалять из БД». Это означает, что сообщение может быть скрыто, но не удалено физически.

4. **`messagesId[]`** — пакетное удаление нескольких сообщений.

## Сводка

`MsgDelete`: messagesId[]/forMe/complaint/notDeleteMessageFromDb. Удаление + жалоба в одной операции.
