---
tags: [mytracker, timespent, activity-tracking, foreground-background, surveillance, telemetry]
status: confirmed
sources:
  - work/jadx_base/sources/com/my/tracker/core/handlers/MyTrackerActivityHandler.java
related:
  - "[[385-mytracker-details]]"
  - "[[500-mytracker-urls]]"
---

# MyTrackerActivityHandler — отслеживание активности приложения

`MyTrackerActivityHandler` — интерфейс для отслеживания активности приложения.

## Методы

| Метод | Что |
|---|---|
| `handleOnActivityStarted(engine, activity, time)` | **Activity запущена** |
| `handleOnActivityStopped(engine, activity, time)` | **Activity остановлена** |
| `manualOnActivityStarted(engine, activity, time)` | ручной запуск |
| `setActionTimespentSwitchToBackground(consumer)` | **переход в фон** |
| `setActionTimespentSwitchToForeground(consumer)` | **переход на передний план** |
| `setActionTrackLaunch(biConsumer)` | **отслеживание запуска** |
| `setActionSensorHandlerStart(runnable)` | **запуск датчиков** |
| `resetFlushTimer()` | сброс таймера отправки |

## Что важно

1. **`setActionTimespentSwitchToBackground/Foreground`** — MyTracker отслеживает переходы foreground/background.

2. **`setActionSensorHandlerStart`** — датчики запускаются при активности.

3. **`TimePoint`** — точное время каждого события.

4. Данные отправляются на `ts.tracker-api.vk-analytics.ru/mobile/v1`.

## Сводка

`MyTrackerActivityHandler`: onActivityStarted/Stopped + timespentSwitchToBackground/Foreground + trackLaunch + sensorHandlerStart.
