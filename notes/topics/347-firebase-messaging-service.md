---
tags: [telemetry, firebase, fcm, push, google-message-id, product-id]
status: confirmed
sources:
  - work/jadx_base/sources/com/google/firebase/messaging/FirebaseMessagingService.java
related:
  - "[[19-fcm-push-payload]]"
  - "[[218-fcm-stat-db-schemas]]"
---

# Firebase Messaging Service — FCM интеграция

`FirebaseMessagingService` — обработчик FCM push-уведомлений.

## Обрабатываемые интенты

| Action | Что |
|---|---|
| `com.google.android.c2dm.intent.RECEIVE` | входящее FCM сообщение |
| `com.google.firebase.messaging.RECEIVE_DIRECT_BOOT` | сообщение при прямой загрузке |
| `com.google.firebase.messaging.NEW_TOKEN` | **новый FCM токен** |

## Поля FCM сообщения

| Поле | Что |
|---|---|
| `google.message_id` | **ID сообщения** |
| `google.product_id` | **ID продукта** |
| `error` | ошибка |

## Что важно

1. **`google.message_id`** — уникальный ID каждого FCM сообщения. Используется для дедупликации.

2. **`google.product_id`** — ID продукта. Это может быть ID приложения или сервиса.

3. **`NEW_TOKEN`** — при получении нового FCM токена клиент должен обновить его на сервере.

## Сводка

FCM: `com.google.android.c2dm.intent.RECEIVE` + `NEW_TOKEN`. Поля: `google.message_id`/`google.product_id`/`error`.
