---
tags: [capabilities, calls, fingerprinting, hex, surveillance, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/capabilities/ClientCapabilities.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/conversation/StartCallApiParams.java
related:
  - "[[389-vchat-api-requests]]"
  - "[[384-externcalls-sdk-config]]"
---

# ClientCapabilities — битовая маска возможностей клиента

`ClientCapabilities` — hex-encoded битовая маска, отправляемая при каждом звонке. Сервер знает полные возможности клиента.

## Биты возможностей

| Capability | Бит | Что |
|---|---|---|
| `SCREEN_TRACK_PRODUCER` | 0 | **захват экрана (отправка)** |
| `VIDEO_TRACKS` | 1 | видеодорожки |
| `WAITING_HALL` | 2 | зал ожидания |
| `FILTER_DEFAULTS` | 3 | фильтры по умолчанию |
| `SCREEN_TRACK_CONSUMER` | 4 | **захват экрана (приём)** |
| `ADMIN_MUTE_NOTIFY` | 5 | уведомление о принудительном отключении микрофона |
| `WATCH_MOVIE` | 6 | WatchTogether |
| `SESSION_ROOMS` | 8 | комнаты сессии |
| `VMOJI` | 9 | анимоджи |
| `CALL_TO_CONTACTS` | 10 | звонок контактам |
| `AUDIENCE_MODE` | 11 | режим аудитории |
| `SESSION_STATE_UPDATES` | 14 | обновления состояния сессии |
| `ADD_PARTICIPANT` | 15 | добавление участника |
| `USE_P2P_RELAY` | 16 | использование P2P relay |
| `WAIT_FOR_ADMIN` | 17 | ожидание администратора |

## Default capabilities

`SCREEN_TRACK_PRODUCER`, `VIDEO_TRACKS`, `WAITING_HALL` + другие.

## Формат

`getHexValueString()` → hex-строка битовой маски. Передаётся как `capabilities` параметр в `vchat.startConversation`/`vchat.joinConversation`.

## StartCallApiParams

| Поле | Что |
|---|---|
| `hexCapability` | hex-encoded capabilities |
| `domainId` | ID домена |
| `payload` | произвольные данные |
| `isVideo` | видеозвонок |
| `isMultipleDevicesEnabled` | multi-device |
| `isWaitForAdminEnabled` | ждать admin |
| `isWatchTogetherEnabledForAll` | WatchTogether для всех |
| `chatId` | ID чата |
| `clientType` | тип клиента |

## Что важно

1. **Hex capabilities** — сервер получает точный список возможностей клиента при каждом звонке. Это fingerprinting.

2. **`SCREEN_TRACK_PRODUCER`** — клиент сообщает серверу, что умеет захватывать экран.

3. **`ADMIN_MUTE_NOTIFY`** — клиент поддерживает принудительное отключение микрофона администратором.

4. **`payload`** — произвольные данные в параметрах звонка.

## Сводка

`ClientCapabilities`: 15 битов, hex-encoded. Default: SCREEN_TRACK_PRODUCER/VIDEO_TRACKS/WAITING_HALL. Передаётся при каждом `startConversation`/`joinConversation`.
