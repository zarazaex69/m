---
tags: [startup-report, telemetry, surveillance, crash, memory, battery, exit-reason]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/kph.java
related:
  - "[[467-telemetry-event-types]]"
  - "[[452-account-initializer]]"
  - "[[466-memory-stat]]"
---

# kph — Startup Report (отчёт о запуске)

`kph` — `startup_report` — отчёт о запуске приложения. Отправляется при каждом запуске.

## Собираемые данные

| Поле | Что |
|---|---|
| `crash` | **причина предыдущего краша** |
| `exit_reason` | **причина предыдущего завершения** (`ApplicationExitInfo.getReason()`) |
| `img_total` | всего изображений в кэше |
| `img_cache` | изображений в кэше |
| `img_err` | ошибок загрузки изображений |
| `bucket` | **`UsageStatsManager.getAppStandbyBucket()`** — bucket активности |
| `memory` | `ActivityManager.getMemoryClass()` — класс памяти |
| `large_memory` | `ActivityManager.getLargeMemoryClass()` — большой класс памяти |
| `class` | **PerfClass** устройства |
| `isIgnoringBatteryOptimizations` | **игнорирует ли оптимизацию батареи** |
| `isBackgroundRestricted` | **ограничен ли фоновый режим** |

## Что важно

1. **`exit_reason`** — `ApplicationExitInfo.getReason()` — причина предыдущего завершения (crash/ANR/kill/etc).

2. **`bucket`** — `UsageStatsManager.getAppStandbyBucket()` — насколько активно используется приложение (ACTIVE/WORKING_SET/FREQUENT/RARE/RESTRICTED).

3. **`isIgnoringBatteryOptimizations`** — приложение в белом списке батареи.

4. **`isBackgroundRestricted`** — фоновый режим ограничен.

5. **`class`** — PerfClass устройства (LOW/MEDIUM/HIGH).

## Сводка

`startup_report`: crash/exit_reason/img_total/img_cache/img_err/bucket/memory/large_memory/class/isIgnoringBatteryOptimizations/isBackgroundRestricted.
