---
tags: [calls, api, logs, upload, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/uq7.java
  - work/jadx_base/sources/ru/ok/android/externcalls/analytics/internal/upload/UploadHelper.java
  - work/jadx_base/sources/defpackage/och.java
related:
  - "[[96-vchat-api-methods]]"
  - "[[135-callssdk-opus-dred-fec-flags]]"
  - "[[140-conversation-params]]"
---

# vchat.getLogUploadUrl — загрузка логов звонков + och.java device fingerprint

## vchat.getLogUploadUrl

Параметры: `conversationId`, `webRtcPlatform=ANDROID`, `type` (тип лога), `anonymToken`.

Возвращает URL для загрузки логов звонка. Используется для загрузки `CallsSDK-OpusFileLogs` и других диагностических файлов (см. [[135-callssdk-opus-dred-fec-flags]]).

## och.java — device fingerprint в URL

`och.java:56` — при построении URL для звонкового API добавляются параметры:

| Параметр | Что |
|---|---|
| `version` | версия |
| `capabilities` | hex-capabilities |
| `device` | `Build.MANUFACTURER + "/" + Build.MODEL` — **модель устройства** |
| `platform` | `ANDROID` |
| `clientType` | тип клиента |
| `appVersion` | версия приложения |
| `osVersion` | версия Android |
| `ispAsOrg` | **организация провайдера** |
| `locCc` | **country code** |
| `locReg` | **регион** |

## Что важно

1. **`device = Build.MANUFACTURER + "/" + Build.MODEL`** — в каждом запросе к звонковому API передаётся модель устройства. Сервер знает точную модель телефона каждого участника звонка.

2. **`ispAsOrg`, `locCc`, `locReg`** — провайдер и геолокация в каждом запросе. Подтверждение [[140-conversation-params]].

3. **`vchat.getLogUploadUrl`** — сервер выдаёт URL для загрузки логов. Это означает, что диагностические логи звонков (Opus, DataChannel) могут быть загружены на серверы VK/OK по запросу.

## Сводка

`vchat.getLogUploadUrl` — загрузка диагностических логов звонков. `och.java` — каждый запрос к звонковому API содержит модель устройства, версию Android, провайдера и геолокацию.
