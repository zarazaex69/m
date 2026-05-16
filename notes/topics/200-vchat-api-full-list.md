---
tags: [calls, api, vchat, endpoints, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/api/request/
related:
  - "[[143-start-conversation-api]]"
  - "[[144-hangup-join-api]]"
  - "[[145-ok-external-ids-mapping]]"
  - "[[146-log-upload-device-fingerprint]]"
---

# vchat API — полный список методов calls SDK

Все `vchat.*` методы, используемые calls SDK:

| Метод | Что |
|---|---|
| `vchat.startConversation` | начать звонок |
| `vchat.joinConversation` | присоединиться по ID |
| `vchat.joinConversationByLink` | присоединиться по ссылке (анонимно) |
| `vchat.hangupConversation` | завершить звонок |
| `vchat.getConversationParams` | получить параметры звонка |
| `vchat.createJoinLink` | создать ссылку для входа |
| `vchat.removeJoinLink` | удалить ссылку для входа |
| `vchat.clientSupportedCodecs` | **сообщить серверу поддерживаемые кодеки** |
| `vchat.getExternalIdsByOkIds` | получить внешние ID по OK ID |
| `vchat.getOkIdByExternalId` | получить OK ID по внешнему ID |
| `vchat.getOkIdsByExternalIds` | получить OK ID по внешним ID (batch) |

## Что важно

1. **`vchat.clientSupportedCodecs`** — клиент явно сообщает серверу список поддерживаемых кодеков. Сервер использует это для выбора кодека.

2. **`vchat.getConversationParams`** — получить параметры звонка (STUN/TURN/endpoint). Параметры: `anonymToken` + `conversationId`.

3. **`vchat.createJoinLink` / `vchat.removeJoinLink`** — управление ссылками для входа. Сервер хранит все ссылки.

4. **ID mapping методы** (3 штуки) — двусторонний маппинг OK ID ↔ внешние ID. Уже покрыто в [[145-ok-external-ids-mapping]].

## Сводка

11 `vchat.*` методов. Ключевые: `vchat.clientSupportedCodecs` (кодеки), `vchat.getConversationParams` (STUN/TURN), `vchat.createJoinLink`/`removeJoinLink`.
