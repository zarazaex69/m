---
tags: [calls, network, topology, bad-connection, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/net/NetworkConnectionManager.java
related:
  - "[[168-server-topology-stat]]"
  - "[[125-p2p-relay-webtransport]]"
  - "[[179-conversation-interface]]"
---

# NetworkConnectionManager — управление сетевым соединением в звонке

`NetworkConnectionManager` — управление сетевым соединением и топологией.

## Методы

| Метод | Что |
|---|---|
| `getTopology()` | **текущая топология** (P2P или серверная) |
| `registerBadConnectionCallback(callback)` | callback при плохом соединении |
| `addNetworkConnectivityListener(listener)` | слушатель изменений сети |
| `plusAssign(listener)` / `minusAssign(listener)` | добавить/удалить слушатель |

## Что важно

1. **`getTopology()`** — возвращает `xri` — текущую топологию звонка. Это P2P или серверная топология (см. [[168-server-topology-stat]]).

2. **`registerBadConnectionCallback`** — callback при плохом соединении. Это триггер для показа UI «плохое соединение» и потенциального переключения на relay.

3. **`addNetworkConnectivityListener`** — слушатель изменений сетевого подключения (WiFi → мобильная сеть, и т.п.).

## Сводка

`NetworkConnectionManager` — `getTopology()` (P2P/server), `registerBadConnectionCallback`, `addNetworkConnectivityListener`.
