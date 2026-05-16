---
tags: [telemetry, server-control, pms, performance, network]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/vbd.java
  - work/jadx_base/sources/defpackage/etb.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[03-pms-server-flags]]"
  - "[[56-perf-events-apptracer-upload]]"
  - "[[57-devnull-telemetry-filter]]"
---

# perf-registrar-config и net-stat-config — конфиги телеметрии

## perf-registrar-config

`PerfRegistrarServerSettings` (`vbd.java`) — конфиг регистратора производительности:

| Поле | Default | Что |
|---|---|---|
| `a` (maxAttemptsForPersistentMetric) | 25 | максимальное количество попыток для persistent-метрики |
| `b` (rawPersistInterval) | 15 секунд | интервал сохранения raw-метрик |
| `c` | 3 дня | TTL метрик |
| `d`, `e` | — | дополнительные параметры |

Сервер контролирует, как часто и как долго хранятся метрики производительности.

## net-stat-config

`NetStatConfig` (`etb.java`) — конфиг сетевой статистики:

```
NetStatConfig(loggableOpcodes=<set>)
```

Default: `loggableOpcodes = {17, 18}` — логировать опкоды 17 и 18.

Это означает: сервер задаёт, для каких WS-опкодов собирать сетевую статистику (latency, error rate). Default — опкоды 17 и 18 (вероятно, MSG_SEND и MSG_GET или аналогичные).

## Сводка

`perf-registrar-config` — server-pushed конфиг регистратора производительности (интервалы, TTL, попытки). `net-stat-config` — server-pushed список WS-опкодов для сбора сетевой статистики (default: опкоды 17, 18).
