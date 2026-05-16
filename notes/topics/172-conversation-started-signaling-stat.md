---
tags: [calls, stats, start, signaling, telemetry]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/stat/start/ConversationStartedStat.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/stat/connect/ConversationConnectedToSignalingStat.java
related:
  - "[[163-conversation-stats]]"
  - "[[164-signaling-transport-stat]]"
---

# ConversationStartedStat и ConversationConnectedToSignalingStat

## ConversationStartedStat

Событие `call_start`:
- `value` — время (duration)
- `string_value` — JSON `{"labels": [callType, warmupStatus]}`

`callType` — тип звонка (1-1, group, и т.п.).
`warmupStatus` — статус прогрева (был ли звонок подготовлен заранее).

## ConversationConnectedToSignalingStat

Событие `signaling_connected`:
- `value` — `SystemClock.elapsedRealtime() - startTimeMs` — **время до подключения к сигналингу** (мс)

## Что важно

1. **`call_start` с `callType` и `warmupStatus`** — сервер знает тип каждого звонка и был ли он «прогрет» (pre-warmed).

2. **`signaling_connected` с временем** — сервер знает, сколько времени заняло подключение к сигналингу. Это метрика производительности.

3. Оба события — `SingleShotStat` — отправляются только один раз за звонок.

## Сводка

`call_start` — тип звонка + warmup status. `signaling_connected` — время до подключения к сигналингу.
