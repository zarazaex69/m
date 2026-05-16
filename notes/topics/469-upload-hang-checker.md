---
tags: [upload-hang, telemetry, surveillance, file-upload, video-upload]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/l0d.java
related:
  - "[[467-telemetry-event-types]]"
  - "[[432-db-uploads-video]]"
---

# l0d — Upload Hang Checker (проверка зависания загрузки)

`l0d` — `OneVideoUploadOperation` — загрузка видео/аудио с проверкой зависания.

## Логика

1. Проверяет существование файла
2. Проверяет ненулевой размер файла
3. Если `DevNullServerConfig.isUploadHangCheckEnabled()` → запускает hang checker

## Что важно

1. **`upload_hang`** — если загрузка зависает, событие отправляется на сервер.

2. **Поддерживаемые типы**: `UploadType.VIDEO`, `UploadType.VIDEO_MESSAGE`, `UploadType.AUDIO`.

3. **`file.length()`** — размер файла проверяется перед загрузкой.

4. Hang checker запускается в отдельном потоке.

## Сводка

`l0d`: OneVideoUploadOperation с hang checker. Если `upload_hang` включён → мониторинг зависания загрузки.
