---
tags: [telemetry, apptracer, sample-upload, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tracer/upload/SampleUploadWorker.java
related:
  - "[[17-apptracer-uplink]]"
  - "[[321-apptracer-perf-upload]]"
---

# Apptracer sample/initUpload — загрузка произвольных сэмплов

`SampleUploadWorker` — загрузка произвольных файлов-сэмплов на Apptracer.

## Endpoint

```
POST https://sdk-api.apptracer.ru/api/sample/initUpload?sampleToken=<token>
Content-Type: application/json; charset=utf-8

{
  "feature": "<feature_name>",
  "sampleSize": <size>,
  "sampleFileName": "<name>",
  "attr1": <value>,
  "attr2": <value>,
  "tag": "<tag>",
  "<custom_key>": "<value>"
}
```

## Параметры

| Поле | Что |
|---|---|
| `feature` | **имя фичи** (тип сэмпла) |
| `sampleSize` | размер файла |
| `sampleFileName` | имя файла |
| `attr1` / `attr2` | **атрибуты** |
| `tag` | тег |
| custom properties | **кастомные свойства** |

## Что важно

1. **`api/sample/initUpload`** — инициализация загрузки произвольного файла. Это подтверждение [[17-apptracer-uplink]] — Apptracer может загружать произвольные файлы.

2. **`feature`** — тип сэмпла. Это может быть heap dump, CPU profile, и т.д.

3. **`tracer_feature_uze_gzip=true`** — файл сжимается gzip перед загрузкой.

## Сводка

`https://sdk-api.apptracer.ru/api/sample/initUpload?sampleToken=<token>` — загрузка произвольных файлов (heap dumps, CPU profiles) с feature/sampleSize/attr1/attr2/tag.
