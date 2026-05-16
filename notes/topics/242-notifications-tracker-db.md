---
tags: [database, notifications, tracking, fcm, drop-reason]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/crc.java
  - work/jadx_base/sources/defpackage/sk.java
related:
  - "[[218-fcm-stat-db-schemas]]"
  - "[[216-local-db-tables]]"
---

# notifications_tracker_messages DB схема

`notifications_tracker_messages` — трекинг уведомлений о сообщениях.

## Поля

| Поле | Что |
|---|---|
| `chat_id` / `message_id` | PK |
| `time` | время |
| `fcm` | **FCM push ID** |
| `drop_reason` | **причина отброса уведомления** |

## Что важно

1. **`drop_reason`** — причина, по которой уведомление было отброшено. Это означает, что система отслеживает, почему уведомления не показываются.

2. **`fcm`** — ID FCM push. Связывает уведомление с конкретным push.

3. `INSERT OR IGNORE` — не перезаписывает существующие записи.

## Сводка

`notifications_tracker_messages`: chat_id/message_id/time/fcm/drop_reason. Трекинг причин отброса уведомлений.
