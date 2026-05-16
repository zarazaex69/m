---
tags: [surveillance, proc-filesystem, cpu, memory, network, battery, processes]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/uoa.java
  - work/jadx_base/sources/defpackage/gy4.java
  - work/jadx_base/sources/defpackage/k6e.java
  - work/jadx_base/sources/defpackage/lel.java
related:
  - "[[316-battery-snapshot]]"
  - "[[317-enriched-battery-snapshot]]"
  - "[[223-device-performance-telemetry]]"
---

# /proc filesystem — чтение системных данных

MAX читает данные из `/proc` filesystem для сбора статистики производительности.

## Читаемые файлы

| Файл | Что |
|---|---|
| `/proc/self/stat` | **CPU статистика процесса** (utime/stime) |
| `/proc/self/statm` | **память процесса** |
| `/proc/<pid>/cmdline` | имя процесса |
| `/proc/self/fd/<fd>` | **символические ссылки файловых дескрипторов** |

## Проверяемые пути (lel.java)

| Путь | Что |
|---|---|
| `/proc/` | proc filesystem |
| `/data/misc/` | системные данные |
| `/data/data/` | данные приложений |
| `/dev/` | устройства |
| `/sys/` | sysfs |

## Что важно

1. **`/proc/self/stat`** — CPU ticks (utime/stime) для BatterySnapshot.

2. **`/proc/self/statm`** — память процесса. Используется в `uoa.java` для мониторинга памяти.

3. **`/proc/self/fd/<fd>`** — чтение символических ссылок файловых дескрипторов. Это может использоваться для определения открытых файлов.

4. **`/data/data/ru.oneme.app`** — путь к данным приложения. Используется для проверки доступа.

## Сводка

MAX читает `/proc/self/stat`(CPU)/`/proc/self/statm`(память)/`/proc/<pid>/cmdline`(имя процесса)/`/proc/self/fd/`(файловые дескрипторы).
