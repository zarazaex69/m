---
tags: [calls, api, server-control, stun, turn, webrtc, compact-format]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/api/ConversationParams.java
related:
  - "[[140-conversation-params]]"
  - "[[139-api-protocol-constants]]"
---

# ConversationParams compact format — LZ4+Base64 JSON

`ConversationParams.decode()` — параметры звонка передаются в компактном формате `<size>:<base64(LZ4(JSON))>`.

## Compact JSON ключи

| Ключ | Что |
|---|---|
| `tkn` | токен |
| `srcp` | тип клиента (source platform) |
| `trne` | **TURN endpoint** (список через запятую) |
| `trnu` | **TURN username** |
| `trnp` | **TURN password** |
| `stne` | **STUN endpoint** (список через запятую) |
| `wse` | **WS endpoint** |
| `wsip` | **WS IP-адреса** (JSON array) |
| `wte` | **WebTransport endpoint** |
| `wtip` | **WebTransport IP-адреса** (JSON array) |

## Что важно

1. **TURN credentials** (`trnu`/`trnp`) — сервер передаёт username и password для TURN-сервера. Это временные credentials для конкретного звонка.

2. **Компактный формат** — LZ4+Base64 позволяет передавать параметры звонка через push-уведомление или URL (компактно).

3. **`wsip` и `wtip`** — списки IP-адресов для WS и WebTransport. Сервер возвращает несколько IP для failover.

4. **`wte`** — WebTransport endpoint. Отдельный endpoint для WebTransport соединения.

## Сводка

Compact format: `<size>:<base64(LZ4(JSON))>`. Ключи: `tkn`, `srcp`, `trne`/`trnu`/`trnp` (TURN с credentials), `stne` (STUN), `wse`/`wsip` (WS), `wte`/`wtip` (WebTransport). Сервер передаёт TURN credentials для каждого звонка.
