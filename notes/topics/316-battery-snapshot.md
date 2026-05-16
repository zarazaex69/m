---
tags: [database, battery, cpu, network-stats, processes, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ww0.java
  - work/jadx_base/sources/defpackage/vw0.java
related:
  - "[[219-battery-organizations-db]]"
  - "[[223-device-performance-telemetry]]"
---

# BatterySnapshot — детальный срез состояния устройства

`BatterySnapshot` (`ww0`) — детальный снимок состояния устройства, сохраняемый в таблицу `battery`.

## Поля снимка

| Поле | Что |
|---|---|
| `slice` | **номер среза** |
| `cpuTicks.u` | **CPU user ticks** |
| `cpuTicks.s` | **CPU system ticks** |
| `cpuTicks.cu` | **CPU child user ticks** |
| `cpuTicks.cs` | **CPU child system ticks** |
| `batteryPercent` | **заряд батареи (%)** |
| `mobileNet.rx` / `mobileNet.tx` | **мобильный трафик** (rx/tx) |
| `mobileNet.idle` | простой мобильной сети |
| `wifiNet.rx` / `wifiNet.tx` | **WiFi трафик** (rx/tx) |
| `wifiNet.idle` | простой WiFi |
| `processes` | **список процессов** |

## Что важно

1. **CPU ticks** — детальная статистика CPU (user/system/child). Это профилирование производительности.

2. **`mobileNet.rx`/`tx`** и **`wifiNet.rx`/`tx`** — трафик по сети. Сервер знает, сколько трафика потребляет приложение.

3. **`processes`** — список процессов. Это может включать другие запущенные приложения.

4. Снимки делаются с интервалом `batterySliceIntervalMs` (PmsKey).

## Сводка

`BatterySnapshot`: slice/cpuTicks(u/s/cu/cs)/batteryPercent/mobileNet(rx/tx/idle)/wifiNet(rx/tx/idle)/processes.
