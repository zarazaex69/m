---
tags: [telemetry, mytracker, activity-lifecycle, session-tracking, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/com/my/tracker/core/o/a.java
  - work/jadx_base/sources/com/my/tracker/applifecycle/MyTrackerAppLifecycle.java
related:
  - "[[331-mytracker-user-lifecycle]]"
  - "[[328-mytracker-init]]"
---

# MyTracker Activity Lifecycle — отслеживание сессий

MyTracker регистрирует `ActivityLifecycleCallbacks` для отслеживания сессий.

## Отслеживаемые события

| Событие | Что |
|---|---|
| `onActivityStarted(activity, timePoint)` | **начало активности** |
| `onActivityStopped(activity, timePoint)` | **конец активности** |

## Что важно

1. **`TimePoint.now()`** — точное время каждого события активности.

2. **`handleOnActivityStarted`/`handleOnActivityStopped`** — обработчики в `MyTrackerActivityHandler`. Это основа для подсчёта времени сессии.

3. MyTracker знает точное время каждого запуска и остановки активности.

4. `trackLaunchManually` — ручной трекинг запуска (для случаев, когда автоматический не работает).

## Сводка

MyTracker ActivityLifecycle: onActivityStarted(timePoint)/onActivityStopped(timePoint). Основа для подсчёта времени сессии.
