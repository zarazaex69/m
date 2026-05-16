---
tags: [protocol, tasks, draft, msg-view, assets, sync, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Tasks.java
related:
  - "[[205-tasks-proto]]"
  - "[[248-task-types]]"
---

# Дополнительные Tasks — DraftSave, MsgView, AssetsAdd, SyncChatHistory

## DraftSave

| Поле | Что |
|---|---|
| `chatId` | ID чата |
| `draft` | **черновик** (bytes) |
| `requestId` | ID запроса |

Черновик сохраняется на сервере как bytes.

## MsgView

| Поле | Что |
|---|---|
| `chatId` | ID чата |
| `messageIds[]` | **ID просмотренных сообщений** |
| `registerView` | **зарегистрировать просмотр** |
| `time` | время |
| `requestId` | ID запроса |

## AssetsAdd

| Поле | Что |
|---|---|
| `id` | ID ассета |
| `assetType` | тип ассета |
| `requestId` | ID запроса |

## SyncChatHistory

| Поле | Что |
|---|---|
| `chatId` | ID чата |
| `count` | количество |
| `itemTypeId` | тип элемента |
| `taskId` | ID задачи |

## Что важно

1. **`MsgView.registerView`** — явная регистрация просмотра сообщения. Сервер знает, какие сообщения просмотрены.

2. **`MsgView.messageIds[]`** — список ID просмотренных сообщений. Это аналитика прочтения.

3. **`DraftSave.draft`** — черновик синхронизируется с сервером. Сервер хранит черновики.

## Сводка

`MsgView`: chatId/messageIds[]/registerView/time. `DraftSave`: chatId/draft(bytes). `SyncChatHistory`: chatId/count/itemTypeId.
