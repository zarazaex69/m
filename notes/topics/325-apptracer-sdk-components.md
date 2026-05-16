---
tags: [telemetry, apptracer, tracer-components, heap-dump, cpu-profiler, disk-usage, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tracer/
related:
  - "[[17-apptracer-uplink]]"
  - "[[323-apptracer-all-endpoints]]"
  - "[[324-disk-usage-worker]]"
---

# Apptracer SDK — полный список компонентов

Все компоненты Apptracer SDK в MAX:

## Компоненты

| Компонент | Что |
|---|---|
| `TracerInitializer` | инициализация |
| `CrashReportInitializer` | **crash-репорты** |
| `HeapDumpInitializer` | **heap dumps** |
| `ShrinkDumpWorker` | сжатие heap dump |
| `PerformanceMetricsInitializer` | **метрики производительности** |
| `DiskUsageWorker` | **использование диска** |
| `DiskUsageInitializer` | инициализация disk usage |
| `SamplingProfilerInitializer` | **CPU sampling profiler** |
| `SampleUploadWorker` | загрузка сэмплов |
| `NativeBridge` / `NativeBridgeInitializer` / `NativeBridgeInstaller` | нативный мост |
| `LoggerInitializer` | логирование |
| `InitializationProvider` | провайдер инициализации |
| `Minidump` | minidump (crash) |

## Что важно

1. **`SamplingProfilerInitializer`** — CPU sampling profiler. Apptracer может профилировать CPU.

2. **`HeapDumpInitializer`** — heap dumps. Apptracer может делать дампы кучи.

3. **`NativeBridge`** — нативный мост. Это JNI-интерфейс к `libtracernative.so`.

4. Все компоненты инициализируются через `TracerInitializer`.

## Сводка

Apptracer SDK: CrashReport + HeapDump + PerformanceMetrics + DiskUsage + SamplingProfiler + NativeBridge + SampleUpload.
