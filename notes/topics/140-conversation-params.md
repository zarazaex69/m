---
tags: [calls, api, server-control, isp, geo, p2p]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/api/ConversationParams.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/api/request/GetConversationParams.java
related:
  - "[[96-vchat-api-methods]]"
  - "[[139-api-protocol-constants]]"
  - "[[125-p2p-relay-webtransport]]"
---

# vchat.getConversationParams — параметры звонка с сервера

`vchat.getConversationParams` — API метод, возвращающий `ConversationParams`.

## Поля ConversationParams

| Поле | Что |
|---|---|
| `clientType` | тип клиента |
| `deviceIndex` | индекс устройства |
| `endpoint` | **endpoint звонкового сервера** |
| `id` | ID разговора |
| `isP2PForbidden` | **P2P запрещён** |
| `ispAsNo` | **ASN провайдера** |
| `ispAsOrg` | **организация провайдера** |
| `locCc` | **country code** пользователя |
| `locReg` | **регион** пользователя |
| `rateCallData` | данные для оценки звонка |
| `stunTurnServers` | **список STUN/TURN серверов** |
| `token` | токен |
| `wsIps` | **IP-адреса WS-серверов** |

## Что важно

1. **`ispAsNo` и `ispAsOrg`** — сервер знает ASN и организацию провайдера пользователя. Это означает, что сервер идентифицирует интернет-провайдера каждого участника звонка.

2. **`locCc` и `locReg`** — country code и регион. Сервер знает геолокацию пользователя по IP.

3. **`isP2PForbidden`** — сервер может запретить P2P для конкретного звонка. Подтверждение [[139-api-protocol-constants]].

4. **`stunTurnServers`** — список STUN/TURN серверов, возвращаемый сервером. Сервер контролирует, через какие серверы идёт ICE negotiation.

5. **`wsIps`** — IP-адреса WS-серверов. Сервер возвращает список IP для подключения.

6. **`rateCallData`** — данные для оценки звонка. Сервер передаёт параметры для алгоритма оценки качества.

## Сводка

`vchat.getConversationParams` возвращает: endpoint, STUN/TURN серверы, P2P запрет, ISP/geo данные пользователя (ASN, country, region), WS IP-адреса. Сервер знает провайдера и геолокацию каждого участника звонка.
