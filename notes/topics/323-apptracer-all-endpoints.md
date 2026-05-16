---
tags: [telemetry, apptracer, crash-track, session, device-id, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ktg.java
  - work/jadx_base/sources/ru/ok/tracer/upload/SampleUploadWorker.java
related:
  - "[[17-apptracer-uplink]]"
  - "[[322-apptracer-sample-upload]]"
  - "[[321-apptracer-perf-upload]]"
---

# Apptracer — полный список endpoints

Все endpoints `sdk-api.apptracer.ru`:

## api/crash/trackSession

```
POST https://sdk-api.apptracer.ru/api/crash/trackSession?crashToken=<token>

{
  "device_id": "<device_id>",
  "sessions": [...],
  "drops": [...]
}
```

**`device_id`** — ID устройства передаётся при каждой сессии.

## api/perf/upload

```
POST https://sdk-api.apptracer.ru/api/perf/upload?crashToken=<token>

{
  "samples": [...]
}
```

BatterySnapshot (CPU/battery/network).

## api/sample/initUpload + api/sample/upload

```
POST https://sdk-api.apptracer.ru/api/sample/initUpload?sampleToken=<token>
POST https://sdk-api.apptracer.ru/api/sample/upload
```

Произвольные файлы (heap dumps, CPU profiles).

## Что важно

1. **`device_id`** в `trackSession` — ID устройства передаётся при каждой сессии.

2. **`drops`** — отброшенные события. Сервер знает, какие данные не были доставлены.

3. **4 endpoint** — crash tracking, performance, sample upload (init + upload).

## Сводка

Apptracer endpoints: `api/crash/trackSession`(device_id/sessions/drops) + `api/perf/upload`(samples) + `api/sample/initUpload`+`api/sample/upload`.
