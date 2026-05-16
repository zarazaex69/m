---
tags: [battery, cpu, network-stats, surveillance, telemetry, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/vw0.java
related:
  - "[[427-db-additional-tables]]"
  - "[[408-pmskey-debug-logging-transcription]]"
---

# vw0 — сбор данных батареи и CPU

`vw0` — сборщик данных батареи и CPU. Управляется PmsKey `battery-slice-interval`.

## Собираемые данные

### Батарея

`BatteryManager.getIntProperty(4)` — мгновенный ток (instantAmperage).

### CPU

| Метрика | Что |
|---|---|
| `utime` | user CPU time |
| `stime` | system CPU time |
| `availableProcessors` | количество процессоров |

### Сеть (aw0)

| Метрика | Что |
|---|---|
| `mrx` | mobile bytes received |
| `mtx` | mobile bytes transmitted |
| `midle` | mobile idle |
| `wrx` | wifi bytes received |
| `wtx` | wifi bytes transmitted |
| `widle` | wifi idle |

## Логика

1. Периодически делает срезы (`battery-slice-interval`)
2. Вычисляет дельты между срезами
3. Сохраняет в таблицу `battery`
4. Отправляет на сервер

## Что важно

1. **`mrx`/`mtx`/`wrx`/`wtx`** — трафик по мобильной сети и WiFi. Сервер знает, сколько данных передаёт устройство.

2. **`instantAmperage`** — мгновенный ток батареи. Косвенно показывает нагрузку на устройство.

3. **`availableProcessors`** — количество процессоров устройства.

4. **`battery-slice-interval`** — интервал задаётся сервером через PmsKey.

## Сводка

`vw0`: срезы батареи (utime/stime/batteryCapacity/instantAmperage) + сетевой трафик (mrx/mtx/wrx/wtx). Интервал: `battery-slice-interval` (PmsKey).
