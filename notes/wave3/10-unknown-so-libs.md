---
tags: [native, supply-chain, so-libs, integrity, anti-tamper, wave3]
status: confirmed
sources:
  - work/apktool_arm64/lib/arm64-v8a/
  - findings/native/libgleff.exports.txt
  - work/jadx_base/sources/one/me/callssdk/CallsSdkInitializer.java
related:
  - "[[09-native-libs]]"
  - "[[443-calls-sdk-initializer]]"
---

# Wave 3: Анализ «неизвестных» .so библиотек на supply-chain / backdoor

## TL;DR

**Ни одна из «неизвестных» библиотек не содержит сетевых вызовов, скрытых хостов, или backdoor-функциональности.** Все идентифицированы как стандартные open-source компоненты или собственный код MAX/one.me без network capability.

**Единственная нетривиальная находка:** `libqrcode.so` содержит не только QR-генератор, но и **нативный anti-tamper / integrity protection** для CallsSDK (`sign_check`, `IntegrityProtectionInit`, `initializeSessionSeed`), включая верификацию APK-подписи через PackageManager. Это code-hiding — критическая security-функция спрятана в библиотеке с невинным именем. Ранее задокументировано только как «QR-сканер» (topic 09).

---

## Методология

Для каждой библиотеки проверено:
- `rz-bin -E` (экспорты) / `rz-bin -i` (импорты)
- `strings` (хосты, URL, IP, base64, /proc, /dev)
- `rz-bin -I` (ELF security: canary, NX, RELRO, PIE)
- `readelf -n` (ELF NOTE: build-id, android.ident)
- Поиск подозрительных импортов: socket/connect/send/recv/dlopen/dlsym/exec/SSL

---

## Сводная таблица

| Библиотека | Размер | Идентификация | Compiler | NDK | Canary | Network | Подозрительное |
|---|---|---|---|---|---|---|---|
| libgleff.so | 303 KB | GL effects для видеосообщений (one.me.sdk.gl_effects) | clang 11.0.5 | r22b | ✅ | ❌ | Нет |
| libqrcode.so | 138 KB | QR-генератор + **IntegrityProtection/CallsSDK anti-tamper** | clang 11.0.5 | r22b | ✅ | ❌ | **⚠️ Code hiding** |
| libsurface_util_jni.so | 4.8 KB | AndroidX CameraX SurfaceUtil | clang 18.0.1 | r27 | ✅ | ❌ | Нет |
| libimage_processing_util_jni.so | 32 KB | AndroidX CameraX ImageProcessingUtil | clang 18.0.1 | r27 | ✅ | ❌ | Нет |
| libnative-filters.so | 23 KB | Facebook Fresco NativeBlurFilter + NativeRoundingFilter | clang 18.0.2 | r27b | ✅ | ❌ | Нет |
| libnative-imagetranscoder.so | 585 KB | Facebook Fresco NativeJpegTranscoder (libjpeg-turbo 2.1.5.1) | clang 18.0.2 | r27b | ✅ | ❌ | Нет |
| libgifimage.so | 319 KB | Facebook Fresco animated-gif (GIFLIB) | clang 18.0.2 | r27b | ✅ | ❌ | Нет |
| libimagepipeline.so | 8.7 KB | Facebook Fresco NativeMemoryChunk + Bitmaps | clang 18.0.2 | r27b | ✅ | ❌ | Нет |
| libstatic-webp.so | 502 KB | Facebook Fresco + libwebp decoder | clang 18.0.2 | r27b | ✅ | ❌ | Нет |
| libjlottie.so | 1.0 MB | rlottie (Lottie animation, one.me.rlottie) | clang 11.0.5 | r22b | ✅ | ❌ | dlopen (см. ниже) |
| libzstd.so | 65 KB | Zstandard decompression JNI wrapper (one.me.sdk.zsrd.ZstdUtil) | clang 11.0.5 | r22b | **❌** | ❌ | No canary |
| libc++_shared.so | 1.3 MB | NDK libc++ runtime | clang 18.0.1 | r27 | — | ❌ | Нет |

---

## Детальный анализ по библиотекам

### 1. libgleff.so — GL Effects для видеосообщений

**Что это:** Нативная OpenGL ES библиотека для рендеринга стенсилов (масок) видеосообщений.

**Экспорты (7 JNI):**
```
Java_one_me_sdk_gl_effects_VideoMessageStencilHolder_createNativeInstance
Java_one_me_sdk_gl_effects_VideoMessageStencilHolder_handleTextureId
Java_one_me_sdk_gl_effects_VideoMessageStencilHolder_onStartRecording
Java_one_me_sdk_gl_effects_VideoMessageStencilHolder_onStopRecording
Java_one_me_sdk_gl_effects_VideoMessageStencilHolder_release
Java_one_me_sdk_gl_effects_VideoMessageStencilHolder_render
Java_one_me_sdk_gl_effects_VideoMessageStencilHolder_updateTextures
```

**Импорты:** Только OpenGL ES (glBind*, glDraw*, glTexture*, eglGetProcAddress), math (sincos, acosf, atan2f), memory (malloc/free/memcpy). Никаких сетевых или файловых вызовов.

**Вердикт:** ✅ Чистая. Собственный код one.me для GL-эффектов в видеосообщениях. Не Cisco Webex.

---

### 2. libqrcode.so — QR-генератор + СКРЫТЫЙ anti-tamper

**Что это:** Двойное назначение:
1. QR-код генератор (`nativeGenerateQR`, `nativeRenderSvg`, SVG-рендеринг)
2. **Integrity Protection для CallsSDK** (`IntegrityProtectionInit`, `sign_check`, `initializeSessionSeed`)

**Экспорты:** Только `JNI_OnLoad` (регистрирует нативные методы динамически).

**Ключевые строки:**
```
one/me/callssdk/CallsSdkInitializer
initializeSessionSeed
IntegrityProtectionInit
sign_check
signatures not found
Failed to register native methods for calls sdk, e=%d
one/me/sdk/uikit/qr/QrCodeGenerator
(Landroid/content/Context;[B[B)[B
```

**Что делает native integrity code:**
- Получает `Context` → `PackageManager` → `SigningInfo` → проверяет APK-подпись
- `sign_check` — верификация что APK подписан ожидаемым ключом
- `IntegrityProtectionInit` — инициализация anti-tamper
- `initializeSessionSeed(context, seed, deviceId)` → возвращает `byte[]` — криптографический seed для сессии звонков, привязанный к целостности APK

**Зачем спрятано в libqrcode.so:** Anti-tamper код намеренно размещён в библиотеке с невинным именем, чтобы затруднить его обнаружение и патчинг. Если атакующий модифицирует APK и не патчит libqrcode.so, звонки не будут работать (неверный session seed).

**Импорты:** Только стандартные (math, memory, string, fopen/fwrite для SVG). Нет сетевых вызовов — вся сетевая отправка seed происходит на Java-уровне.

**Вердикт:** ⚠️ Не backdoor, но **code hiding** — security-critical функция спрятана в библиотеке с нерелевантным именем. Ранее задокументировано только как «QR-сканер».

---

### 3. libsurface_util_jni.so — AndroidX CameraX

**Что это:** Стандартная библиотека из `androidx.camera.core.impl.utils.SurfaceUtil`.

**Единственный экспорт:**
```
Java_androidx_camera_core_impl_utils_SurfaceUtil_nativeGetSurfaceInfo
```

**Импорты:** ANativeWindow_fromSurface, ANativeWindow_getFormat/Width/Height, ANativeWindow_release. Минимальный набор для получения информации о Surface.

**Вердикт:** ✅ Стандартный CameraX. 4.8 KB — слишком мал для чего-либо скрытого.

---

### 4. libimage_processing_util_jni.so — AndroidX CameraX

**Что это:** Стандартная библиотека из `androidx.camera.core.ImageProcessingUtil`.

**Экспорты (8 JNI):**
```
nativeShiftPixel, nativeConvertAndroid420ToBitmap, nativeCopyBetweenByteBufferAndBitmap,
nativeWriteJpegToSurface, nativeConvertAndroid420ToABGR, nativeRotateYUV,
nativeGetYUVImageVUOff, nativeNewDirectByteBuffer
```

**Импорты:** AndroidBitmap_*, ANativeWindow_*, malloc/free/memcpy. Чистая image processing.

**Вердикт:** ✅ Стандартный CameraX image processing.

---

### 5. libnative-filters.so — Facebook Fresco

**Что это:** Facebook Fresco `NativeBlurFilter` + `NativeRoundingFilter`.

**Строки подтверждают:**
```
com/facebook/imagepipeline/nativecode/NativeBlurFilter
com/facebook/imagepipeline/nativecode/NativeRoundingFilter
BlurFilter_iterativeBoxBlur: ...
Rounding#toCircleWithOptionalBorder: ...
```

**Импорты:** AndroidBitmap_getInfo/lockPixels/unlockPixels, malloc/free/memset/memcpy. Только bitmap-операции.

**Вердикт:** ✅ Стандартный Fresco.

---

### 6. libnative-imagetranscoder.so — Facebook Fresco + libjpeg-turbo

**Что это:** Fresco `NativeJpegTranscoder` с встроенным libjpeg-turbo 2.1.5.1 (build 20240418).

**Строки:** `com/facebook/imagepipeline/nativecode/NativeJpegTranscoder`, стандартные JPEG error messages.

**Импорты:** Стандартные + `__system_property_get` (для `ro.arch` — определение архитектуры CPU для SIMD-оптимизаций). Также `openlog`/`syslog` — стандартный logging libjpeg-turbo. Читает `/proc/cpuinfo` для CPU feature detection.

**Вердикт:** ✅ Стандартный Fresco + libjpeg-turbo. `__system_property_get("ro.arch")` — штатное поведение для SIMD dispatch.

---

### 7. libgifimage.so — Facebook Fresco animated-gif

**Что это:** Fresco animated GIF decoder (GIFLIB).

**Строки:** `com/facebook/animated/gif/GifImage`, `com/facebook/animated/gif/GifFrame`, DGif* API.

**Импорты:** `__system_property_get` (для `ro.arch`), file I/O (dup, fdopen, fseek, fread), bitmap ops.

**Вердикт:** ✅ Стандартный Fresco animated-gif.

---

### 8. libimagepipeline.so — Facebook Fresco core

**Что это:** Fresco `NativeMemoryChunk` + `Bitmaps` + `DalvikPurgeableDecoder`.

**Строки:** `com/facebook/imagepipeline/memory/NativeMemoryChunk`, `com/facebook/imagepipeline/nativecode/Bitmaps`.

**Вердикт:** ✅ Стандартный Fresco. 8.7 KB — минимальный JNI wrapper.

---

### 9. libstatic-webp.so — Facebook Fresco + libwebp

**Что это:** Fresco WebP decoder с встроенным libwebp.

**Экспорты:** WebPDecode*, WebPDemux*, WebPGetFeatures*, WebPINew* — стандартный libwebp API.

**Импорты:** `__system_property_get` (для `ro.arch`). Нет сетевых вызовов.

**Вердикт:** ✅ Стандартный Fresco + libwebp.

---

### 10. libjlottie.so — rlottie (Lottie animation)

**Что это:** Samsung rlottie — нативный рендерер Lottie-анимаций. Пакет `one.me.rlottie`.

**Экспорты (9 JNI):**
```
Java_one_me_rlottie_RLottieDrawable_create/createWithJson/destroy/foo/
getDuration/getFrame/getFramesCount/replaceColors/setLayerColor
```

**⚠️ dlopen/dlsym:** Пытается загрузить `librlottie-image-loader.so` для поддержки embedded images в Lottie-файлах. **Этот .so отсутствует в APK** — значит dlopen вернёт NULL и image loading будет disabled. Это стандартное поведение rlottie (опциональный image loader plugin).

**Вердикт:** ✅ Стандартный rlottie. dlopen безопасен — целевой .so отсутствует, fallback на отсутствие image loading.

---

### 11. libzstd.so — Zstandard decompression

**Что это:** JNI wrapper для Zstandard decompression. Класс `one.me.sdk.zsrd.ZstdUtil`.

**Экспорты:** Только `JNI_OnLoad` (регистрирует `nativeDecompress`).

**Импорты:** malloc/free/memcpy/memmove/memset + ZSTD_trace_decompress_begin/end (weak symbols для tracing).

**⚠️ Нет stack canary** (`canary: false`). Единственная библиотека без canary. При 65 KB размера и отсутствии сетевых вызовов — низкий риск, но потенциально эксплуатируемый buffer overflow при обработке crafted zstd-фреймов.

**Вердикт:** ⚠️ Низкий риск. Отсутствие canary — слабость, но не backdoor.

---

## ELF Security Summary

| Свойство | Все 11 библиотек |
|---|---|
| RELRO | Full ✅ |
| NX | Enabled ✅ |
| PIE | Shared lib (position-independent by definition) ✅ |
| Stripped | Yes ✅ |
| Stack Canary | Все кроме libzstd.so ✅ |

---

## Compiler Toolchain Analysis

Две группы по возрасту toolchain:

**Старые (NDK r22b, clang 11.0.5, ~2021):**
- libgleff.so, libqrcode.so, libjlottie.so, libzstd.so

**Новые (NDK r27/r27b, clang 18.0.1-18.0.2, ~2024):**
- libsurface_util_jni.so, libimage_processing_util_jni.so, libnative-filters.so, libnative-imagetranscoder.so, libgifimage.so, libimagepipeline.so, libstatic-webp.so

**Интерпретация:** Старые библиотеки (gleff, qrcode, jlottie, zstd) — собственный код one.me/TamTam, не обновлявшийся с 2021. Новые — стандартные AndroidX/Fresco, обновляемые через Gradle dependencies.

---

## Diff 26.15.3 → 26.16.0

ARM64 APK для 26.16.0 **отсутствует** в репозитории (только base + xxhdpi). Из `native_diff/` видно что сравнивались только:
- `libEnhancementLibShared.so`: уменьшился с 5.72 MB → 5.66 MB, потерял 26 экспортов (396→370) — удаление KWS
- `libjingle_peerconnection_so.so`: уменьшился с 12.78 MB → 12.77 MB

Остальные .so **не сравнивались** из-за отсутствия arm64 split APK для 26.16.0.

---

## Сетевые вызовы в native code

**Результат: НОЛЬ.** Ни одна из 11 проверенных библиотек не импортирует:
- socket / connect / bind / listen / accept
- send / recv / sendto / recvfrom
- getaddrinfo / gethostbyname / inet_pton
- SSL_* / OPENSSL_* / curl_*

Все сетевые операции MAX происходят исключительно на Java-уровне (OkHttp, WebSocket).

---

## ELF NOTE Signatures

Ни одна библиотека не содержит криптографической подписи в ELF NOTE sections. Присутствуют только:
- `.note.android.ident` — NDK version + build number
- `.note.gnu.build-id` — уникальный build hash

Это стандартно для Android NDK — подпись APK покрывает все файлы включая .so.

---

## Выводы

1. **Нет backdoor'ов или supply-chain атак** в «неизвестных» .so библиотеках.
2. **Нет сетевых вызовов** ни в одной из них — вся сеть на Java-уровне.
3. **Единственная нетривиальная находка:** `libqrcode.so` содержит anti-tamper код для CallsSDK, спрятанный под именем QR-библиотеки. Это не backdoor, но **намеренное code hiding** — усложняет патчинг/модификацию APK.
4. **libzstd.so** — единственная без stack canary. Низкий риск (нет network surface).
5. **libjlottie.so** — dlopen `librlottie-image-loader.so` безопасен (файл отсутствует, стандартный rlottie plugin mechanism).
6. Все библиотеки идентифицированы: 4× Fresco (Facebook), 2× AndroidX CameraX, 1× rlottie, 1× zstd, 1× GL effects, 1× QR+integrity.
