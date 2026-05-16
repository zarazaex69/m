---
tags: [memory-stat, heap, surveillance, telemetry, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/woa.java
related:
  - "[[465-devnull-server-config]]"
  - "[[428-battery-cpu-collector]]"
---

# woa — Memory Statistics (статистика памяти)

`woa` — сбор статистики памяти через `Debug.MemoryInfo`.

## Собираемые данные

| Метрика | Что |
|---|---|
| `summary.java-heap` | **Java heap** |
| `summary.native-heap` | **Native heap** |
| `summary.code` | код |
| `summary.stack` | стек |
| `summary.graphics` | графика |
| `summary.private-other` | прочее private |
| `summary.system` | система |
| `summary.total-swap` | **swap** |
| `summary.total-pss` | **total PSS** |

## Что важно

1. **`summary.java-heap`/`summary.native-heap`** — размеры Java и Native heap.

2. **`summary.total-pss`** — Proportional Set Size — реальное потребление памяти.

3. **`summary.total-swap`** — использование swap.

4. Включается через `DevNullServerConfig.isMemoryStatEnabled`.

## Сводка

`woa`: `Debug.MemoryInfo.getMemoryStat()` → java-heap/native-heap/code/stack/graphics/private-other/system/total-swap/total-pss.
