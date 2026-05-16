---
tags: [calls, api, server-control, start-call, capabilities]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/api/request/StartConversation.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/api/ApiProtocol.java
related:
  - "[[96-vchat-api-methods]]"
  - "[[139-api-protocol-constants]]"
  - "[[140-conversation-params]]"
---

# vchat.startConversation — параметры запуска звонка

`vchat.startConversation` — API метод для начала звонка. Клиент отправляет:

| Параметр | Что |
|---|---|
| `isVideo` | видеозвонок ли |
| `turnServers` | TURN-серверы (клиент предлагает) |
| `conversationId` | ID разговора |
| `createJoinLink` | создать ссылку для присоединения |
| `waitForAdmin` | ждать администратора |
| `capabilities` | **hex-encoded возможности клиента** |
| `protocolVersion` | версия протокола (6 для VOIP_MULTIPLE_DEVICES) |
| `domainId` | ID домена |
| `payload` | дополнительные данные |
| `onlyAdminCanShareMovie` | только администратор может делиться видео |

## Что важно

1. **`capabilities`** — hex-encoded битовая маска возможностей клиента. Сервер знает, что именно поддерживает клиент (кодеки, фичи, и т.п.).

2. **`protocolVersion=6`** — для `FEATURE_VOIP_MULTIPLE_DEVICES` используется версия протокола 6. Это означает, что звонки на нескольких устройствах одновременно — отдельная фича с отдельной версией протокола.

3. **`onlyAdminCanShareMovie`** — клиент сообщает серверу, разрешён ли шаринг видео для всех. Это `!isWatchTogetherEnabledForAll`.

## Сводка

`vchat.startConversation` отправляет hex-capabilities клиента, protocolVersion, domainId, и флаги прав. Сервер знает полные возможности клиента при каждом звонке.
