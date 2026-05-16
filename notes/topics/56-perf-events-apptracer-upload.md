---
tags: [telemetry, apptracer, performance, server-control, pms]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/nei.java
  - work/jadx_base/sources/defpackage/vbi.java
  - work/jadx_base/sources/defpackage/c6.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[17-apptracer-uplink]]"
  - "[[04-telemetry-endpoints]]"
  - "[[03-pms-server-flags]]"
---

# perf-events — server-controlled performance telemetry upload

`perf-events` и `perf-registrar-config` — два PmsKey для управления телеметрией производительности, которая уходит на `sdk-api.apptracer.ru`.

## Механизм

`nei.java:188` — формирует JSON-payload и отправляет POST на:

```
https://sdk-api.apptracer.ru/api/perf/upload?crashToken=<token>
```

Payload структура:
- `samples` — массив объектов с полями `name`, `value`, `unit`, `attributes` (произвольные key-value пары)

Это тот же `sdk-api.apptracer.ru`, что и в [[17-apptracer-uplink]] (endpoints `/api/sample/upload`, `/api/perf/upload`, `/api/crash/trackSession`). Подтверждение: `/api/perf/upload` — реальный endpoint, используемый в production.

## PmsKey

| Ключ | # | Что |
|---|---|---|
| `perf-events` | (в qp6) | JSON-конфиг событий производительности для сбора; default `{}` |
| `perf-registrar-config` | (в qp6) | конфиг регистратора перформанс-метрик |

`qp6.perfEventsConfig` → `PerfEventsServerConfig(events=...)` — список событий, которые нужно собирать. Сервер задаёт, какие именно метрики производительности собирать у клиента.

## performance.class

`vbi.java:88` — `osi.e("perf_class", strName)` — записывает класс производительности устройства. `c6.java:192` — `"performance.class = " + accountInitializer7` — логирует класс при инициализации аккаунта.

Это классификация устройства по производительности (low/medium/high), которая используется для адаптации качества медиа и UI. Класс производительности отправляется на сервер как часть perf-метрик.

## Что важно

1. **Сервер через `perf-events` задаёт, какие метрики собирать** — это не фиксированный набор, а server-configurable список. Сервер может в любой момент добавить новые метрики для сбора.

2. **`/api/perf/upload` с `crashToken`** — тот же токен, что используется для crash-репортов. Это означает, что perf-метрики привязаны к конкретному устройству/сессии через crashToken.

3. **Ответ сервера** — `PERFORMANCE_METRICS` JSON-объект. Сервер может вернуть что-то в ответ на perf-upload (например, новые конфиги или команды).

## Сводка

`perf-events` — server-configurable список метрик производительности, которые клиент собирает и отправляет на `sdk-api.apptracer.ru/api/perf/upload`. Payload включает `name`, `value`, `unit`, `attributes` для каждой метрики. Сервер контролирует, что именно собирается.
