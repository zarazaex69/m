---
tags: [protocol, tasks, scheduled-messages, fire-time, photo-change]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Tasks.java
related:
  - "[[205-tasks-proto]]"
  - "[[275-tasks-batch-ops]]"
---

# UpdateFireTimeProtoTask + ChangeProfileOrChatPhoto Tasks

## UpdateFireTimeProtoTask — отложенные сообщения

| Поле | Что |
|---|---|
| `chatId` | ID чата |
| `messageId` | ID сообщения |
| `fireTime` | **время срабатывания** |
| `notifySender` | **уведомить отправителя** |
| `requestId` | ID запроса |

## ChangeProfileOrChatPhoto

| Поле | Что |
|---|---|
| `chatId` | ID чата (0 = профиль) |
| `file` | **путь к файлу** |
| `lastModified` | время изменения файла |
| `crop` | обрезка |
| `requestId` | ID запроса |

## Что важно

1. **`UpdateFireTimeProtoTask.fireTime`** — время срабатывания отложенного сообщения. Это механизм отложенных сообщений.

2. **`UpdateFireTimeProtoTask.notifySender`** — уведомить отправителя при срабатывании.

3. **`ChangeProfileOrChatPhoto.file`** — путь к файлу фото. Это локальный путь на устройстве.

## Сводка

`UpdateFireTimeProtoTask`: chatId/messageId/fireTime/notifySender. `ChangeProfileOrChatPhoto`: chatId/file/lastModified/crop.
