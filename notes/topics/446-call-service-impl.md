---
tags: [calls, connection-service, telecom, foreground-service, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/calls/impl/service/CallServiceImpl.java
related:
  - "[[371-media-projection-service]]"
  - "[[442-background-listen-service]]"
  - "[[101-calls-sdk-managers]]"
---

# CallServiceImpl — сервис звонков (ConnectionService)

`CallServiceImpl extends ConnectionService` — Android Telecom ConnectionService для звонков.

## Ключевые методы

| Метод | Что |
|---|---|
| `onCreateIncomingConnection(...)` | входящий звонок |
| `onCreateOutgoingConnection(...)` | исходящий звонок |
| `onCreateIncomingConnectionFailed(...)` | ошибка входящего |
| `onCreateOutgoingConnectionFailed(...)` | ошибка исходящего |
| `onStartCommand(intent, ...)` | обработка команд |
| `onTaskRemoved(intent)` | приложение удалено из задач |

## Команды (ACTION)

| Команда | Что |
|---|---|
| `t12.a` | start |
| `t12.b` | show hidden incoming notification |
| `t12.c` | restart |
| `t12.d` | restart for screen sharing |

## WakeLock

`"max:calls_prx"` — WakeLock для звонков.

## Уведомления

- `"ru.oneme.app.new.incomingCalls."` — входящий звонок
- `"ru.oneme.app.new.activeCalls"` — активный звонок
- `"action-finished-call"` — завершение звонка

## Что важно

1. **`ConnectionService`** — интеграция с Android Telecom. Звонки отображаются в системном интерфейсе.

2. **`onTaskRemoved`** — при удалении приложения из задач сервис проверяет наличие активного звонка.

3. **`LOCAL_ACCOUNT_ID`** — ID аккаунта передаётся в Intent.

4. **`max:calls_prx`** — WakeLock для предотвращения засыпания устройства во время звонка.

## Сводка

`CallServiceImpl`: ConnectionService + WakeLock("max:calls_prx") + уведомления(incomingCalls/activeCalls). Команды: start/restart/restart_for_screen_sharing.
