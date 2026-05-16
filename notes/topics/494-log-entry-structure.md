---
tags: [log-entry, telemetry, surveillance, event-structure]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/mph.java
  - work/jadx_base/sources/defpackage/qk9.java
related:
  - "[[471-log-controller]]"
  - "[[493-log-external-api]]"
---

# mph + qk9 — Структура лог-события

## mph — StatEntity

```
StatEntity(id=long, timestamp=long, data=qk9)
```

## qk9 — LogEntry

```
LogEntry(
  type=String,      // тип события (DEV/CALL/BACKGROUND/...)
  event=String,     // имя события
  userId=long,      // ID пользователя
  sessionId=long,   // ID сессии
  params=Map,       // параметры
  time=long         // timestamp
)
```

## Что важно

1. **`userId`** — каждое событие содержит ID пользователя.

2. **`sessionId`** — каждое событие содержит ID сессии.

3. **`time`** — точное время события.

4. **`type`/`event`** — тип и имя события.

## Сводка

`qk9(type, event, userId, sessionId, params, time)`. Каждое событие содержит userId + sessionId + timestamp.
