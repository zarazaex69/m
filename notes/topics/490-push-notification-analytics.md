---
tags: [push-analytics, telemetry, surveillance, notification, push-action, drop, show]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/a6c.java
related:
  - "[[476-push-inbound-call-analytics]]"
  - "[[472-log-controller-event-types]]"
  - "[[19-fcm-push-payload]]"
---

# a6c — Push Notification Analytics (полная аналитика push)

`a6c` — полная аналитика push-уведомлений.

## Базовые поля (для всех событий)

| Поле | Что |
|---|---|
| `trid` | trace ID |
| `eKey` | event key |
| `ttime` | timestamp push |
| `dtime` | задержка доставки |
| `fcmdtime` | задержка FCM |
| `suid` | session user ID |

## События

### PUSH.{pushType} с p_op="show"

```json
{
  "trid", "eKey", "ttime", "dtime", "fcmdtime", "suid",
  "p_op": "show",
  "chat_id": long,
  "show_source": int
}
```

### PUSH.{pushType} с p_op="drop"

```json
{
  "trid", "eKey", "ttime", "dtime", "fcmdtime", "suid",
  "p_op": "drop",
  "p_dr": drop_reason
}
```

### PUSH.Action с p_op="open_chats"

```json
{
  "p_op": "open_chats"
}
```

### PUSH.Action с p_op="open_chat"|"open_url"

```json
{
  "trid": long,
  "eKey": string,
  "p_op": "open_chat" | "open_url"
}
```

## Что важно

1. **`p_op: "drop"`** — каждое отброшенное уведомление логируется с причиной.

2. **`p_op: "open_chat"`/`"open_url"`** — каждое открытие чата/URL из уведомления логируется.

3. **`dtime`/`fcmdtime`** — задержки доставки.

4. **`chat_id`** — ID чата при показе уведомления.

## Сводка

`a6c`: show/drop/open_chats/open_chat/open_url. Все push-уведомления полностью логируются с trid/eKey/ttime/dtime/fcmdtime/suid.
