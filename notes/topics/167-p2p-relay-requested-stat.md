---
tags: [calls, p2p-relay, stats, telemetry, network]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/stat/p2prelay/P2PRelayRequestedStat.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/stat/p2prelay/P2PRelayRequestReason.java
related:
  - "[[125-p2p-relay-webtransport]]"
  - "[[163-conversation-stats]]"
---

# P2PRelayRequestedStat — статистика запроса P2P relay

`P2PRelayRequestedStat.onP2PRelayRequested(reason)` — событие `client_requested_p2p_relay`.

## P2PRelayRequestReason

| Поле | Что |
|---|---|
| `trigger` | триггер запроса relay |
| `threshold` | порог (long) |
| `violationsCount` | количество нарушений |

## Что важно

1. **`trigger`** — причина, по которой клиент запросил P2P relay. Это может быть `bad_network`, `high_rtt`, и т.п.

2. **`threshold`** и **`violationsCount`** — пороговые значения и количество нарушений. Сервер знает, насколько плохой была сеть перед переключением на relay.

3. Это событие отправляется, когда клиент **сам** запрашивает переключение на relay (в отличие от `p2p_forbidden` из [[139-api-protocol-constants]], когда сервер запрещает P2P).

## Сводка

`P2PRelayRequestedStat` — событие `client_requested_p2p_relay` с `trigger`/`threshold`/`violationsCount`. Сервер знает, когда и почему клиент переключился на relay.
