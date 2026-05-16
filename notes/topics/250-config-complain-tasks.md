---
tags: [protocol, config, complain, tasks, user-settings]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Tasks.java
related:
  - "[[205-tasks-proto]]"
  - "[[248-task-types]]"
  - "[[247-user-settings-full]]"
---

# Config + Complain Tasks — конфигурация и жалобы

## Config Task

| Поле | Что |
|---|---|
| `chatId` / `chatServerId` | ID чата |
| `isPushToken` | обновить push-токен |
| `isUserSettings` | **обновить UserSettings** |
| `userSettings` | **Map<String, String> настроек** |
| `reset` | сброс |
| `syncChatIds` | ID чатов для синхронизации |
| `requestId` | ID запроса |

## Complain Task

| Поле | Что |
|---|---|
| `ids[]` | ID объектов жалобы |
| `serverIds[]` | серверные ID |
| `parentId` | родительский ID |
| `reasonId` | **ID причины жалобы** |
| `typeId` | **тип жалобы** |
| `details` | детали |
| `requestId` | ID запроса |

## Что важно

1. **`Config.userSettings`** — UserSettings передаются как `Map<String, String>`. Это означает, что при каждом изменении настроек клиент отправляет полный map на сервер.

2. **`Config.isPushToken`** — обновление push-токена через Config task.

3. **`Complain.reasonId`/`typeId`** — жалобы имеют типизированные причины. Сервер знает точную причину каждой жалобы.

## Сводка

`Config`: isPushToken/isUserSettings/userSettings(Map)/syncChatIds. `Complain`: ids[]/reasonId/typeId/details.
