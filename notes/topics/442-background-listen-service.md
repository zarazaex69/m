---
tags: [background-wake, foreground-service, boot-receiver, surveillance, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/background/wake/BackgroundListenService.java
  - work/jadx_base/sources/one/me/background/wake/BackgroundWakeBootReceiver.java
  - work/jadx_base/sources/one/me/background/wake/BackgroundCheckReceiver.java
  - work/apktool_base/res/values/strings.xml
related:
  - "[[08-background-wake]]"
  - "[[372-notification-service-boot-receiver]]"
---

# BackgroundListenService — фоновая работа MAX

`BackgroundListenService` — Foreground Service для фоновой работы MAX.

## Уведомление

- Заголовок: **"MAX работает в фоне"**
- Подзаголовок: **"Вы будете получать уведомления"**
- Notification ID: **9001**

## Логика

1. `onCreate()` → `startForeground(9001, notification)`
2. Логирует: `"system_curtain_shown"` (BACKGROUND_MODE)
3. `onDestroy()` → `stopForeground(1)`, логирует `"system_curtain_hidden"`
4. `onTimeout()` → `stopForeground(1)` (Android 14+ timeout)

## BackgroundWakeBootReceiver

`BroadcastReceiver` — перезапускает фоновую работу при обновлении приложения.

## BackgroundCheckReceiver

`BroadcastReceiver` — проверяет состояние фоновой работы.

## Что важно

1. **Notification ID 9001** — фиксированный ID уведомления.

2. **`system_curtain_shown`/`system_curtain_hidden`** — события логируются в аналитику.

3. **`BackgroundWakeBootReceiver`** — перезапускает при обновлении приложения.

4. **`onTimeout`** — Android 14+ ограничивает время foreground service.

## Сводка

`BackgroundListenService`: `startForeground(9001)`, "MAX работает в фоне". `BackgroundWakeBootReceiver`: перезапуск при обновлении. Логирует `system_curtain_shown/hidden`.
