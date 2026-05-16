---
tags: [telemetry, push, inbound-call, timing, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/bt1.java
related:
  - "[[207-critlog-events]]"
  - "[[19-fcm-push-payload]]"
---

# InboundCall PUSH телеметрия — тайминги входящего звонка

CritLog `PUSH`/`InboundCall` — телеметрия входящего звонка.

## Параметры

| Поле | Что |
|---|---|
| `p_op` | операция (`show`) |
| `chat_id` | ID чата |
| `call_id` | **ID звонка** |
| `show_source` | **источник показа** |
| `ttime` | **время отправки push** |
| `dtime` | **задержка доставки** (dtime - ttime) |
| `fcmdtime` | **задержка FCM** (fcmdtime - ttime) |

## Что важно

1. **`ttime`/`dtime`/`fcmdtime`** — три временные метки для измерения задержки push-уведомления о входящем звонке. Сервер знает точные задержки FCM.

2. **`show_source`** — источник показа входящего звонка (push, WS, и т.д.).

3. Это подтверждение [[19-fcm-push-payload]] — `InboundCall` push логируется с таймингами.

## Сводка

`InboundCall` PUSH: p_op/chat_id/call_id/show_source/ttime/dtime(delivery delay)/fcmdtime(FCM delay).
