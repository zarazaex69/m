---
tags: [calls, analytics, upload, config, telemetry]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/analytics/config/UploadConfig.java
  - work/jadx_base/sources/ru/ok/android/externcalls/analytics/config/CallAnalyticsConfig.java
related:
  - "[[147-call-native-analytics-request]]"
  - "[[148-call-external-analytics]]"
---

# UploadConfig — конфиг загрузки аналитики звонков

`UploadConfig` — конфиг для загрузки аналитических данных звонков.

## Дефолтные значения

| Константа | Значение | Что |
|---|---|---|
| `DEFAULT_COMPRESS_CONTENT` | false | не сжимать по умолчанию |
| `DEFAULT_DISABLE_UPLOAD_IN_CALL` | **true** | **отключить загрузку во время звонка** |
| `DEFAULT_LOCAL_FILE_COUNT` | 100 | максимум 100 локальных файлов |
| `DEFAULT_MAX_EVENT_COUNT` | 800 | максимум 800 событий |
| `DEFAULT_MAX_FILE_SIZE_KB` | 15 | максимум 15 KB на файл |
| `DEFAULT_USE_DB_CACHE` | false | не использовать DB кэш |

## Параметры

- `maxTimeToUploadMillis` — максимальное время до загрузки
- `silenceToUploadMillis` — тишина перед загрузкой
- `disableUploadWhenCallIsActiveProvider` — **отключить загрузку во время активного звонка**
- `compressContentProvider` — сжимать ли контент
- `useDbCacheProvider` — использовать ли DB кэш

## Что важно

1. **`DEFAULT_DISABLE_UPLOAD_IN_CALL = true`** — по умолчанию аналитика **не загружается во время активного звонка**. Это оптимизация: не нагружать сеть во время звонка. Загрузка происходит после завершения.

2. **`DEFAULT_MAX_EVENT_COUNT = 800`** — до 800 событий буферизуется локально.

3. **`DEFAULT_MAX_FILE_SIZE_KB = 15`** — каждый файл аналитики не более 15 KB.

## Сводка

`UploadConfig` — конфиг буферизации и загрузки аналитики звонков. Ключевое: загрузка отключена во время активного звонка (DEFAULT_DISABLE_UPLOAD_IN_CALL=true), буфер до 800 событий.
