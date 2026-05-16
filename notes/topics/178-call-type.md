---
tags: [calls, call-type, state, stats]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/Conversation.java
related:
  - "[[173-call-finish-init-stat]]"
  - "[[172-conversation-started-signaling-stat]]"
  - "[[171-ice-restart-peer-connection-stat]]"
---

# Conversation.CallType и Conversation.State — типы и состояния звонков

## CallType

| Тип | Что |
|---|---|
| `Incoming` | входящий звонок |
| `Outgoing` | исходящий звонок |
| `Join` | присоединение к существующему звонку |

## State

| Состояние | order | Что |
|---|---|---|
| `None` | 0 | нет звонка |
| `Preparing` | 1 | подготовка (warmup) |
| `Starting` | 2 | запуск |
| `Connecting` | 3 | подключение |
| `Connected` | 4 | подключён |
| `Finished` | 5 | завершён |

`State` используется в `PeerConnectionChangedStat` (см. [[171-ice-restart-peer-connection-stat]]) — `connection_state_changed`.

## Сводка

`CallType`: Incoming/Outgoing/Join. `State`: None/Preparing/Starting/Connecting/Connected/Finished (с порядком 0-5).
