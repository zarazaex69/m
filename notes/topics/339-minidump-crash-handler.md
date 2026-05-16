---
tags: [telemetry, apptracer, minidump, crash, native, libtracernative]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tracer/minidump/Minidump.java
  - work/jadx_base/sources/ru/ok/tracer/crash/report/CrashReportInitializer.java
related:
  - "[[228-libtracernative-exports]]"
  - "[[325-apptracer-sdk-components]]"
  - "[[337-crash-report-device-info]]"
---

# Minidump — нативный crash handler

`Minidump` — JNI-обёртка для нативного minidump writer.

## Методы

| Метод | Что |
|---|---|
| `installMinidumpWriterImpl(path)` | **установить minidump writer** |
| `uninstallMinidumpWriterImpl()` | удалить minidump writer |

## Путь к файлу

```
context.getCacheDir() / "tracer" / "minidump"
```

## Что важно

1. **`installMinidumpWriterImpl(path)`** — устанавливает нативный обработчик краша. При краше создаётся minidump файл по указанному пути.

2. **`System.loadLibrary("tracernative")`** — загружает `libtracernative.so`.

3. Minidump файл затем загружается на `sdk-api.apptracer.ru` через `SampleUploadWorker`.

4. Путь: `getCacheDir()/tracer/minidump` — в кэше приложения.

## Сводка

`Minidump.installMinidumpWriterImpl(path)` — нативный crash handler. Minidump сохраняется в `getCacheDir()/tracer/minidump`.
