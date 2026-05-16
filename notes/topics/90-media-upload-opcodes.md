---
tags: [media, upload, server-control, pms, video]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ydc.java
  - work/jadx_base/sources/defpackage/u0d.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[03-pms-server-flags]]"
  - "[[20-ws-protocol-opcodes]]"
  - "[[89-video-transcoding-config]]"
---

# Media upload — WS-опкоды и speedy-upload

## WS-опкоды загрузки медиа

| Опкод | Enum # | WS code | Что |
|---|---|---|---|
| `PHOTO_UPLOAD` | 82 | 80 | загрузка фото |
| `STICKER_UPLOAD` | 83 | 81 | загрузка стикера |
| `VIDEO_UPLOAD` | 84 | 82 | загрузка видео |
| `FILE_UPLOAD` | 88 | 87 | загрузка файла |

## speedy-upload (#275)

`PmsKey.f275speedyupload` — bool, default `false`. Описание из `ydc.java`: «Фейк прогресс для загрузки видео».

Это **server-gated включение фейкового прогресс-бара** при загрузке видео. При `speedy-upload=true` клиент показывает анимированный прогресс загрузки, который не отражает реальный прогресс. Это UX-оптимизация — пользователь видит «быструю» загрузку.

## upload-hang-barrier

`PmsKey.f290uploadhangbarrier` — long (ms). Таймаут, после которого загрузка считается «зависшей». Сервер задаёт, через сколько миллисекунд считать загрузку проблемной.

## file-upload-max-size (#35)

`PmsKey.f118fileuploadmaxsize` — server-controlled максимальный размер файла для загрузки.

## file-upload-unsupported-types (#36)

`PmsKey.f119fileuploadunsupportedtypes` — server-controlled список типов файлов, которые нельзя загружать.

## Что важно

1. **`speedy-upload` = «Фейк прогресс»** — сервер может включить поддельный прогресс-бар. Это не security-проблема, но показывает, что сервер контролирует даже UX-детали.

2. **`file-upload-unsupported-types`** — сервер контролирует, какие типы файлов можно отправлять. Это механизм content-фильтрации на уровне клиента.

3. **`file-upload-max-size`** — сервер контролирует максимальный размер файла. Может быть использован для ограничения передачи больших файлов.

## Сводка

4 WS-опкода для загрузки медиа. `speedy-upload` — server-gated фейковый прогресс-бар. `file-upload-unsupported-types` — server-controlled content-фильтрация. `file-upload-max-size` — server-controlled лимит размера.
