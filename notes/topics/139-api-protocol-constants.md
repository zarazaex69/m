---
tags: [calls, api, protocol, server-control, stun, turn]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/api/ApiProtocol.java
related:
  - "[[96-vchat-api-methods]]"
  - "[[138-settings-get-api]]"
  - "[[43-libjingle-webrtc-custom-build]]"
---

# ApiProtocol — константы звонкового API

`ApiProtocol` — константы для звонкового API VK/OK. Содержит ключи ответов сервера и параметры запросов.

## Ключи ответов сервера (KEY_*)

| Ключ | Что |
|---|---|
| `client_type` | тип клиента |
| `device_idx` | индекс устройства |
| `endpoint` | **endpoint звонкового сервера** |
| `id` | ID |
| `is_concurrent` | параллельный звонок |
| `join_link` | ссылка для присоединения |
| `p2p_forbidden` | **P2P запрещён** |
| `stun_server` | **STUN-сервер** |
| `token` | токен |
| `turn_server` | **TURN-сервер** |
| `upload_url` | URL для загрузки |
| `wt_endpoint` | **WebTransport endpoint** |

## Параметры запросов (PARAM_*)

| Параметр | Что |
|---|---|
| `onlyAdminCanRecord` | только администратор может записывать |
| `onlyAdminCanShareMovie` | только администратор может делиться видео |
| `waitForAdmin` | ждать администратора |
| `capabilities` | возможности клиента |
| `protocolVersion` | версия протокола |
| `sdkVersion` | версия SDK |
| `webRtcPlatform` | платформа WebRTC |

## Что важно

1. **`p2p_forbidden`** — сервер может запретить P2P для конкретного звонка. При `p2p_forbidden=true` все медиа идут через relay.

2. **`stun_server` и `turn_server`** — сервер возвращает адреса STUN/TURN серверов. Это означает, что сервер контролирует, через какие серверы идёт ICE negotiation.

3. **`wt_endpoint`** — WebTransport endpoint. Сервер возвращает URL для WebTransport соединения.

4. **`onlyAdminCanRecord`** и **`onlyAdminCanShareMovie`** — сервер контролирует права записи и шаринга видео в звонке.

## Сводка

`ApiProtocol` содержит ключи для управления звонками: `p2p_forbidden` (запрет P2P), `stun_server`/`turn_server` (server-controlled ICE), `wt_endpoint` (WebTransport), `onlyAdminCanRecord`/`onlyAdminCanShareMovie` (права в звонке).
