---
tags: [calls, stats, finish, init, telemetry, end-reason]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/stat/finish/CallFinishStat.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/stat/init/CallInitStat.java
related:
  - "[[163-conversation-stats]]"
  - "[[172-conversation-started-signaling-stat]]"
---

# CallFinishStat и CallInitStat — начало и завершение звонка

## CallFinishStat

Событие `call_finish`:

| Поле | Что |
|---|---|
| `reason` | **причина завершения** (`ConversationEndReason.getKey()`) |
| `rate_reasons` | **причины плохого качества** (через запятую) |
| `value` | текст ошибки (если есть) |

`ConversationEndReason` — enum причин завершения: `Missed`, `CallTimeout`, и другие. Если `Missed` и `isCaller=true` — заменяется на `CallTimeout`.

## CallInitStat

Событие `call_init`:

| Поле | Что |
|---|---|
| `source` | **тип звонка + isAnon** |

`source = StatExtensionsKt.asString(callType, isAnon)` — строка с типом звонка и флагом анонимности.

## Что важно

1. **`reason` в `call_finish`** — сервер знает причину завершения каждого звонка.

2. **`rate_reasons`** — список причин плохого качества (из `RateHint`). Это автоматически определённые причины (не субъективная оценка пользователя).

3. **`source` в `call_init`** — тип звонка и анонимность. Сервер знает, был ли участник анонимным.

## Сводка

`call_finish` — `reason`/`rate_reasons`/`errorText`. `call_init` — `source` (callType + isAnon).
