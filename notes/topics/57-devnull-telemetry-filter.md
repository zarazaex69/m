---
tags: [telemetry, server-control, pms, analytics]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/om5.java
  - work/jadx_base/sources/defpackage/mm5.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[03-pms-server-flags]]"
  - "[[31-onelog-event-categories]]"
  - "[[56-perf-events-apptracer-upload]]"
---

# devnull — server-controlled blacklist событий аналитики

`PmsKey.devnull` — JSON-конфиг, default `{}`. Тип: `DevNullServerConfig(events=...)`.

Пакет `one.me.statistics.devnull.DevNull` — отдельный модуль статистики. По имени и структуре — **server-controlled список событий аналитики, которые нужно игнорировать** (не отправлять на сервер).

Это инвертированный механизм: вместо включения событий сервер задаёт список для отключения. Позволяет серверу снизить нагрузку на аналитический pipeline, временно отключить конкретные события или скрыть их от аналитики.

Аналогично `PerfEventsServerConfig` (см. [[56-perf-events-apptracer-upload]]) — оба конфига управляют тем, что именно собирается.

## dps

`PmsKey.dps` — присутствует в списке PmsKey, явного usage в jadx не найдено. Статус: needs-verification.

## net-stat-config

`PmsKey.f200netstatconfig` — JSON-конфиг сетевой статистики. `NetStatConfig` — отдельная модель. Управляет, какие сетевые метрики собираются (latency, packet loss, bandwidth).

## opcode-stat-config

`qp6.opcodeStatConfig` — long. Управляет, для каких WS-опкодов собирается статистика (latency, error rate). Сервер может включить детальную статистику для конкретных опкодов.

## Сводка

`devnull` — server-pushed blacklist событий аналитики. `net-stat-config` — конфиг сетевой статистики. `opcode-stat-config` — конфиг статистики WS-опкодов. Все три позволяют серверу точно управлять тем, что клиент собирает и отправляет.
