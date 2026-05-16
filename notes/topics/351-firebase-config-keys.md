---
tags: [telemetry, firebase, config, api-key, app-id, project-id, surveillance]
status: confirmed
sources:
  - work/apktool_base/res/values/strings.xml
related:
  - "[[350-firebase-installations-data]]"
  - "[[347-firebase-messaging-service]]"
---

# Firebase конфигурация — ключи и идентификаторы

Из `res/values/strings.xml` — публичные Firebase конфигурационные данные.

## Значения

| Ключ | Значение |
|---|---|
| `google_api_key` | `AIzaSyABuDYeeDXIOrKTXLkUj30Ii143ofPe63Q` |
| `google_crash_reporting_api_key` | `AIzaSyABuDYeeDXIOrKTXLkUj30Ii143ofPe63Q` |
| `google_app_id` | `1:659634599081:android:9605285443b661167225b8` |
| `gcm_defaultSenderId` | `659634599081` |
| `google_storage_bucket` | `max-messenger-app.firebasestorage.app` |
| `project_id` | `max-messenger-app` |

## Что важно

1. **`google_api_key`** — публичный API ключ Firebase. Используется в `X-Goog-Api-Key` заголовке (см. [[348-firebase-cct-transport]]).

2. **`google_app_id`** — ID приложения в Firebase: `1:659634599081:android:9605285443b661167225b8`. Это `appId` в Firebase Installations (см. [[350-firebase-installations-data]]).

3. **`gcm_defaultSenderId: 659634599081`** — Sender ID для FCM. Это Google Cloud project number.

4. **`project_id: max-messenger-app`** — Firebase project ID.

5. **`google_storage_bucket: max-messenger-app.firebasestorage.app`** — Firebase Storage bucket.

## Сводка

Firebase config: `google_api_key=AIzaSyABuDYeeDXIOrKTXLkUj30Ii143ofPe63Q`, `google_app_id=1:659634599081:android:...`, `project_id=max-messenger-app`.
