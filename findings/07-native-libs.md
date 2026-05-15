# Finding: Native Libraries (arm64-v8a)

## Библиотеки (по размеру)

| Файл | Размер | Назначение |
|------|--------|-----------|
| libjingle_peerconnection_so.so | 13 MB | WebRTC (Google's jingle) |
| libEnhancementLibShared.so | 5.5 MB | TensorFlow Lite (AI видео/аудио) |
| libffmpg.so | 1.9 MB | FFmpeg (медиа кодеки) |
| libc++_shared.so | 1.3 MB | C++ стандартная библиотека |
| libjlottie.so | 978 KB | Lottie анимации |
| libtracernative.so | 781 KB | AppTracer (нативный трейсер) |
| libnative-imagetranscoder.so | 572 KB | Транскодирование изображений |
| libstatic-webp.so | 491 KB | WebP декодер |
| libgifimage.so | 312 KB | GIF декодер |
| libgleff.so | 297 KB | OpenGL эффекты |
| libqrcode.so | 135 KB | QR-код сканер |
| libzstd.so | 64 KB | Zstandard сжатие |
| libimage_processing_util_jni.so | 32 KB | Обработка изображений |
| libnative-filters.so | 24 KB | Нативные фильтры |
| libimagepipeline.so | 8.6 KB | Fresco image pipeline |
| libsurface_util_jni.so | 4.8 KB | Surface utilities |

## Ключевые находки

### WebRTC (libjingle_peerconnection_so.so)
- Стандартный Google WebRTC (libwebrtc)
- STUN/TURN полная реализация (RFC 5389/5766)
- DTLS с Comodo сертификатами (CRL: crl.comodo.net, crl.comodoca.com)
- Поддержка: VP8, VP9, H.264, H.265, AV1
- RTP extensions: abs-send-time, transport-wide-cc, playout-delay, video-timing
- Нет hardcoded STUN/TURN серверов — всё динамически с бэкенда

### AI Enhancement (libEnhancementLibShared.so)
- TensorFlow Lite (NNAPI)
- Используется для: шумоподавление, улучшение видео, фоновое размытие
- Модели загружаются динамически

### AppTracer (libtracernative.so)
- Нативный компонент трейсера
- Функции: `tracer_report_nonfatal_from_here`, `tracer_set_api_endpoint`
- Endpoint настраивается динамически (sdk-api.apptracer.ru)

### FFmpeg (libffmpg.so)
- Кастомная сборка FFmpeg
- Для транскодирования видео/аудио

## Безопасность native libs
- Нет обфускации символов (можно читать имена функций)
- Нет anti-tampering в нативном коде
- Стандартные open-source библиотеки без модификаций
- Comodo CRL URLs захардкожены в WebRTC (можно использовать для fingerprinting)
