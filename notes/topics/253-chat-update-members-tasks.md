---
tags: [protocol, chat-update, chat-members, tasks]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Tasks.java
related:
  - "[[205-tasks-proto]]"
  - "[[252-profile-chat-create-tasks]]"
---

# ChatUpdate + ChatMembersUpdate Tasks

## ChatUpdate

| Поле | Что |
|---|---|
| `chatId` / `chatServerId` | ID чата |
| `description` / `descriptionIsNull` | описание |
| `photoToken` / `photoTokenIsNull` | фото |
| `crop` | обрезка фото |
| `pinMessageId` / `pinMessageIdIsNull` | закреплённое сообщение |
| `notifyPin` | уведомить о закреплении |
| `theme` / `themeIsNull` | **тема чата** |
| `requestId` | ID запроса |

## ChatMembersUpdate

| Поле | Что |
|---|---|
| `chatId` / `chatServerId` | ID чата |
| `userIds[]` | **ID пользователей** |
| `operation` | **операция** (add/remove/ban и т.д.) |
| `chatMemberType` | **тип участника** |
| `showHistory` | показывать историю |
| `requestId` | ID запроса |

## Что важно

1. **`ChatUpdate.theme`** — тема чата управляется через задачу. Сервер знает тему каждого чата.

2. **`ChatMembersUpdate.operation`** — операция над участниками (добавить/удалить/забанить). Сервер знает все изменения состава чата.

3. **`ChatMembersUpdate.showHistory`** — при добавлении участника можно показать/скрыть историю.

## Сводка

`ChatUpdate`: description/photoToken/pinMessageId/notifyPin/theme. `ChatMembersUpdate`: userIds[]/operation/chatMemberType/showHistory.
