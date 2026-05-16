---
tags: [calls, stats, warmup, telemetry]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/stat/warmup/ConversationPreparedStat.java
related:
  - "[[163-conversation-stats]]"
  - "[[172-conversation-started-signaling-stat]]"
---

# ConversationPreparedStat — статистика прогрева звонка

`ConversationPreparedStat.onConversationPrepared()` — событие `call_warmup`.

- `value` = `SystemClock.elapsedRealtime() - startTimeMs` — **время прогрева звонка** (мс)

## Что важно

«Прогрев» (warmup) — предварительная подготовка WebRTC сессии до начала звонка. Это оптимизация задержки: клиент начинает подготовку заранее, чтобы звонок начался быстрее.

Сервер знает, сколько времени занял прогрев. Это метрика производительности клиента.

## Сводка

`call_warmup` — время прогрева WebRTC сессии (мс). SingleShotStat.
