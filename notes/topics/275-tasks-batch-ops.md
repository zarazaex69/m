---
tags: [protocol, tasks, batch-ops, chat-mark, delete-batch]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Tasks.java
related:
  - "[[205-tasks-proto]]"
  - "[[274-tasks-file-share-pin]]"
---

# Batch Tasks — ChatMarkBatch, DeleteChatsBatch, ChatsList

## ChatMarkBatch

| Поле | Что |
|---|---|
| `chatIds[]` | **ID чатов** |
| `maxMark` | максимальная отметка |
| `lastFailTime` | время последней неудачи |
| `taskId` | ID задачи |

## DeleteChatsBatch

| Поле | Что |
|---|---|
| `chatIds[]` | **ID удаляемых чатов** |
| `lastFailTime` | время последней неудачи |
| `taskId` | ID задачи |

## ChatsList

| Поле | Что |
|---|---|
| `chatsSync` | время синхронизации чатов |
| `count` | количество |
| `marker` | маркер пагинации |
| `requestId` | ID запроса |

## DraftDiscard

| Поле | Что |
|---|---|
| `chatId` | ID чата |
| `requestId` | ID запроса |

## Что важно

1. **`ChatMarkBatch.chatIds[]`** — пакетная отметка нескольких чатов как прочитанных.

2. **`DeleteChatsBatch.chatIds[]`** — пакетное удаление нескольких чатов.

3. **`lastFailTime`** — время последней неудачи. Задачи повторяются при неудаче.

## Сводка

`ChatMarkBatch`: chatIds[]/maxMark. `DeleteChatsBatch`: chatIds[]. `ChatsList`: chatsSync/count/marker. `DraftDiscard`: chatId.
