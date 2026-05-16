---
tags: [protocol, tasks, assets, bots, stickers, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Tasks.java
related:
  - "[[205-tasks-proto]]"
  - "[[280-tasks-callback-delete-range]]"
---

# Assets Tasks + SuspendBot

## AssetsAdd / AssetsRemove

| Поле | Что |
|---|---|
| `id` | ID ассета |
| `assetType` | **тип ассета** (стикер, GIF, и т.д.) |
| `requestId` | ID запроса |

## AssetsListModify

| Поле | Что |
|---|---|
| `ids[]` | **ID ассетов** |
| `assetType` | тип ассета |
| `modifyTime` | время изменения |
| `requestId` | ID запроса |

## AssetsMove

| Поле | Что |
|---|---|
| `id` | ID ассета |
| `assetType` | тип ассета |
| `position` | **новая позиция** |
| `prevId` | **предыдущий ассет** |
| `requestId` | ID запроса |

## SuspendBot

| Поле | Что |
|---|---|
| `botId` | **ID бота** |
| `chatId` | ID чата |
| `suspend` | **приостановить/возобновить** |
| `requestId` | ID запроса |

## Что важно

1. **`AssetsMove.position`/`prevId`** — перемещение ассета в списке. Сервер знает порядок стикеров/GIF пользователя.

2. **`SuspendBot.suspend`** — пользователь может приостановить/возобновить бота.

## Сводка

`AssetsAdd/Remove/ListModify/Move`: управление ассетами (стикеры/GIF). `SuspendBot`: botId/chatId/suspend.
