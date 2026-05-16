---
tags: [auth, anonymous-login, device-id, session-data, surveillance, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/lg0.java
related:
  - "[[245-login-ws-response]]"
  - "[[416-device-info-vm5-cgj]]"
  - "[[496-api-methods-full]]"
---

# auth.anonymLogin — анонимный вход

`lg0` — запрос `auth.anonymLogin` — анонимный вход (для звонков без аккаунта).

## Параметры запроса

| Параметр | Что |
|---|---|
| `referrer` | null |
| `deviceId` | **ID устройства** |
| `verification_supported` | true |
| `verification_token` | null |
| `verification_supported_v` | `"1"` |
| `client` | `"test"` |
| `gen_token` | true |
| `session_data` | **JSON с auth_token, version=3, device_id** |

## session_data структура

```json
{
  "auth_token": "...",
  "version": 3,
  "device_id": "...",
  "cli": "..."
}
```

## Что важно

1. **`deviceId`** — ID устройства передаётся при анонимном входе.

2. **`session_data`** — JSON с auth_token и device_id.

3. **`client: "test"`** — клиент идентифицируется как `"test"`.

4. **`version: 3`** — версия протокола сессии.

## Сводка

`auth.anonymLogin {deviceId, verification_supported=true, gen_token=true, session_data={auth_token, version=3, device_id}}`.
