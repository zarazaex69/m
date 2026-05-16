---
tags: [media-observer, content-observer, gallery, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/wh8.java
  - work/jadx_base/sources/defpackage/eh8.java
related:
  - "[[06-contacts]]"
  - "[[23-camera-mic-screen-entry-points]]"
---

# wh8 — Media ContentObserver (наблюдение за медиа)

`wh8` — наблюдатель за изменениями медиа-файлов через `ContentObserver`.

## Наблюдаемые URI

```java
MediaStore.Images.Media.EXTERNAL_CONTENT_URI  // внешние изображения
MediaStore.Video.Media.EXTERNAL_CONTENT_URI   // внешние видео
MediaStore.Images.Media.INTERNAL_CONTENT_URI  // внутренние изображения
```

## Логика

При изменении медиа (`onChange`) → `"ContentObserver: on content changed"` → обновление галереи.

## Что важно

1. **Три URI** — наблюдение за внешними и внутренними изображениями и видео.

2. **`registerContentObserver(..., true, ...)`** — `true` = наблюдение за всеми дочерними URI.

3. При изменении галереи приложение автоматически обновляется.

## Сводка

`wh8`: `registerContentObserver` для `MediaStore.Images.EXTERNAL/INTERNAL` + `MediaStore.Video.EXTERNAL`. `onChange` → обновление галереи.
