---
tags: [calls, call-info, server-control, stun, turn, p2p]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/api/CallInfo.java
related:
  - "[[140-conversation-params]]"
  - "[[142-conversation-params-compact]]"
  - "[[139-api-protocol-constants]]"
---

# CallInfo — полная информация о звонке от сервера

`CallInfo` — структура, возвращаемая сервером при старте/присоединении к звонку.

## Поля

| Поле | Что |
|---|---|
| `endpoint` | **WS endpoint звонкового сервера** |
| `wsIps` | **IP-адреса WS-серверов** |
| `wtEndpoint` | **WebTransport endpoint** |
| `wtIps` | **IP-адреса WebTransport серверов** |
| `id` | ID звонка |
| `token` | токен |
| `clientType` | тип клиента |
| `joinLink` | ссылка для присоединения |
| `isConcurrent` | параллельный звонок |
| `isP2PForbidden` | **P2P запрещён** |
| `turnServer` | **TURN серверы** (List<IceServer>) |
| `stunServer` | **STUN серверы** (List<IceServer>) |
| `deviceIndex` | индекс устройства |

## Что важно

1. **`isP2PForbidden`** — сервер может запретить P2P. Подтверждение [[139-api-protocol-constants]].

2. **`turnServer` и `stunServer`** — списки ICE серверов. Сервер контролирует, через какие серверы идёт ICE negotiation.

3. **`wsIps` и `wtIps`** — IP-адреса для WS и WebTransport. Несколько IP для failover.

4. **`deviceIndex`** — индекс устройства. Для multi-device (см. [[40-multi-account-server-gated]]).

5. `CallInfo.toParams()` — конвертирует в `ConversationParams` (см. [[140-conversation-params]]).

## Сводка

`CallInfo` — полная информация о звонке: endpoint/wsIps/wtEndpoint/wtIps/STUN/TURN/isP2PForbidden/deviceIndex. Сервер полностью контролирует маршрутизацию медиа-трафика.
