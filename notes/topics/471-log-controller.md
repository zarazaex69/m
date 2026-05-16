---
tags: [log-controller, telemetry, surveillance, event-sender, critical-event]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ok9.java
related:
  - "[[470-km5-telemetry-sender]]"
  - "[[373-onelog-telemetry]]"
  - "[[04-telemetry-endpoints]]"
---

# ok9 — LogController (отправка событий телеметрии)

`ok9` — `LogController` — контроллер отправки событий телеметрии.

## Методы

| Метод | Что |
|---|---|
| `f(type, params)` | **отправить критическое событие** |
| `g(type, event, params, force)` | **сохранить обычное событие** |
| `h(ok9, type, event, params, i)` | **отправить событие** (static) |
| `i(reason, force)` | попытаться отправить логи |

## Структура события

```java
new ap(timestamp, userId, deviceId, type, event, params)
```

| Поле | Что |
|---|---|
| `timestamp` | `System.currentTimeMillis()` |
| `userId` | `yag.s()` — ID пользователя |
| `deviceId` | `ri9.N()` — ID устройства |
| `type` | тип события (DEV/CALL/BACKGROUND/...) |
| `event` | имя события |
| `params` | параметры (Map) |

## Что важно

1. **`userId`/`deviceId`** — каждое событие содержит ID пользователя и устройства.

2. **`timestamp`** — точное время события.

3. **Retry** — при ошибке отправки повторяет до 3 раз. После 3 попыток → `LogController$AnalyticsDebugException`.

4. **`LOG_DISCONNECTION_BLOCKER`** — блокировка отправки при отключении.

## Сводка

`ok9` (LogController): `ap(timestamp, userId, deviceId, type, event, params)`. Retry до 3 раз. Типы: DEV/CALL/BACKGROUND/...
