---
tags: [calls, stats, accept-call, telemetry]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/stat/accept/AcceptCallStat.java
related:
  - "[[163-conversation-stats]]"
  - "[[71-call-onelog-events]]"
---

# AcceptCallStat — статистика принятия звонка

`AcceptCallStat.onAcceptCall(isCaller, isMe, isConcurrent)` — отправляет событие `call_accepted_incoming`.

## Логика

| isCaller | isMe | isConcurrent | Событие |
|---|---|---|---|
| true | true | true | `call_accepted_incoming` = `"concurrent"` |
| true | false | false | `call_accepted_incoming` (другой вариант) |
| false | true | false | `call_accepted_incoming` (входящий) |

## Что важно

1. **`isCaller`** — является ли текущий пользователь инициатором звонка.
2. **`isMe`** — принял ли звонок текущий пользователь (или другое устройство того же аккаунта).
3. **`isConcurrent`** — параллельный звонок (multi-device).

Это означает, что сервер знает: кто принял звонок, на каком устройстве, и был ли это параллельный звонок.

## Сводка

`AcceptCallStat` — событие `call_accepted_incoming` с флагами `isCaller`/`isMe`/`isConcurrent`. Сервер знает детали принятия каждого звонка.
