---
tags: [calls-api, vchat, start-conversation, join, hangup, codec, capabilities, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/api/request/StartConversation.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/api/request/JoinConversation.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/api/request/ClientSupportedCodecs.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/api/request/HangupConversation.java
related:
  - "[[388-ok-api-service-internal]]"
  - "[[384-externcalls-sdk-config]]"
---

# vchat API — запросы звонков

## vchat.startConversation

Параметры запроса:

| Параметр | Что |
|---|---|
| `isVideo` | видеозвонок |
| `turnServers` | TURN серверы |
| `conversationId` | ID звонка |
| `createJoinLink` | создать ссылку |
| `waitForAdmin` | ждать администратора |
| `capabilities` | **hex-encoded возможности клиента** |
| `protocolVersion` | версия протокола (6) |
| `domainId` | ID домена |
| `payload` | произвольные данные |
| `onlyAdminCanShareMovie` | только admin может показывать видео |

## vchat.joinConversation

Параметры: `conversationId`, `peerId`, `isVideo`, `capabilities`, `chatId`, `protocolVersion`.

Ответ: `endpoint`, `wtEndpoint`, `p2pForbidden`, `deviceIndex`.

## vchat.clientSupportedCodecs

Отправляет JSON с поддерживаемыми кодеками. Ответ: `{success: bool}`.

## vchat.hangupConversation

Параметры: `conversationId`, `reason`, `anonymToken`.

## Что важно

1. **`capabilities` (hex-encoded)** — клиент отправляет hex-encoded битовую маску возможностей при каждом звонке. Сервер знает полные возможности устройства.

2. **`vchat.clientSupportedCodecs`** — список поддерживаемых кодеков отправляется на сервер. Это fingerprinting устройства.

3. **`p2pForbidden`** в ответе `joinConversation` — сервер может запретить P2P для конкретного звонка.

4. **`protocolVersion = 6`** — версия протокола при multi-device.

5. **`hangupConversation(reason)`** — причина завершения звонка логируется на сервере.

## Сводка

`vchat.startConversation`: capabilities(hex)/turnServers/waitForAdmin/onlyAdminCanShareMovie. `vchat.joinConversation` → `{endpoint, wtEndpoint, p2pForbidden}`. `vchat.clientSupportedCodecs` → fingerprinting. `vchat.hangupConversation(reason)`.
