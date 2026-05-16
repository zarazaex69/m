---
tags: [telemetry, device-info, memory, standby-bucket, exit-reason, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/kph.java
  - work/jadx_base/sources/defpackage/uoa.java
related:
  - "[[17-apptracer-uplink]]"
  - "[[04-telemetry-endpoints]]"
---

# Device performance telemetry — standby bucket, memory, exit reason

`kph.java` — сборщик данных о производительности устройства. Отправляется через Apptracer.

## Собираемые данные

| Поле | Что |
|---|---|
| `bucket` | **App Standby Bucket** (`getAppStandbyBucket()`) |
| `memory` | класс памяти устройства (`getMemoryClass()`) |
| `large_memory` | большой класс памяти |
| `class` | класс производительности устройства |
| `exit_reason` | **причина последнего завершения процесса** (`getHistoricalProcessExitReasons`) |
| `img_cache` | размер кэша изображений |
| `img_err` | ошибки кэша изображений |

## App Standby Bucket

`getAppStandbyBucket()` — Android API 28+. Возвращает «ведро» активности приложения:
- `ACTIVE` (10) — активно используется
- `WORKING_SET` (20) — регулярно используется
- `FREQUENT` (30) — часто используется
- `RARE` (40) — редко используется
- `RESTRICTED` (45) — ограничено системой

## getHistoricalProcessExitReasons

Android API 30+. Причина последнего завершения процесса: ANR, OOM, crash, user kill, и т.д.

## Что важно

1. **`exit_reason`** — сервер знает, почему завершился процесс приложения (ANR, OOM, crash, kill).

2. **`bucket`** — сервер знает, насколько активно пользователь использует приложение.

3. **`memory`/`large_memory`** — класс памяти устройства. Часть device fingerprint.

## Сводка

Device performance telemetry: `bucket`(App Standby Bucket), `memory`/`large_memory`, `exit_reason`(getHistoricalProcessExitReasons), `img_cache`/`img_err`.
