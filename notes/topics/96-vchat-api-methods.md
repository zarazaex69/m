---
tags: [calls, api, telemetry, surveillance, vchat]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/analytics/events/SdkIntervalStatEvent.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/api/request/StartConversation.java
  - work/jadx_base/sources/ru/ok/android/onelog/OneLogApiRequest.java
related:
  - "[[95-calls-stats-40-metrics]]"
  - "[[04-telemetry-endpoints]]"
  - "[[43-libjingle-webrtc-custom-build]]"
---

# vchat.* API — звонковый API VK/OK

Звонковый SDK использует отдельный API с методами `vchat.*`. Это не WS-протокол, а HTTP API.

## Полный список методов

| Метод | Что |
|---|---|
| `vchat.clientStats` | **отправка статистики звонка** (40 метрик из [[95-calls-stats-40-metrics]]) |
| `vchat.clientSupportedCodecs` | **список поддерживаемых кодеков** клиента |
| `vchat.createJoinLink` | создать ссылку для присоединения к звонку |
| `vchat.getAnonymTokenByLink` | получить анонимный токен по ссылке |
| `vchat.getConversationParams` | получить параметры разговора |
| `vchat.getExternalIdsByOkIds` | маппинг OK ID → внешние ID |
| `vchat.getLogUploadUrl` | **получить URL для загрузки логов звонка** |
| `vchat.getOkIdByExternalId` | маппинг внешний ID → OK ID |
| `vchat.getOkIdsByExternalIds` | маппинг внешние ID → OK ID (batch) |
| `vchat.hangupConversation` | завершить звонок |
| `vchat.joinConversation` | присоединиться к звонку |
| `vchat.joinConversationByLink` | присоединиться по ссылке |
| `vchat.removeJoinLink` | удалить ссылку |
| `vchat.startConversation` | начать звонок |

## Что важно

1. **`vchat.clientStats`** — HTTP API для отправки 40 метрик звонка. Это отдельный канал от WS и OneLog.

2. **`vchat.clientSupportedCodecs`** — клиент сообщает серверу, какие кодеки поддерживает. Сервер знает возможности устройства.

3. **`vchat.getLogUploadUrl`** — сервер выдаёт URL для загрузки логов звонка. Это означает, что логи звонков (включая потенциально `CallsSDK-OpusFileLogs` и `Calls-SDK-LogDCTraffic` из [[43-libjingle-webrtc-custom-build]]) могут быть загружены на сервер по запросу.

4. **`vchat.getExternalIdsByOkIds` / `vchat.getOkIdsByExternalIds`** — маппинг между OK ID и внешними ID. Это механизм для связывания аккаунтов MAX с внешними системами.

5. **`OneLogApiRequest`** использует `CallAnalyticsApiRequest.KEY_COLLECTOR/APPLICATION/ITEMS` — статистика звонков отправляется через OneLog-инфраструктуру.

## Сводка

14 `vchat.*` API методов. Ключевые: `vchat.clientStats` (40 метрик), `vchat.getLogUploadUrl` (загрузка логов звонка на сервер), `vchat.clientSupportedCodecs` (возможности устройства). Все через HTTP API, отдельно от WS.
