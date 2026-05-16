---
tags: [telemetry, apptracer, token, crash-token, surveillance]
status: confirmed
sources:
  - work/apktool_base/res/values/strings.xml
  - work/jadx_base/sources/defpackage/osi.java
related:
  - "[[323-apptracer-all-endpoints]]"
  - "[[341-apptracer-crash-types]]"
---

# Apptracer App Token — ключ аутентификации

`tracer_app_token` — токен аутентификации для Apptracer API.

## Значение

```
t6QnlHov0Gq1UBGYG9GPqZu0EiVMZ922FKvwyAEASa90
```

## Использование

Передаётся как `crashToken` query parameter во всех Apptracer API запросах:

- `https://sdk-api.apptracer.ru/api/crash/trackSession?crashToken=t6QnlHov0Gq1UBGYG9GPqZu0EiVMZ922FKvwyAEASa90`
- `https://sdk-api.apptracer.ru/api/perf/upload?crashToken=t6QnlHov0Gq1UBGYG9GPqZu0EiVMZ922FKvwyAEASa90`
- `https://sdk-api.apptracer.ru/api/sample/initUpload?sampleToken=...`

## Что важно

1. **Публичный токен** — хранится в `res/values/strings.xml`. Это не секрет — он доступен любому, кто декомпилирует APK.

2. Токен используется для идентификации приложения на Apptracer сервере.

3. Получается через `z8f.w(context, "tracer_app_token")` — из ресурсов приложения.

## Сводка

`tracer_app_token=t6QnlHov0Gq1UBGYG9GPqZu0EiVMZ922FKvwyAEASa90` — crashToken для всех Apptracer API запросов.
