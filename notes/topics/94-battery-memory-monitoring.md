---
tags: [battery, memory, telemetry, server-control, pms, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/vw0.java
  - work/jadx_base/sources/defpackage/ra1.java
  - work/jadx_base/sources/defpackage/sk.java
  - work/jadx_base/sources/defpackage/w32.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[03-pms-server-flags]]"
  - "[[35-mytracker-antifraud-sensors]]"
  - "[[04-telemetry-endpoints]]"
---

# Battery и Memory мониторинг — серверный контроль интервалов

## battery-slice-interval (#15)

`PmsKey.f15batterysliceinterval` — long (ms), default `1 минута`. Интервал сбора «срезов» состояния батареи.

Клиент периодически записывает в Room-таблицу `battery`:
- `sliceTime` — timestamp среза
- `payload` — данные батареи (BLOB)

Из `ra1.java:1603` — при отправке аналитики включает:
- `battery_level_change` — изменение уровня заряда
- `stat_time_delta` — временной интервал
- `timestamp` — timestamp

## memory-slice-interval (#181)

`PmsKey.f181memorysliceinterval` — long (ms). Интервал сбора срезов памяти. `rtd.memorySliceIntervalMs` — accessor.

## Calls stats — BatteryLevelChange

В `w32.java` — список метрик звонков, включая `BatteryLevelChange` (9, `battery_level_change`). Это означает, что **изменение уровня заряда батареи во время звонка** отправляется как часть статистики звонка.

## Что важно

1. **Сервер контролирует интервал мониторинга батареи** через `battery-slice-interval`. При `1 минута` — клиент собирает данные о батарее каждую минуту.

2. **`battery_level_change` в статистике звонков** — сервер знает, насколько разрядилась батарея во время каждого звонка. Это может использоваться для оптимизации, но также означает, что сервер имеет детальный профиль энергопотребления устройства.

3. **Room-таблица `battery`** — данные батареи хранятся локально и периодически отправляются на сервер (вероятно через OneLog или apptracer).

4. **`memory-slice-interval`** — аналогичный мониторинг памяти. Сервер знает, сколько памяти использует MAX в разные моменты времени.

## Сводка

`battery-slice-interval` (default 1 мин) и `memory-slice-interval` — server-controlled интервалы мониторинга батареи и памяти. Данные батареи включают `battery_level_change` и отправляются как часть статистики звонков. Сервер имеет детальный профиль энергопотребления устройства.
