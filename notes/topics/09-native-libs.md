# Native-библиотеки: что это и что они делают

В `lib/arm64-v8a/` шестнадцать `.so`. Перечень с пояснениями:

| Файл | Размер | Что это |
|---|---|---|
| `libjingle_peerconnection_so.so` | 12.7 MB | WebRTC/libjingle. Strings: `WebRTC.Call.LifetimeInSeconds`, `WebRTC.Audio.AndroidNativeAudioBufferSizeMs`, `WebRTC-Bwe-SafeResetOnRouteChange`, ABI mismatch banner. Сборка кастомизирована (есть VK-специфичные имена), но базово — WebRTC. Все звонки идут через неё. |
| `libEnhancementLibShared.so` | 5.7 MB | DSP-обработка аудио для звонков; namespace `vk::enh::EnhancementFactory`. Классы алгоритмов: `Df`, `GTCRNTF`. Сюда передаётся raw audio до того, как его передадут в WebRTC encoder. То есть VK-овский ноис-саппрешшен и поток-обработка живут тут. |
| `libffmpg.so` | 1.9 MB | **FFmpeg n4.4.3** (видно в strings: `FFmpeg version n4.4.3`). Используется для transcoding медиа. Версия древняя — n4.4.3 от 2022 года, в ней набор known-CVE. Контекстно: `https protocol not found, recompile FFmpeg with openssl, gnutls or securetransport enabled.` — TLS не вкомпилен (для медиа-трансcode и не надо). |
| `libjlottie.so` | 1.0 MB | jlottie — нативная либа Lottie-анимаций (стикеры, эмодзи) |
| `libtracernative.so` | 799 KB | Нативный мост Apptracer (OK.ru): минидампы при крэше, NativeBridgeInstaller. Перехватывает SIGSEGV/SIGABRT. |
| `libgifimage.so` | 319 KB | GIF-декодер (Fresco) |
| `libgleff.so` | 303 KB | GL-эффекты (фильтры) |
| `libstatic-webp.so` | 502 KB | WebP-декодер (Fresco) |
| `libnative-imagetranscoder.so` | 585 KB | image transcoder (Fresco) |
| `libqrcode.so` | 138 KB | QR-сканер |
| `libzstd.so` | 65 KB | Zstandard сжатие |
| `libnative-filters.so` | 24 KB | Fresco-фильтры |
| `libimage_processing_util_jni.so` | 33 KB | CameraX util |
| `libimagepipeline.so` | 9 KB | Fresco pipeline |
| `libsurface_util_jni.so` | 5 KB | CameraX util |
| `libc++_shared.so` | 1.3 MB | runtime C++ |

NDK r27b, ABI arm64. `extractNativeLibs="false"` — мапятся прямо из APK.

## Динамическая загрузка/обновление

`<meta-data android:name="com.facebook.soloader.enabled" android:value="false"/>` — **Facebook SoLoader выключен**. То есть динамической подгрузки .so из data-каталога/из сети **не предусмотрено** этим путём (классический способ был бы качать `.so` с сервера и грузить через SoLoader).

Точки `System.loadLibrary` найдены в:

- `ru/ok/tracer/nativebridge/NativeBridgeInstaller` — `tracernative`
- `ru/ok/tracer/minidump/Minidump` — `tracernative`
- `one/me/sdk/zsrd/ZstdUtil` — `zstd`
- `one/me/rlottie/NativeLibraryLoader` — `jlottie`
- `one/me/sdk/uikit/qr/QrCodeGenerator` — `qrcode`
- `org/webrtc/NativeLibrary` — `jingle_peerconnection_so`
- `defpackage/gbi.java`, `rrb.java`, `gq6.java`, `p0l.java`, `dq0.java`, `bj6.java` — обёртки конкретных декодеров
- `io/antmedia/rtmp_client/RtmpClient` — RTMP клиент **(но самой `librtmp-jni.so` в APK нет!)** — вызов `System.loadLibrary("rtmp-jni")` упадёт. Это либо мёртвый код, либо предполагается, что либу подгрузят отдельно. Скорее первое.
- `net/jpountz/lz4/LZ4JNI` — LZ4, в state «if java fallback fails, load native»
- `androidx/camera/core/ImageProcessingUtil`, `SurfaceUtil` — стандартные

Скачивания `.so` с сервера в коде **не нашёл** (нет URL `*.so` нигде в `https://`-строках).

## Apptracer minidump path

В `Minidump.java`:

```java
public Minidump() { System.loadLibrary("tracernative"); }
private native void installMinidumpWriterImpl(String str);
private native void uninstallMinidumpWriterImpl();
public final void a(String str) {
    synchronized (this.a) {
        if (this.b) uninstallMinidumpWriterImpl();
        installMinidumpWriterImpl(str);
        this.b = true;
    }
}
```

Это значит: при инициализации Apptracer-а нативный writer регистрируется как обработчик SIGSEGV/SIGABRT. При крэше пишет .dmp в каталог, путь которого передаётся аргументом. После — он же грузится через `SampleUploadWorker` на `https://sdk-api.apptracer.ru/api/crash/...`.

`HeapDumpInitializer` (декларирован в манифесте) — отдельная штука: периодический snapshot Java-heap (там в памяти лежат и сообщения, и токен сессии, и контакты).

## Что важно

- В клиенте нет встроенного «runtime self-update» через .so c сервера (SoLoader выключен, ссылок на `.so` в http нет). Это плюс с точки зрения бэкдор-параноидальной модели.
- Зато штатно вкомпилен **handler крэшей с уходом минидампа на `sdk-api.apptracer.ru`** + heap-dumps по серверной команде. То есть «сделай дамп процесса и пришли» — это штатно.
- Старый FFmpeg n4.4.3 — известный набор CVE; используется для перекодирования получаемого/отправляемого медиа. С точки зрения «странного поведения в продукте 2026 года» — компонент с давно исправленными в апстриме уязвимостями.
- Стек звонков — стандартный WebRTC, плюс отдельная **VK-овская enhancement-либа** в начале аудиопайплайна. Звук попадает к VK-коду до энкодера WebRTC. Сам по себе это не криминал — у всех мессенджеров кастомный noise-suppression — но это часть, которая полностью VK-вая.
