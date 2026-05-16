---
tags: [telemetry, firebase, installations, fid, app-id, sdk-version, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/pw6.java
related:
  - "[[349-firebase-installations-api]]"
  - "[[347-firebase-messaging-service]]"
---

# Firebase Installations — данные запроса

`pw6.java` — Firebase Installations API запросы.

## Запрос регистрации (g)

```json
{
  "fid": "<Firebase Installation ID>",
  "appId": "<app_id>",
  "authVersion": "FIS_v2",
  "sdkVersion": "a:18.0.0"
}
```

## Запрос обновления (h)

```json
{
  "installation": {
    "sdkVersion": "a:18.0.0"
  }
}
```

## Ответ

| Поле | Что |
|---|---|
| `fid` | **Firebase Installation ID** |

## Что важно

1. **`fid`** — Firebase Installation ID. Уникальный идентификатор установки.

2. **`appId`** — ID приложения в Firebase.

3. **`authVersion: FIS_v2`** — версия протокола аутентификации.

4. **`sdkVersion: a:18.0.0`** — версия Firebase Installations SDK.

5. Данные уходят на `firebaseinstallations.googleapis.com` — Google.

## Сводка

Firebase Installations: fid/appId/authVersion(FIS_v2)/sdkVersion(a:18.0.0). Данные уходят в Google.
