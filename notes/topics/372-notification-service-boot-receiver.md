---
tags: [notifications, service, direct-reply, push, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/android/services/NotificationTamService.java
  - work/jadx_base/sources/ru/ok/tamtam/android/services/BootCompletedReceiver.java
related:
  - "[[19-fcm-push-payload]]"
  - "[[08-background-wake]]"
---

# NotificationTamService + BootCompletedReceiver

## NotificationTamService

Service для обработки уведомлений с поддержкой direct reply.

### Методы

| Метод | Что |
|---|---|
| `a(chatServerId, service, text, j2)` | **directReply** — отправка ответа из уведомления |
| `onNotificationQuickReplied` | быстрый ответ из уведомления |
| `onNotificationQuickRepliedWithEmptyText` | пустой ответ |
| `onNotificationCancelledBundledChat` | отмена bundled уведомления |
| `onNotificationCancelled` | отмена уведомления |

### Что важно

1. **`directReply`** — отправка сообщений прямо из уведомления. Сервис знает `chatServerId` и текст ответа.

2. **`ru.ok.tamtam.extra.MARK`** — extra в Intent для маркировки уведомлений.

3. **`pushId`/`eventKey`** — уведомления имеют pushId и eventKey для трекинга.

## BootCompletedReceiver

`android.intent.action.BOOT_COMPLETED` → `BackgroundWake` — запуск фоновой работы при загрузке устройства.

## Сводка

`NotificationTamService`: directReply/onNotificationQuickReplied/onNotificationCancelled. `BootCompletedReceiver`: BOOT_COMPLETED → BackgroundWake.
