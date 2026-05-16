---
tags: [facebook, fresco, gif, webp, image-loading, soloader]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/
related:
  - "[[364-manifest-metadata-full]]"
---

# Facebook Fresco — библиотека загрузки изображений

MAX использует Facebook Fresco для загрузки и отображения изображений.

## Компоненты

| Компонент | Что |
|---|---|
| `com.facebook.fresco` | **Fresco image loading library** |
| `com.facebook.animated.gif.GifImage` | **GIF анимации** |
| `com.facebook.animated.webp.WebPImage` | **WebP анимации** |
| `com.facebook.soloader.SoLoader` | **нативные библиотеки** |

## Fresco потоки

| Поток | Что |
|---|---|
| `FrescoIoBoundExecutor` | I/O операции |
| `FrescoDecodeExecutor` | декодирование |
| `FrescoBackgroundExecutor` | фоновые задачи |
| `FrescoLightWeightBackgroundExecutor` | лёгкие фоновые |
| `FrescoAnimationWorker` | анимации |

## Что важно

1. **`com.facebook.soloader.enabled=false`** — SoLoader отключён в AndroidManifest, но используется в коде. Это означает, что нативные библиотеки загружаются через `System.loadLibrary`.

2. **`FrescoHttpDownloadException`** — кастомное исключение для HTTP ошибок при загрузке изображений.

3. **`stat.fresco`** — статистика Fresco хранится в SharedPreferences.

4. **`PmsKey.frescoexecutor`** — сервер может управлять executor Fresco.

## Сводка

Facebook Fresco: GIF/WebP анимации + 5 потоков + SoLoader + FrescoHttpDownloadException + stat.fresco.
