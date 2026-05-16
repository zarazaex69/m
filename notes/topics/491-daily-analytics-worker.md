---
tags: [permission-status, daily-analytics, telemetry, surveillance, worker]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/messages/analytics/DailyAnalyticsWorker.java
related:
  - "[[474-permission-tracking]]"
  - "[[472-log-controller-event-types]]"
---

# DailyAnalyticsWorker — ежедневная аналитика разрешений

`DailyAnalyticsWorker` — Worker, отправляющий ежедневный отчёт о статусе разрешений.

## Событие PERMISSION.permission_status

```json
{
  "permissions": [
    {"pType": "push",       "pStatus": "allowed"|"denied"},
    {"pType": "contacts",   "pStatus": "allowed"|"denied"},
    {"pType": "fsi",        "pStatus": "allowed"|"denied"},
    {"pType": "gallery",    "pStatus": "allowed"|"denied"|"partial"},
    {"pType": "camera",     "pStatus": "allowed"|"denied"},
    {"pType": "microphone", "pStatus": "allowed"|"denied"},
    {"pType": "geo",        "pStatus": "allowed"|"denied"}
  ]
}
```

## Что важно

1. **Ежедневный отчёт** — каждый день сервер получает статус всех разрешений.

2. **7 типов разрешений**: push/contacts/fsi/gallery/camera/microphone/geo.

3. **`gallery: "partial"`** — Android 14+ поддерживает частичный доступ к галерее.

4. **`fsi`** — Full Screen Intent разрешение.

## Сводка

`DailyAnalyticsWorker`: `PERMISSION.permission_status {permissions: [{pType, pStatus}]}`. 7 разрешений: push/contacts/fsi/gallery/camera/microphone/geo. Ежедневно.
