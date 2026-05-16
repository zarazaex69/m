---
tags: [telemetry, firebase, installations, googleapis, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/pw6.java
related:
  - "[[348-firebase-cct-transport]]"
  - "[[347-firebase-messaging-service]]"
---

# Firebase Installations API — endpoint

`pw6.java` — Firebase Installations API endpoint.

## Endpoint

```
https://firebaseinstallations.googleapis.com/v1/<path>
```

## Что важно

1. **Firebase Installations** — получение Firebase Installation ID (FID). Это уникальный идентификатор установки приложения.

2. **`firebaseinstallations.googleapis.com`** — Google endpoint. Данные уходят в Google.

3. FID используется для идентификации установки при отправке FCM push-уведомлений.

## Сводка

Firebase Installations: `https://firebaseinstallations.googleapis.com/v1/<path>`. FID — уникальный ID установки.
