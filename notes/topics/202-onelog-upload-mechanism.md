---
tags: [telemetry, onelog, upload, fingerprint, collector]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/onelog/Uploader.java
  - work/jadx_base/sources/ru/ok/android/onelog/Collector.java
  - work/jadx_base/sources/ru/ok/android/onelog/OneLogApiRequest.java
  - work/jadx_base/sources/ru/ok/android/onelog/UploadService.java
  - work/jadx_base/sources/ru/ok/android/onelog/StreamingOneLogItemsApiValue.java
related:
  - "[[04-telemetry-endpoints]]"
  - "[[61-leakcanary-gost-debug-flags]]"
---

# OneLog — механизм загрузки телеметрии

OneLog — система сбора и загрузки телеметрии. Используется двумя инстансами (см. [[04-telemetry-endpoints]]).

## Архитектура

```
OneLogAppender → Collector (файл) → Uploader → OneLogApiRequest → сервер
```

**Collector** — буферизует события в файлы:
- Директория: `context.getFilesDir()/onelog/<collector>/`
- Два файла: `append` (текущий) и `upload` (готовый к отправке)

**UploadService** — `IntentService` с action `ru.ok.android.onelog.action.UPLOAD`, scheme `one-log`.

## OneLogApiRequest — параметры запроса

| Поле | Что |
|---|---|
| `collector` | имя коллектора (endpoint) |
| `application` | `<packageName>:<versionCode>:<versionName>` |
| `platform` | `android:<phone|tablet>:<Build.VERSION.RELEASE>` |

**`application`** = `ru.oneme.app:26153:26.15.3` — точная версия приложения.

**`platform`** = `android:phone:14` — платформа + форм-фактор + версия Android.

## Два режима загрузки

- **`SimpleOneLogItemsApiValue`** — загрузка из памяти (List<OneLogItem>)
- **`StreamingOneLogItemsApiValue`** — потоковая загрузка из файла (InputStreamReader)

## Что важно

1. **`application` и `platform`** — в каждом запросе. Сервер знает точную версию приложения и версию Android.

2. **Форм-фактор** определяется по `smallestScreenWidthDp < 600` — phone vs tablet.

3. **Файловый буфер** — события сначала пишутся в файл, потом загружаются. Это означает, что события не теряются при обрыве сети.

4. **`UploadService`** — отдельный `IntentService`. Загрузка происходит в фоне.

## Сводка

OneLog: `Collector` (файловый буфер) → `Uploader` → `OneLogApiRequest(collector/application/platform)`. `application=ru.oneme.app:26153:26.15.3`, `platform=android:phone:<version>` в каждом запросе.
