---
tags: [calls, ice, stats, peer-connection, telemetry]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/stat/icerestart/IceRestartStat.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/stat/connection/PeerConnectionChangedStat.java
related:
  - "[[163-conversation-stats]]"
  - "[[125-p2p-relay-webtransport]]"
---

# IceRestartStat и PeerConnectionChangedStat

## IceRestartStat

`onIceRestart()` — событие `ice_restart`. Простое событие без параметров — сервер знает, когда произошёл ICE restart.

## PeerConnectionChangedStat

`onStateChanged(PeerConnectionState state, boolean isP2PRelayEnabled)` — событие `connection_state_changed`:

| Поле | Что |
|---|---|
| `connection_state` | состояние PeerConnection (new/connecting/connected/disconnected/failed/closed) |
| `p2p_relay` | включён ли P2P relay |

## Что важно

1. **`connection_state`** — сервер знает каждое изменение состояния PeerConnection. Это полная история подключения/отключения WebRTC.

2. **`p2p_relay`** — при каждом изменении состояния сервер знает, используется ли P2P relay.

3. **`ice_restart`** — сервер знает, когда произошёл ICE restart (переподключение при смене сети).

## Сводка

`IceRestartStat` — `ice_restart` (без параметров). `PeerConnectionChangedStat` — `connection_state_changed` с `connection_state`/`p2p_relay`.
