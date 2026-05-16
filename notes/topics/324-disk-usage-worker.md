---
tags: [telemetry, apptracer, disk-usage, surveillance, internal-data, external-data]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tracer/disk/usage/DiskUsageWorker.java
  - work/jadx_base/sources/defpackage/it7.java
related:
  - "[[17-apptracer-uplink]]"
  - "[[323-apptracer-all-endpoints]]"
---

# DiskUsageWorker — сбор данных об использовании диска

`DiskUsageWorker` — сбор и загрузка данных об использовании дискового пространства.

## Категории

| Категория | Что |
|---|---|
| `INTERNAL_DATA` (`int_data`) | **внутренние данные** (`applicationInfo.dataDir`) |
| `EXTERNAL_DATA` (`ext_data`) | **внешние данные** (`getExternalFilesDir`) |
| `SRC` (`src_data`) | исходные данные |

## Структура данных

```json
{
  "consumers": {
    "int_data": {
      "size": <bytes>,
      "name": "<name>",
      "is_dir": true,
      "is_overflow": true,
      "is_excluded": true,
      "children": [...]
    }
  },
  "total_size": <bytes>
}
```

## Что важно

1. **`applicationInfo.dataDir`** — полный путь к внутренним данным приложения. Сервер знает размер каждой директории.

2. **`getExternalFilesDir`** — внешние данные. Сервер знает размер внешнего хранилища.

3. **`children`** — рекурсивный список файлов/директорий (до 20 элементов).

4. **`is_overflow`** — флаг переполнения (слишком много файлов).

## Сводка

`DiskUsageWorker`: int_data(applicationInfo.dataDir)/ext_data(externalFilesDir)/total_size. Рекурсивный список файлов.
