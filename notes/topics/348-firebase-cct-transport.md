---
tags: [telemetry, firebase, cct-transport, datatransport, google-api-key]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ok.java
  - work/jadx_base/sources/defpackage/ddb.java
related:
  - "[[347-firebase-messaging-service]]"
  - "[[04-telemetry-endpoints]]"
---

# Firebase CctTransportBackend — транспорт телеметрии

`CctTransportBackend` — Firebase Data Transport для отправки телеметрии.

## HTTP запрос

```
POST <url>
User-Agent: datatransport/3.1.9 android/
Content-Encoding: gzip
Content-Type: application/json
Accept-Encoding: gzip
X-Goog-Api-Key: <api_key>  (если задан)
```

## Параметры

| Параметр | Что |
|---|---|
| `url` | **URL endpoint** (из конфигурации) |
| `X-Goog-Api-Key` | **Google API ключ** (опционально) |
| `connectTimeout` | 30000 мс |
| `readTimeout` | из конфигурации |

## Что важно

1. **`X-Goog-Api-Key`** — Google API ключ передаётся в заголовке. Это аутентификация для Firebase endpoints.

2. **`datatransport/3.1.9`** — версия Firebase Data Transport SDK.

3. Данные сжимаются gzip перед отправкой.

4. URL задаётся динамически через конфигурацию (не hardcoded).

## Сводка

Firebase CctTransportBackend: POST + gzip + X-Goog-Api-Key + datatransport/3.1.9. URL задаётся динамически.
