---
tags: [telemetry, server-control, pms, analytics]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/om5.java
  - work/jadx_base/sources/defpackage/mm5.java
  - work/jadx_base/sources/defpackage/mel.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[03-pms-server-flags]]"
  - "[[31-onelog-event-categories]]"
  - "[[56-perf-events-apptracer-upload]]"
---

# devnull и dps — server-controlled фильтрация телеметрии

## devnull — server-controlled blacklist событий аналитики

`PmsKey.devnull` — JSON-конфиг, default `{}`. Тип: `DevNullServerConfig(events=...)`.

Пакет `one.me.statistics.devnull.DevNull` и `DevNullStatsDependenciesProvider` — отдельный модуль статистики. По имени и структуре — это **server-controlled список событий аналитики, которые нужно игнорировать** (не отправлять на сервер).

Это инвертированный механизм: вместо того чтобы включать события, сервер задаёт список событий для отключения. Это позволяет серверу:
1. Снизить нагрузку на аналитический pipeline, отключив ненужные события.
2. Временно отключить конкретные события для диагностики.
3. Скрыть определённые события от аналитики (например, если они содержат чувствительные данные).

Аналогично `PerfEventsServerConfig` (см. [[56-perf-events-apptracer-upload]]) — оба конфига управляют тем, что именно собирается и отправляется.

## dps — неизвестный PmsKey

`PmsKey.dps` — присутствует в списке PmsKey, но в jadx-декомпиляции не найдено явного использования. Возможно:
- «Data Processing Service» — серверная обработка данных
- «Dynamic Push Service» — динамические push-уведомления
- Новый ключ, добавленный в 26.15.3 без полной реализации

Статус: `needs-verification`.

## Сводка

`devnull` — server-pushed JSON-список событий аналитики для игнорирования. Это server-controlled фильтр телеметрии: сервер решает, какие события клиент должен «выбросить» вместо отправки. `dps` — неизвестный PmsKey без явного usage в коде.
