---
tags: [calls-api, api-methods, vchat, batch, external-ids, codec, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/api/OkApiServiceInternal.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/api/request/StartConversation.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/api/request/GetSettings.java
related:
  - "[[384-externcalls-sdk-config]]"
  - "[[101-calls-sdk-managers]]"
---

# OkApiServiceInternal — API-методы звонков

`OkApiServiceInternal` — внутренний API-клиент для звонков. Все методы через `rxApiClient`.

## API-методы

| Метод | Endpoint | Что |
|---|---|---|
| `startConversation(servers, cid, createLink, opponent, ...)` | — | начать звонок |
| `joinToConversation(cid, peerId, params)` | — | войти в звонок |
| `joinConversationByLink(joinLink, anonToken, peerId, params)` | — | войти по ссылке |
| `hangupConversation(cId, reason, anonToken)` | — | завершить звонок |
| `getConversationParams(anonToken, isFastRetry, cid)` | — | получить параметры звонка |
| `createJoinLink(cid)` | — | создать ссылку для входа |
| `removeJoinLink(conversationId)` | — | удалить ссылку |
| `getSettings(keys)` | `settings.get` | получить настройки |
| `requestUploadUrl(cid, type, anonToken)` | — | запросить URL для загрузки |
| `sendSupportedCodecsStatistics(codecList)` | — | отправить список поддерживаемых кодеков |
| `getExternalIdsByOkIds(candidates)` | `batch.execute/vchat.getExternalIdsByOkIds` | маппинг OK ID → external ID |
| `getOkIdsByExternalIds(candidates)` | `batch.execute/vchat.getOkIdsByExternalIds` | маппинг external ID → OK ID |
| `getOkIdByExternalId(participantExternalId)` | — | один external ID → OK ID |

## Batch-запросы

- `BATCH_PREFIX = "batch.execute/"`
- `MAX_EXTERNAL_IDS_PER_REQUEST = 200`
- `MAX_OK_IDS_PER_REQUEST = 100`

## Что важно

1. **`sendSupportedCodecsStatistics`** — клиент отправляет список поддерживаемых кодеков на сервер. Сервер знает возможности устройства.

2. **`getExternalIdsByOkIds`/`getOkIdsByExternalIds`** — двусторонний маппинг ID. Сервер может деанонимизировать участников.

3. **`requestUploadUrl`** — сервер выдаёт URL для загрузки медиа из звонка.

4. **`hangupConversation(reason)`** — причина завершения звонка отправляется на сервер.

## Сводка

`OkApiServiceInternal`: 13 методов. Ключевые: `sendSupportedCodecsStatistics`, `getExternalIdsByOkIds`/`getOkIdsByExternalIds` (деанонимизация), `requestUploadUrl`, `hangupConversation(reason)`.
