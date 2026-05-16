---
tags: [calls-analytics, telemetry, externcalls, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/analytics/CallAnalyticsSender.java
  - work/jadx_base/sources/ru/ok/android/externcalls/analytics/internal/api/CallAnalyticsApiRequest.java
  - work/jadx_base/sources/ru/ok/android/externcalls/analytics/internal/api/CallNativeAnalyticsApiRequest.java
  - work/jadx_base/sources/ru/ok/android/externcalls/analytics/config/UploadConfig.java
related:
  - "[[04-telemetry-endpoints]]"
  - "[[101-calls-sdk-managers]]"
  - "[[373-onelog-telemetry]]"
---

# CallAnalytics — телеметрия звонков

`CallAnalyticsSender` — отправка аналитики звонков.

## CallAnalyticsSender

| Метод | Что |
|---|---|
| `initialize(config)` | инициализация |
| `send(event)` | отправка события |
| `setIdle(isIdle, isCallActive)` | состояние звонка |
| `forceSendScheduledEvents()` | принудительная отправка |

## Два типа запросов

### CallExternalAnalyticsApiRequest

Поля: `collector`, `application`, `items`.

### CallNativeAnalyticsApiRequest

Поля: `app_version`, `sdk_type`, `sdk_version`, `items`.

## UploadConfig (defaults)

| Параметр | Значение |
|---|---|
| `DEFAULT_COMPRESS_CONTENT` | false |
| `DEFAULT_DISABLE_UPLOAD_IN_CALL` | **true** (не загружает во время звонка) |
| `DEFAULT_LOCAL_FILE_COUNT` | 100 |
| `DEFAULT_MAX_EVENT_COUNT` | 800 |
| `DEFAULT_MAX_FILE_SIZE_KB` | 15 |
| `DEFAULT_USE_DB_CACHE` | false |

## Что важно

1. **`DEFAULT_DISABLE_UPLOAD_IN_CALL = true`** — аналитика не отправляется во время звонка. Это снижает нагрузку на сеть.

2. **`sdk_type`/`sdk_version`** — идентификатор SDK в аналитике.

3. **`forceSendScheduledEvents()`** — принудительная отправка накопленных событий.

4. **`setIdle(isIdle, isCallActive)`** — аналитика знает о состоянии звонка.

## Сводка

`CallAnalyticsSender.send(event)` → `CallExternalAnalyticsApiRequest`/`CallNativeAnalyticsApiRequest`. `DEFAULT_DISABLE_UPLOAD_IN_CALL=true`. Поля: collector/application/sdk_type/sdk_version/items.
