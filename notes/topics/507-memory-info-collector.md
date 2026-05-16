---
tags: [memory-info, proc-statm, surveillance, telemetry, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/uoa.java
related:
  - "[[466-memory-stat]]"
  - "[[465-devnull-server-config]]"
  - "[[454-proc-files]]"
---

# uoa — Memory Info Collector (полный сбор памяти)

`uoa` — полный сборщик информации о памяти.

## Собираемые данные

| Источник | Данные |
|---|---|
| `Debug.getMemoryInfo()` | java-heap/native-heap/code/stack/graphics/total-pss |
| `ActivityManager.getMemoryInfo()` | `lowMemory`, `availMem`, `totalMem` |
| `/proc/self/statm` | **RSS** (Resident Set Size), **shared** |
| `ActivityManager.getMyMemoryState()` | `lastTrimLevel` |

## Структура zoa (результат)

| Поле | Что |
|---|---|
| `xoa` | Debug.MemoryInfo данные |
| `lastTrimLevel` | уровень trim памяти |
| `lowMemory` | флаг нехватки памяти |
| `availMem` | доступная память |
| `totalMem` | общая память |

## Что важно

1. **`/proc/self/statm`** — RSS и shared memory из `/proc`.

2. **`lastTrimLevel`** — уровень trim (TRIM_MEMORY_*). Показывает давление памяти.

3. **`lowMemory`** — флаг нехватки памяти.

4. Включается через `DevNullServerConfig.isMemoryStatEnabled`.

## Сводка

`uoa`: Debug.MemoryInfo + ActivityManager.MemoryInfo + /proc/self/statm(RSS/shared) + lastTrimLevel + lowMemory/availMem/totalMem.
