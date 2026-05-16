---
tags: [calls, server-topology, stats, telemetry]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/stat/topology/ServerTopologyRequestedStat.java
related:
  - "[[163-conversation-stats]]"
  - "[[125-p2p-relay-webtransport]]"
---

# ServerTopologyRequestedStat — статистика запроса серверной топологии

`ServerTopologyRequestedStat.onServerTopologyRequested(event)` — событие `client_requested_server_topology`.

## Данные

- Значение: время (timestamp или duration)
- `string_value`: `event.a.a` — строковое описание события

## Что важно

1. **Серверная топология** — это режим, когда медиа-трафик идёт через серверы VK/OK, а не напрямую P2P. Клиент может запросить переключение на серверную топологию.

2. Сервер знает, когда клиент переключился на серверную топологию и сколько времени это заняло.

3. В сочетании с `P2PRelayRequestedStat` (см. [[167-p2p-relay-requested-stat]]) — два механизма маршрутизации через серверы: P2P relay и серверная топология.

## Сводка

`client_requested_server_topology` — сервер знает, когда клиент переключился на серверную топологию (медиа через серверы VK/OK).
