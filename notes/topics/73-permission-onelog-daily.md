---
tags: [permissions, onelog, telemetry, surveillance, daily-analytics]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/messages/analytics/DailyAnalyticsWorker.java
  - work/jadx_base/sources/defpackage/vdd.java
related:
  - "[[31-onelog-event-categories]]"
  - "[[18-manifest-deep-dive]]"
  - "[[04-telemetry-endpoints]]"
---

# PERMISSION OneLog — ежедневный отчёт о разрешениях

`DailyAnalyticsWorker` — WorkManager-воркер, который **ежедневно** отправляет в OneLog статус всех разрешений приложения.

## Операции

| Операция | Что |
|---|---|
| `permission_status` | ежедневный снапшот всех разрешений |
| `permission_changed_state` | изменение статуса разрешения (из `vdd.java`) |

## Разрешения в ежедневном отчёте

| pType | Разрешение | Статусы |
|---|---|---|
| `push` | POST_NOTIFICATIONS (Android 13+) | allowed/denied |
| `contacts` | READ_CONTACTS | allowed/denied |
| `fsi` | FOREGROUND_SERVICE_INFORMATION | allowed/denied |
| `gallery` | READ_MEDIA_IMAGES (Android 13+) / READ_EXTERNAL_STORAGE | allowed/partial/denied |
| `camera` | CAMERA | allowed/denied |
| `microphone` | RECORD_AUDIO | allowed/denied |
| `geo` | ACCESS_FINE_LOCATION | allowed/denied |

## Что важно

1. **Ежедневный отчёт** — `DailyAnalyticsWorker` запускается каждый день и отправляет статус всех 7 разрешений. Сервер знает, какие разрешения у пользователя включены, и как они меняются со временем.

2. **`permission_changed_state`** — дополнительное событие при изменении разрешения. Сервер получает уведомление в реальном времени, когда пользователь включает или отключает разрешение.

3. **Комбинация**: сервер знает, что у пользователя включены камера + микрофон + геолокация + контакты. Это полный профиль разрешений, обновляемый ежедневно.

4. **`gallery=partial`** (Android 14+) — сервер знает, дал ли пользователь частичный доступ к галерее (только выбранные фото) или полный.

## Сводка

`DailyAnalyticsWorker` ежедневно отправляет в OneLog статус 7 разрешений: push, contacts, fsi, gallery, camera, microphone, geo. `permission_changed_state` — real-time уведомление при изменении. Сервер ведёт полный исторический профиль разрешений каждого пользователя.
