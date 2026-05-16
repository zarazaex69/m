---
tags: [calls, analytics, api, external, collector]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/analytics/internal/api/CallExternalAnalyticsApiRequest.java
  - work/jadx_base/sources/ru/ok/android/externcalls/analytics/internal/api/CallAnalyticsApiRequest.java
related:
  - "[[147-call-native-analytics-request]]"
  - "[[96-vchat-api-methods]]"
---

# CallExternalAnalyticsApiRequest — внешняя аналитика звонков

`CallExternalAnalyticsApiRequest` — HTTP запрос для внешней аналитики звонков (в отличие от нативной из [[147-call-native-analytics-request]]).

## Параметры

| Параметр | Что |
|---|---|
| `apiMethod` | метод API |
| `application` | приложение-источник |
| `collector` | **коллектор аналитики** |
| `platform` | платформа |
| `items` | данные |

## CallAnalyticsApiRequest базовые ключи

| Ключ | Что |
|---|---|
| `KEY_APPLICATION = "application"` | приложение |
| `KEY_COLLECTOR = "collector"` | коллектор |
| `KEY_ITEMS = "items"` | данные |

## Что важно

1. **`collector`** — коллектор аналитики. Это означает, что аналитика звонков может отправляться в разные коллекторы (разные системы сбора данных).

2. **`application`** — приложение-источник. Позволяет серверу различать, из какого приложения пришла аналитика.

3. Два типа аналитических запросов: `CallNativeAnalyticsApiRequest` (с `sdkVersion`, `appVersion`) и `CallExternalAnalyticsApiRequest` (с `application`, `collector`). Это означает, что аналитика звонков отправляется в две разные системы.

## Сводка

`CallExternalAnalyticsApiRequest` — внешняя аналитика с `application`, `collector`, `platform`. Два независимых канала аналитики звонков: нативный (vchat.clientStats) и внешний.
