---
tags: [file-paths, storage, surveillance, file-provider]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/lv6.java
related:
  - "[[432-db-uploads-video]]"
  - "[[380-ml-features-manager]]"
---

# lv6 — File Path Manager

`lv6` — менеджер путей к файлам приложения.

## Пути

| Метод | Путь | Что |
|---|---|---|
| `b()` | `getExternalCacheDir()` | **внешний кэш** |
| `n()` | `getFilesDir()` | **внутренние файлы** |
| `c()` | `getCacheDir()` | **кэш** |
| `i()` | `Environment.DIRECTORY_DOWNLOADS` / `getExternalFilesDir()` | **загрузки** |

## FileProvider

`"ru.oneme.app.provider"` — FileProvider для передачи файлов другим приложениям.

## Что важно

1. **`getExternalCacheDir()`** — внешний кэш (доступен другим приложениям).

2. **`getFilesDir()`** — внутренние файлы (только для приложения).

3. **`ru.oneme.app.provider`** — FileProvider для безопасной передачи файлов.

4. **`Environment.DIRECTORY_DOWNLOADS/MAX`** — загрузки сохраняются в папку `MAX`.

## Сводка

`lv6`: externalCacheDir/filesDir/cacheDir/downloadsDir. FileProvider: `ru.oneme.app.provider`. Загрузки: `Downloads/MAX/`.
