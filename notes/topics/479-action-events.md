---
tags: [action-events, telemetry, surveillance, cache, fcm, install-referrer]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/cg9.java
  - work/jadx_base/sources/one/me/sdk/vendor/push/FcmMessagingService.java
  - work/jadx_base/sources/defpackage/d6.java
related:
  - "[[472-log-controller-event-types]]"
  - "[[405-install-referrer]]"
  - "[[19-fcm-push-payload]]"
---

# ACTION events — аналитика действий

## ACTION.ACTION_CACHE_CLEARED

Логируется при очистке кэша. Параметры: пустой map.

## ACTION.FCM_ON_DELETED_MESSAGES

Логируется при получении `onDeletedMessages` от FCM. Параметры: пустой map.

`onDeletedMessages` — FCM уведомляет, что некоторые сообщения были удалены из очереди (не доставлены).

## ACTION.GET_INSTALL_REFERRER

Логируется при получении install referrer. Параметры: `{value: referrer_string}`.

Сохраняет версию `"26.15.3"` в SharedPreferences после получения.

## Что важно

1. **`FCM_ON_DELETED_MESSAGES`** — сервер знает, когда FCM удаляет сообщения из очереди.

2. **`GET_INSTALL_REFERRER`** — install referrer логируется в аналитику.

3. **`ACTION_CACHE_CLEARED`** — очистка кэша логируется.

## Сводка

`ACTION.ACTION_CACHE_CLEARED` / `ACTION.FCM_ON_DELETED_MESSAGES` / `ACTION.GET_INSTALL_REFERRER {value: referrer}`.
