---
tags: [native, wave4, functional-decomp, rizin, libgleff, libffmpg, libimagepipeline, libnative-filters, libnative-imagetranscoder, libjlottie, libgifimage, libstatic-webp]
status: complete
sources:
  - work/apktool_arm64/lib/arm64-v8a/libgleff.so
  - work/apktool_arm64/lib/arm64-v8a/libffmpg.so
  - work/apktool_arm64/lib/arm64-v8a/libimagepipeline.so
  - work/apktool_arm64/lib/arm64-v8a/libnative-filters.so
  - work/apktool_arm64/lib/arm64-v8a/libnative-imagetranscoder.so
  - work/apktool_arm64/lib/arm64-v8a/libjlottie.so
  - work/apktool_arm64/lib/arm64-v8a/libgifimage.so
  - work/apktool_arm64/lib/arm64-v8a/libstatic-webp.so
related:
  - "[[09-native-libs]]"
  - "[[545-three-audio-interception-channels]]"
  - "[[546-libqrcode-anti-tamper-code-hiding]]"
  - "[[547-webview-ssl-bypass-deepfake-tflite-key]]"
  - "[[findings/native/libffmpg.cve-crossref.md]]"
---

# Wave 4 — Native Libraries Functional Decomposition

Полный функциональный декомп через `rz-bin` / `rizin` всех ранее не разобранных `.so` из `work/apktool_arm64/lib/arm64-v8a/`.

## Сводная таблица

| Library | Size | Compiler | Exports (FUNC) | JNI exports | Linked libs | Risk |
|---------|------|----------|-----------------|-------------|-------------|------|
| **libgleff.so** | 303 KB | GCC 4.9.x | 7 | 7 | liblog, libGLESv3, libEGL, libm, libdl, libc | **clean** |
| **libffmpg.so** | 1.9 MB | Clang 9.0.9 (NDK r21) | 2461 | 7 | libjnigraphics, liblog, libandroid, libdl, libm, libc | **patched** |
| **libimagepipeline.so** | 8.7 KB | Clang 18.0.2 (NDK r27) | 1 | 0 (dynamic reg) | liblog, libjnigraphics, libc, libm, libdl | **clean** |
| **libnative-filters.so** | 23 KB | Clang 18.0.2 (NDK r27) | 1 | 0 (dynamic reg) | liblog, libjnigraphics, libc, libm, libdl | **clean** |
| **libnative-imagetranscoder.so** | 585 KB | Clang 18.0.2 (NDK r27) | 68 | 0 (dynamic reg) | liblog, libc, libm, libdl | **clean** |
| **libjlottie.so** | 1.0 MB | GCC 4.9.x | 673 | 9 | libjnigraphics, libdl, libm, libc | **clean** |
| **libgifimage.so** | 319 KB | (Fresco) | 130 | 0 (dynamic reg) | (standard) | **clean** |
| **libstatic-webp.so** | 502 KB | (Fresco) | 127 | 0 (dynamic reg) | (standard) | **clean** |

## Детальный анализ по каждой .so

---

### 1. libgleff.so (303 384 bytes)

**Назначение:** OpenGL ES 3.0 video message stencil renderer для круглых видеосообщений.

**JNI namespace:** `one.me.sdk.gl.effects.VideoMessageStencilHolder`

**7 JNI exports:**
- `createNativeInstance`, `handleTextureId`, `onStartRecording`, `onStopRecording`, `release`, `render`, `updateTextures`

**Шейдеры (inline GLSL в .rodata):**
1. Fragment: простой `texture2D(sTexture, vTextureCoord) * colorMatrix` с alpha
2. Vertex: стандартный MVP transform
3. Fragment passthrough: `gl_FragColor = texture2D(sTexture, vTextureCoord)`

Все шейдеры — тривиальные 2D-текстурные операции. **Нет** compute shaders, SSBO, transform feedback, imageStore или любых механизмов для exfiltration данных через GPU.

**Imports:** Только GL/EGL + libc. **Нет** сетевых, файловых (кроме openlog), криптографических импортов.

**Strings:** 434 строки. Все — GL API names, C++ runtime, shader source. **Нет** URL, доменов, ключей.

**Вердикт: CLEAN.** Стандартный GL effects renderer, функционально эквивалентен Telegram's video message circle renderer. Никаких отклонений от заявленного назначения.

---

### 2. libffmpg.so (1 954 240 bytes)

**Назначение:** FFmpeg n4.4.3 (GPL v3) + libyuv (bundled) — медиа-декодирование для animated stickers/GIF preview.

**JNI namespace:** `one.me.sdk.media.ffmpeg.AnimatedFileDrawable`

**7 JNI exports:**
- `createDecoder`, `destroyDecoder`, `stopDecoder`, `prepareToSeek`, `seekToMs`, `getVideoFrame`, `getFrameAtTime`

**Состав exports (2461 total):**
- FFmpeg API (av_/ff_/avio_/sws_): 948
- libyuv pixel conversion: 840
- Java JNI: 7
- C++ std:: runtime: 172
- vpx codec internals: ~100
- Прочие (color conversion, scale helpers): ~394

**Build tag:** `?oneme_ffmpeg_native` (строка в .rodata offset 0x5484f) — подтверждает кастомную сборку.

**Версия:** `n4.4.3` (строка в .rodata offset 0x56ec8). Лицензия: GPL v3 (libavcodec, libavformat, libavutil, libswscale — все GPL v3).

**Сетевые возможности: ОТСУТСТВУЮТ.**
- Imports: 142 функции. **Нет** socket/connect/send/recv/getaddrinfo/DNS. Только: file I/O (open/read/write/fopen/fclose), memory, threading, math, bitmap.
- Протоколы `http`/`https`/`tls` **зарегистрированы как строки** в codec registry, но **не могут быть использованы** — нет сетевых syscall imports. Это мёртвый код из стандартной FFmpeg сборки.
- `ffurl_connect` export присутствует, но без socket imports он не может установить соединение.

**Кастомизация vs upstream FFmpeg n4.4.3:**
- Bundled libyuv (840 exports) — стандартная практика для Android media apps (Telegram делает то же)
- `?oneme_ffmpeg_native` build tag
- Конфигурация стёрта (avcodec_configuration() возвращает пустую строку)
- Широкий набор кодеков вкомпилен (h264/hevc/av1/vp8/vp9/aac/opus/vorbis/flac + десятки legacy)
- Сетевые протоколы зарегистрированы но не функциональны (нет imports)

**Hardcoded keys/URLs:** НЕТ. Строки — стандартные FFmpeg error messages, codec names, format descriptions.

**CVE exposure:** Подробно в `findings/native/libffmpg.cve-crossref.md`. Ключевое: 6+ CVE с reachable компонентами, включая CVE-2025-9951 (jpeg2000 heap overflow) и CVE-2026-40962 (MOV CENC int overflow). Версия 3.5 года без обновления.

**Вердикт: PATCHED (minor).** Кастомная сборка FFmpeg n4.4.3 + libyuv. Функционально — только локальное декодирование файлов для превью. Нет сетевых возможностей в нативном коде. Основной риск — устаревшая версия с известными CVE (attack surface через crafted media files), но это не backdoor, а техдолг. Нет hardcoded ключей, URL, или скрытой функциональности.

---

### 3. libimagepipeline.so (8 760 bytes)

**Назначение:** Facebook Fresco image pipeline — native memory management + bitmap pinning.

**JNI namespace:** `com.facebook.imagepipeline.nativecode.DalvikPurgeableDecoder`, `com.facebook.imagepipeline.nativecode.Bitmaps`, `com.facebook.imagepipeline.memory.NativeMemoryChunk`

**Exports:** 1 (JNI_OnLoad — dynamic registration)

**Registered methods (from strings):**
- `nativePinBitmap`, `nativeFree`, `nativeCopyBitmap`, `nativeCopyFromByteArray`, `nativeReadByte`, `nativeMemcpy`, `nativeAllocate`, `nativeCopyToByteArray`

**Imports:** Только libjnigraphics (AndroidBitmap_*) + libc. **Нет** сетевых, криптографических, файловых операций.

**Вердикт: CLEAN.** Стандартный Fresco `libimagepipeline.so` без модификаций. Идентичен open-source версии по API surface.

---

### 4. libnative-filters.so (23 712 bytes)

**Назначение:** Facebook Fresco native image filters — blur + rounding.

**JNI namespace:** `com.facebook.imagepipeline.nativecode.NativeBlurFilter`, `com.facebook.imagepipeline.nativecode.NativeRoundingFilter`

**Exports:** 1 (JNI_OnLoad — dynamic registration)

**Registered methods (from strings):**
- `nativeIterativeBoxBlur`, `nativeToCircleFilter`, `nativeToCircleFastFilter`, `nativeToCircleWithBorderFilter`, `nativeAddRoundedCornersFilter`

**Imports:** Только libjnigraphics + libc. **Нет** ничего подозрительного.

**Вердикт: CLEAN.** Стандартный Fresco blur/rounding filter. Pixel-level bitmap operations only.

---

### 5. libnative-imagetranscoder.so (585 376 bytes)

**Назначение:** Facebook Fresco JPEG transcoder (libjpeg-turbo 2.1.5.1, build 20240418) + libunwind.

**JNI namespace:** `com.facebook.imagepipeline.nativecode.NativeJpegTranscoder`

**Exports:** 68 (JNI_OnLoad + C++ exception handling runtime + libunwind)

**`__system_property_get` usage:**
- Reads `ro.arch`, compares against `"exynos9810"` — **стандартный Samsung Exynos workaround** для JPEG decoding bug на Galaxy S9/Note9. Присутствует в upstream Fresco.

**Imports:** fopen, fwrite, getenv, __system_property_get + libc. **Нет** сетевых, криптографических.

**libjpeg-turbo version:** 2.1.5.1 (build 20240418) — относительно свежая.

**Вердикт: CLEAN.** Стандартный Fresco JPEG transcoder с Samsung workaround. Без модификаций.

---

### 6. libjlottie.so (1 001 048 bytes)

**Назначение:** rlottie (Samsung's Lottie renderer) — native Lottie animation rendering.

**JNI namespace:** `one.me.rlottie.RLottieDrawable`

**9 JNI exports:**
- `create`, `createWithJson`, `destroy`, `getFrame`, `getFramesCount`, `getDuration`, `replaceColors`, `setLayerColor`, `foo` (stub, returns 4 bytes — likely version check)

**dlopen/dlsym usage:**
- `dlopen("librlottie-image-loader.so", RTLD_LAZY)` — загрузка опционального image loader plugin
- `dlsym` resolves: `lottie_image_load`, `lottie_image_free`, `lottie_image_load_from_data`
- **`librlottie-image-loader.so` НЕ присутствует в APK** — это optional plugin для embedded images в Lottie JSON, gracefully fails если не найден.

**Imports:** fopen/fread/fwrite (для чтения .json файлов), dlopen/dlsym (для optional plugin), math, threading. **Нет** сетевых, криптографических.

**Strings:** 645 строк. Все — Lottie JSON parsing (shapes, layers, assets, masks, transforms), C++ runtime, locale. **Нет** URL, доменов, ключей.

**Java side:** `RLottieDrawable.create(filePath, ...)` — принимает только локальный путь к файлу. Загрузка Lottie JSON из сети происходит на Java-стороне, нативная библиотека работает только с локальными файлами.

**Вердикт: CLEAN.** Стандартный rlottie (Samsung open-source, https://github.com/nicktgn/rlottie-android pattern). Namespace переименован в `one.me.rlottie` но функциональность идентична upstream. Нет code injection через Lottie URL — нативный код не имеет сетевых возможностей.

---

### 7. libgifimage.so (318 992 bytes) [бонус — не в исходном задании]

**Назначение:** Facebook Fresco GIF decoder.

**JNI namespace:** `com.facebook.animated.gif.GifImage`, `com.facebook.animated.gif.GifFrame`

**`__system_property_get`:** `ro.arch` vs `exynos9810` — тот же Samsung workaround.

**Вердикт: CLEAN.** Стандартный Fresco GIF decoder.

---

### 8. libstatic-webp.so (501 856 bytes) [бонус — не в исходном задании]

**Назначение:** Facebook Fresco WebP decoder (static build).

**`__system_property_get`:** `ro.arch` vs `exynos9810` — тот же Samsung workaround.

**Вердикт: CLEAN.** Стандартный Fresco WebP decoder.

---

## Сравнение с upstream

| Library | Upstream | Отклонения |
|---------|----------|-----------|
| libgleff.so | Нет public upstream (one.me proprietary) | N/A — проприетарный, но тривиальный GL renderer |
| libffmpg.so | FFmpeg n4.4.3 + libyuv | Build tag `oneme_ffmpeg_native`, configuration string стёрта, bundled libyuv, сетевые протоколы мертвы (нет socket imports) |
| libimagepipeline.so | Facebook Fresco 3.x | Идентичен upstream по API |
| libnative-filters.so | Facebook Fresco 3.x | Идентичен upstream по API |
| libnative-imagetranscoder.so | Facebook Fresco 3.x + libjpeg-turbo 2.1.5.1 | Идентичен upstream по API |
| libjlottie.so | rlottie (Samsung) | Namespace `one.me.rlottie` вместо оригинального, `foo()` stub добавлен |
| libgifimage.so | Facebook Fresco 3.x | Идентичен upstream |
| libstatic-webp.so | Facebook Fresco 3.x | Идентичен upstream |

## Hardcoded Keys / URLs / Suspect Functions

**НЕ НАЙДЕНО** ни в одной из проанализированных библиотек:
- Hardcoded crypto keys (16/32 byte high-entropy blobs в .rodata)
- URL strings с доменами MAX/VK/OK/apptracer/trace-flow
- Вызовы execve/popen/system
- Сетевые imports (socket/connect/send/recv)
- Криптографические imports (AES/RSA/EVP/HMAC/SSL)

Единственный «подозрительный» паттерн — `__system_property_get("ro.arch")` в трёх Fresco-библиотеках — подтверждён как стандартный Samsung Exynos workaround (сравнение с `"exynos9810"`).

## Что НЕ является backdoor-индикатором

1. **dlopen в libjlottie.so** — загружает `librlottie-image-loader.so` (стандартный rlottie plugin pattern), библиотека отсутствует в APK, graceful fallback.
2. **`__system_property_get` в Fresco libs** — Samsung Exynos JPEG bug workaround, присутствует в upstream Fresco.
3. **Мёртвые HTTP/HTTPS protocol strings в libffmpg.so** — зарегистрированы в FFmpeg codec registry, но без socket imports не могут быть использованы.
4. **`getenv` в libffmpg.so** — стандартный FFmpeg pattern для `FFREPORT`, `AV_LOG_FORCE_NOCOLOR` и т.п.

## Итоговая оценка

Из 6 целевых + 2 бонусных нативных библиотек:
- **0 suspect** — ни одна не содержит скрытой функциональности, hardcoded ключей, сетевых backdoor'ов или exfiltration-механизмов
- **1 patched** (libffmpg.so) — кастомная сборка с техдолгом (3.5 года без обновления, 6+ reachable CVE)
- **7 clean** — стандартные upstream-библиотеки без модификаций

**Вывод:** Все surveillance/backdoor-примитивы в MAX сосредоточены в:
- `libEnhancementLibShared.so` (ASR/KWS/Wav2Lip/voiceprints — topic 545, 547)
- `libqrcode.so` (anti-tamper — topic 546)
- `libjingle_peerconnection_so.so` (audio interception hooks — topic 545)
- Java-слой (DPS/TraceFlow, WS opcodes, PmsKeys — topics 542-544)

Остальные нативные библиотеки — стандартные media/image processing без скрытой функциональности.

## Кандидаты на topic 548+

**Нет новых кандидатов** из этого анализа. Все подозрительные гипотезы (GL shader exfiltration, Lottie URL injection, Fresco modifications, FFmpeg network backdoor) **опровергнуты** статическим анализом.

Единственная рекомендация: **libffmpg.so CVE exposure** заслуживает отдельного упоминания в FINDINGS.md как attack surface (crafted media → RCE через jpeg2000/MOV), но это не backdoor, а техдолг.
