---
tags: [calls-config, remote-settings, api-protocol, conversation-params, turn-stun, surveillance, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/api/RemoteSettings.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/api/request/GetSettings.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/api/ApiProtocol.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/api/ConversationParams.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/config/BaseConfigProvider.java
related:
  - "[[03-pms-server-flags]]"
  - "[[380-ml-features-manager]]"
  - "[[378-p2prelay-sessionroom]]"
---

# externcalls.sdk.config — конфигурация SDK

## RemoteSettings — серверные ключи конфигурации

| Ключ | Что |
|---|---|
| `android.dump.bitrate` | **дамп битрейта** |
| `android.mlfeatures.%s` | конфиг ML-модели (KWS/NS) |
| `android.rating.limits` | лимиты рейтинга |
| `android.wordspotter.config` | **конфиг KWS (word spotter)** |
| `android.p2prelay.config` | конфиг P2P relay switch |

Запрос: `settings.get?keys=android.dump.bitrate,android.rating.limits,...`

## ApiProtocol — ключи протокола

| Ключ | Что |
|---|---|
| `token` | токен сессии |
| `endpoint` | WS endpoint |
| `wt_endpoint` | WebTransport endpoint |
| `turn_server` | TURN сервер |
| `stun_server` | STUN сервер |
| `p2p_forbidden` | **P2P запрещён** |
| `join_link` | ссылка для входа |
| `upload_url` | URL для загрузки |
| `is_concurrent` | параллельный звонок |
| `device_idx` | индекс устройства |
| `client_type` | тип клиента |

Параметры запросов: `capabilities`, `chatId`, `conversationId`, `deviceId`, `domainId`, `isVideo`, `onlyAdminCanRecord`, `onlyAdminCanShareMovie`, `platform`, `protocolVersion`, `sdkVersion`, `turnServers`, `waitForAdmin`, `webrtcPlatform`.

## ConversationParams — параметры звонка с сервера

| Поле | Что |
|---|---|
| `token` | токен сессии |
| `endpoint` | WS endpoint |
| `wtEndpoint` | WebTransport endpoint |
| `stunTurnServers` | список STUN/TURN серверов |
| `clientType` | тип клиента |
| `deviceIndex` | индекс устройства |
| `isP2PForbidden` | **P2P запрещён сервером** |
| `ispAsNo` | **ASN провайдера** |
| `ispAsOrg` | **организация провайдера** |
| `locCc` | **код страны** |
| `locReg` | **регион** |
| `wsIps` | IP-адреса WS |
| `wtIps` | IP-адреса WebTransport |
| `rateCallData` | данные для оценки звонка |

### Compact формат

`tkn` (token), `srcp` (clientType), `trne`/`trnu`/`trnp` (TURN endpoint/username/password), `stne` (STUN endpoint), `wse` (WS endpoint), `wsip` (WS IPs), `wte` (WT endpoint), `wtip` (WT IPs).

## Что важно

1. **`android.wordspotter.config`** — конфиг KWS приходит с сервера. Сервер контролирует, какие слова отслеживаются.

2. **`android.dump.bitrate`** — сервер может включить дамп битрейта.

3. **`isP2PForbidden`** — сервер может запретить P2P соединение, принудив весь трафик идти через relay.

4. **`ispAsNo`/`ispAsOrg`/`locCc`/`locReg`** — сервер возвращает геолокацию и провайдера клиента в параметрах звонка.

5. **`onlyAdminCanRecord`/`onlyAdminCanShareMovie`** — серверные флаги ограничения прав.

## Сводка

`RemoteSettings`: 5 ключей (android.dump.bitrate/android.mlfeatures.*/android.rating.limits/android.wordspotter.config/android.p2prelay.config). `ConversationParams`: token/endpoint/TURN-STUN/isP2PForbidden/ispAsNo/ispAsOrg/locCc/locReg.
