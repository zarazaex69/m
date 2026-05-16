---
tags: [calls, analytics, api, telemetry, sdk-version]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/analytics/internal/api/CallNativeAnalyticsApiRequest.java
  - work/jadx_base/sources/ru/ok/android/externcalls/analytics/internal/upload/UploadHelper.java
related:
  - "[[96-vchat-api-methods]]"
  - "[[95-calls-stats-40-metrics]]"
---

# CallNativeAnalyticsApiRequest — нативная аналитика звонков

`CallNativeAnalyticsApiRequest` — HTTP запрос для отправки аналитики звонков (40 метрик из [[95-calls-stats-40-metrics]]).

## Параметры запроса

| Параметр | Значение | Что |
|---|---|---|
| `apiMethod` | `vchat.clientStats` | метод API |
| `platform` | `ANDROID` | платформа |
| `appVersion` | версия MAX | версия приложения |
| `sdkType` | `ANDROID` | тип SDK |
| `sdkVersion` | `0.1.13` | **версия calls SDK analytics** |
| `version` | 1 | версия формата |
| `items` | 40 метрик | данные |

## Что важно

1. **`sdkVersion = "0.1.13"`** — версия calls SDK analytics. Это идентификатор версии аналитического SDK, который отправляется с каждым запросом.

2. **`appVersion`** — версия MAX отправляется с каждым аналитическим запросом. Сервер знает точную версию клиента.

3. **`platform = "ANDROID"`** — платформа. Это позволяет серверу различать Android/iOS/Web клиентов.

4. Все эти параметры отправляются через `vchat.clientStats` (см. [[96-vchat-api-methods]]).

## Сводка

`CallNativeAnalyticsApiRequest` отправляет: `platform=ANDROID`, `appVersion`, `sdkType=ANDROID`, `sdkVersion=0.1.13`, `version=1`, и 40 метрик звонка.
