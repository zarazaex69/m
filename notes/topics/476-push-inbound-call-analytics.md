---
tags: [push, inbound-call, telemetry, surveillance, fcm, call-analytics]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/bt1.java
related:
  - "[[19-fcm-push-payload]]"
  - "[[472-log-controller-event-types]]"
  - "[[446-call-service-impl]]"
---

# PUSH.InboundCall — аналитика входящих звонков через push

Событие `PUSH.InboundCall` отправляется при показе уведомления о входящем звонке.

## Структура события

| Поле | Что |
|---|---|
| `trid` | **trace ID** |
| `eKey` | **event key** |
| `suid` | **session user ID** |
| `ttime` | **timestamp push** |
| `dtime` | **задержка доставки** (ttime - send_time) |
| `fcmdtime` | **задержка FCM** (fcm_time - send_time) |
| `p_op` | операция (`"show"`) |
| `chat_id` | **ID чата** |
| `call_id` | **ID звонка** (conversationId) |
| `show_source` | источник показа |

## Что важно

1. **`dtime`** — задержка доставки push-уведомления. Сервер знает, насколько быстро доставляются уведомления.

2. **`fcmdtime`** — задержка FCM. Сервер знает задержку FCM.

3. **`call_id`** — ID звонка передаётся в аналитику.

4. **`chat_id`** — ID чата передаётся в аналитику.

## Сводка

`PUSH.InboundCall {trid, eKey, suid, ttime, dtime, fcmdtime, p_op="show", chat_id, call_id, show_source}`.
