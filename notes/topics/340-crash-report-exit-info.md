---
tags: [telemetry, apptracer, crash-report, application-exit-info, anr, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tracer/crash/report/CrashReportInitializer.java
related:
  - "[[339-minidump-crash-handler]]"
  - "[[223-device-performance-telemetry]]"
---

# CrashReportInitializer — сбор данных о завершении процесса

`CrashReportInitializer` использует `ApplicationExitInfo` (Android 11+) для сбора данных о завершении процесса.

## Данные ApplicationExitInfo

| Метод | Что |
|---|---|
| `getReason()` | **причина завершения** |
| `getProcessName()` | **имя процесса** |
| `getTimestamp()` | **время завершения** |
| `getTraceInputStream()` | **трассировка** (ANR trace) |
| `getDescription()` | описание |

## Что важно

1. **`getReason() == 6`** — ANR (Application Not Responding). Специальная обработка для ANR.

2. **`getTraceInputStream()`** — трассировка ANR. Это полный stack trace всех потоков при ANR.

3. **`getTimestamp()`** — точное время завершения процесса.

4. Данные загружаются на `sdk-api.apptracer.ru` через `my4Var3.b(10, ...)`.

## Сводка

`ApplicationExitInfo`: getReason()/getProcessName()/getTimestamp()/getTraceInputStream(ANR trace)/getDescription(). ANR trace загружается на Apptracer.
