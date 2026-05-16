---
tags: [telemetry, mytracker, event-types, session, launch, referrer, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/com/my/tracker/applifecycle/o/d.java
  - work/jadx_base/sources/com/my/tracker/core/EngineCore.java
related:
  - "[[332-mytracker-activity-lifecycle]]"
  - "[[333-mytracker-install-referrer]]"
---

# MyTracker Event Types — типы событий

`insertEventSync(eventType, subType, ...)` — вставка событий в MyTracker.

## Типы событий

| eventType | subType | Что |
|---|---|---|
| `1L` | `12` | **referrer/deeplink** |
| `2L` | `13` | **launch event** |
| `3L` | `11` | **session** (с lastStopTimestamp) |
| `5L` | `17` | **install referrer** (str/str2/str3/str4/str5) |
| `33L` | `41` | **custom event** |

## Что важно

1. **`3L/11` — session** — при каждом запуске создаётся сессия с `lastStopTimestamp`. Сервер знает время между сессиями.

2. **`1L/12` — referrer** — deeplink/referrer данные.

3. **`5L/17` — install referrer** — данные об источнике установки (5 строковых полей).

4. **`33L/41` — custom event** — кастомные события.

## Сводка

MyTracker event types: session(3L/11)/launch(2L/13)/referrer(1L/12)/install_referrer(5L/17)/custom(33L/41).
