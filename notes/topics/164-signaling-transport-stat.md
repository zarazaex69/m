---
tags: [calls, signaling, stats, telemetry, ping]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/stat/signaling/SignalingTransportStat.java
related:
  - "[[163-conversation-stats]]"
  - "[[147-call-native-analytics-request]]"
---

# SignalingTransportStat — статистика WS-сигналинга звонков

`SignalingTransportStat` — компонент статистики WS-сигналинга в звонках.

## Отслеживаемые события

| Метод | Что |
|---|---|
| `onConnect(type)` | попытка подключения |
| `onConnected(type)` | успешное подключение |
| `onRestart(type)` | перезапуск соединения |
| `onMessageReceived(type, name, isPing)` | получено сообщение (ping/pong или команда) |
| `onCommandSent(type)` | отправлена команда |
| `onDisconnectedSuccessfully(type)` | успешное отключение |
| `onCallFinished(type)` | завершение звонка |
| `onFailedByPings(type, isPong)` | ошибка из-за ping timeout |
| `onFailedByException(type, throwable)` | ошибка из-за исключения |
| `onTimeout(type)` | timeout |

## Поля

- `connectedAtLeastOnceInCall` — подключался ли хотя бы раз
- `startConnectTime` — время начала подключения
- `lastMessageReceived` — время последнего сообщения
- `firstFailTime` — время первой ошибки

## Что важно

1. **`onFailedByPings`** — ошибка из-за ping timeout. Сервер знает, когда клиент потерял соединение из-за ping.

2. **`reportCommandSummary` и `reportPingSummary`** — сводная статистика команд и ping-ов. Отправляется через `vchat.clientStats`.

3. **`isSummaryStatsEnabled`** — server-controlled включение сводной статистики (через `calls-sdk-incall-stat` PmsKey).

## Сводка

`SignalingTransportStat` — детальная статистика WS-сигналинга: подключения, команды, ping/pong, ошибки. Отправляется как часть `ConversationStats`.
