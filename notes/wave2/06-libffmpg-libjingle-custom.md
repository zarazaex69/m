---
tags: [native, webrtc, ffmpeg, vk-custom, ok-custom, surveillance, kws, asr, dump, encryption, wave2]
status: verified
severity: critical
sources:
  - findings/native/libffmpg.exports.txt
  - findings/native/libffmpg.strings.txt
  - findings/native/libffmpg.proof.txt
  - findings/native/libffmpg.cve-crossref.md
  - findings/native/libffmpg.registered-codec-names.txt
  - findings/native/libjingle_peerconnection_so.exports.txt
  - findings/native/libjingle_peerconnection_so.strings.txt
  - native_diff/old_vk_full.txt
  - native_diff/new_vk_full.txt
related:
  - "[[540-webrtc-encryption-disable-stun-marking-vpn-bypass]]"
  - "[[15-on-device-asr-kws-diarization]]"
  - "[[16-server-pushed-ml-models-in-calls]]"
  - "[[43-libjingle-webrtc-custom-build]]"
  - "[[09-native-libs]]"
---

# 06. Глубокий реверс libffmpg.so и libjingle_peerconnection_so.so: собственные расширения VK/OK

## TL;DR

**libffmpg.so** — чистый FFmpeg n4.4.3 (GPL v3) + libyuv + libvpx v1.14.0 с единственным кастомным JNI-слоем `one_me_sdk_media_ffmpeg_AnimatedFileDrawable` (7 функций). Конфигурация сборки **намеренно стёрта** (заменена пробелом). Собственных VK/OK-расширений в кодеках нет. Строка `VKSN` — единственный VK-маркер.

**libjingle_peerconnection_so.so** — **глубоко модифицированный** форк WebRTC с 7+ собственными модулями VK/OK, не существующими в upstream:

| # | Расширение | Тип | Upstream? |
|---|-----------|-----|-----------|
| 1 | `enhancer_ns/` — полный DSP-пайплайн (ASR, KWS, Animoji, Diarization) | Модуль audio_processing | ❌ |
| 2 | `nativeSubmitDumpRequest` / `nativeClearDumpRequests` — серверный дамп аудио/видео | JNI API | ❌ |
| 3 | `nativeSetKeywordSpotterParams` — KWS в реальном времени | JNI API | ❌ |
| 4 | `nativeSetAnimojiParams` — Wav2Lip анимация | JNI API | ❌ |
| 5 | `UnencryptedRtpTransport` + `getDisableEncryption` | Транспорт | ❌ |
| 6 | `WebRTC-OK-StunCustomAttr` / `WebRTC-OK-TurnChannelDataMark` | Field trials | ❌ |
| 7 | `VpnPreference` enum (NEVER_USE_VPN) | ICE config | ❌ |
| 8 | `nativeDeviceDataIsRecorded` / `nativeDeviceCacheDirectBufferAddress` — дублированный аудио-вход | JNI API | ❌ |
| 9 | `VpxDecoderWrapper` / `VpxEncoderWrapper` — обёртки VP8/VP9 | JNI API | ❌ |
| 10 | `WebRTC-OK-FrameDropper-Alt` — альтернативный frame dropper | Field trial | ❌ |
| 11 | `WebRTC-VK-OpusMaxPlcDurationMs` — кастомный PLC | Field trial | ❌ |
| 12 | `ru.ok.android.util.compressor.LZ4` — OK-утилита внутри WebRTC .so | JNI API | ❌ |

---

## 1. libffmpg.so — анализ

### 1.1 Состав (2987 экспортов)

| Категория | Кол-во | Примечание |
|-----------|--------|------------|
| libyuv (цветовые конверсии, масштабирование) | ~2300 | Стандартный Google libyuv |
| FFmpeg (av_*, avio_*, avpriv_*, ff_*, ffurl_*) | ~600 | FFmpeg n4.4.3 |
| libvpx (vpx_*) | ~50 | v1.14.0 |
| C++ runtime (__cxa_*, _Unwind_*) | ~30 | Статически слинкован |
| **Кастомные JNI (one_me_sdk)** | **7** | Единственное расширение |

### 1.2 Кастомные экспорты (non-upstream)

```
Java_one_me_sdk_media_ffmpeg_AnimatedFileDrawable_createDecoder
Java_one_me_sdk_media_ffmpeg_AnimatedFileDrawable_destroyDecoder
Java_one_me_sdk_media_ffmpeg_AnimatedFileDrawable_getFrameAtTime
Java_one_me_sdk_media_ffmpeg_AnimatedFileDrawable_getVideoFrame
Java_one_me_sdk_media_ffmpeg_AnimatedFileDrawable_prepareToSeek
Java_one_me_sdk_media_ffmpeg_AnimatedFileDrawable_seekToMs
Java_one_me_sdk_media_ffmpeg_AnimatedFileDrawable_stopDecoder
```

**Назначение**: JNI-обёртка для декодирования анимированных файлов (GIF/APNG/WebP-анимация) через FFmpeg. Пакет `one.me.sdk.media.ffmpeg` — собственный SDK MAX/OneME. Функционально безобидно — стандартный паттерн для Telegram-подобных мессенджеров.

### 1.3 Маркер `VKSN`

Единственная VK-строка в libffmpg.strings.txt (строка 8). Вероятно — magic bytes или идентификатор формата контейнера VK для стриминга (VK Stream Notation?). Требует дальнейшего исследования в контексте видео-сообщений.

### 1.4 Скрытая конфигурация

Из `libffmpg.proof.txt`:
- `avcodec_configuration()` возвращает **один пробел** вместо полной строки `./configure --enable-...`
- Это **намеренная** модификация `libavutil/utils.c` для сокрытия флагов сборки
- Лицензия: GPL v3 (не LGPL) → включены проприетарные кодеки

### 1.5 Вывод по libffmpg.so

**Нет собственных кодеков, фильтров, watermark-функций или surveillance-расширений.** Библиотека — стандартная сборка FFmpeg с тонким JNI-слоем для анимации. Основной риск — 3.5 года без security-патчей (CVE-2024-7055, CVE-2023-47470 и др.).

---

## 2. libjingle_peerconnection_so.so — анализ

### 2.1 Состав (227 экспортов)

| Категория | Кол-во | Примечание |
|-----------|--------|------------|
| Стандартный WebRTC JNI (org.webrtc.*) | 180 | PeerConnection, MediaStream, etc. |
| **VK/OK кастомные JNI** | **34** | Ниже |
| JNI_OnLoad | 1 | |
| Прочие | 12 | |

### 2.2 Собственные (non-upstream) экспорты

#### A. Серверный дамп медиа-потока (КРИТИЧНО)

```c
Java_org_webrtc_PeerConnectionFactory_nativeSubmitDumpRequest   // 0x9c7104
Java_org_webrtc_PeerConnectionFactory_nativeClearDumpRequests   // 0x9c5918
```

Строки подтверждают полноценную систему дампа:
- `"DUMP_REQUESTED"` — состояние/сигнал
- `"opening dump file:"` — открытие файла записи
- `"Dump bitrate is about to start. The file is open:"` — запись битрейта
- `"Dump bitrate flushed"` / `"Dump bitrate renamed to"` — ротация файлов
- `"dumping for"` — активная запись
- `"capture is null, ignoring dumping config"` — проверка capture-потока
- `"capture is null, ignoring clear dumping"` — очистка
- `"../../modules/audio_processing/enhancer_ns/dumper.h"` — **собственный** модуль dumper

**Импликация**: сервер может в любой момент инициировать запись аудио/видео звонка на устройстве через `nativeSubmitDumpRequest`. Это **не** стандартный WebRTC AecDump — это отдельная система с собственным `dumper.h`.

#### B. Keyword Spotter в реальном времени (КРИТИЧНО)

```c
Java_org_webrtc_PeerConnectionFactory_nativeSetKeywordSpotterParams  // 0x9c6920
```

Строки:
- `"SetKeywordSpotterParams called"`
- `"keyword spotter instantiation failed with (runtime error)"`
- `"keyword spotter instantiation failed with (exception)"`
- `"keyword spotter instantiation failed by unknown reason"`
- `"keyword spotter muted:"`
- `"../../modules/audio_processing/enhancer_ns/kws_impl.cc"` — **собственный** исходник

KWS работает **внутри** WebRTC audio processing pipeline, анализируя аудио-поток звонка в реальном времени. Параметры (модель, ключевые слова) задаются сервером.

#### C. Полный DSP-пайплайн `enhancer_ns/` (КРИТИЧНО)

Собственный модуль `modules/audio_processing/enhancer_ns/` с файлами:
- `enhancer_ns_impl.h` — основная реализация
- `animoji_impl.cc` / `animoji_impl.h` — Wav2Lip
- `kws_impl.cc` — Keyword Spotter
- `dumper.h` — дамп аудио

Строки:
- `"NsEnhancer 1s avg"` / `"NsEnhancer 10s avg"` / `"NsEnhancer last run"` — метрики
- `"SetEnhancerParams: pipeline algorithm created"` — пайплайн
- `"SetEnhancerParams: baseline algorithm created"`
- `"SetEnhancerParams: TFLite was not yet inited, postponing configuration"`
- `"in_after_animoji"` — точка в пайплайне после animoji-обработки
- `"RenderPreProcessor;"` — пре-процессор рендера

**Архитектура**: `enhancer_ns` — это **надстройка** над стандартным WebRTC Noise Suppression, которая добавляет ASR/KWS/Animoji/Diarization прямо в audio processing chain.

#### D. Animoji / Wav2Lip

```c
Java_org_webrtc_PeerConnectionFactory_nativeSetAnimojiParams  // 0x9c675c
```

Строки:
- `"SetAnimojiParams called"`
- `"no animoji data source"`
- `"no animoji data source or preprocessor"`
- `"capture is null, ignoring animoji params"`
- `"animoji-ds-thread"` — отдельный поток
- `"animoji data source:"`

#### E. Preprocessor (серверно-конфигурируемый)

```c
Java_org_webrtc_PeerConnectionFactory_nativeSetPreprocessorParams  // 0x9c6b50
Java_org_webrtc_PeerConnectionFactory_nativeSetTFLiteLibraryPath   // 0x9c6dd8
```

- `"setpreprocessorparams called"`
- Путь к TFLite задаётся сервером → модели загружаются динамически

#### F. Дублированный аудио-вход (Device*)

```c
Java_org_webrtc_audio_WebRtcAudioRecord_nativeDeviceCacheDirectBufferAddress  // 0x9b5fc4
Java_org_webrtc_audio_WebRtcAudioRecord_nativeDeviceDataIsRecorded            // 0x9b60b4
```

Строки:
- `"OnDeviceDataIsRecorded audio_record_jni audio_device"`
- `"OnDeviceCacheDirectBufferAddress"`

В upstream WebRTC есть только `nativeCacheDirectBufferAddress` и `nativeDataIsRecorded`. Дублирование с префиксом `Device` указывает на **второй параллельный аудио-вход** — возможно, для записи с другого микрофона или для отдельного канала обработки (KWS/ASR получает свою копию аудио).

#### G. VpxDecoderWrapper / VpxEncoderWrapper

```c
Java_org_webrtc_VpxDecoderWrapper_nativeCreate       // 0x9a901c
Java_org_webrtc_VpxDecoderWrapper_nativeDecode       // 0x9a9054
Java_org_webrtc_VpxDecoderWrapper_nativeGetDecodedImage
Java_org_webrtc_VpxDecoderWrapper_nativeInit
Java_org_webrtc_VpxDecoderWrapper_nativeRelease
Java_org_webrtc_VpxDecoderWrapper_nativeSetData
Java_org_webrtc_VpxEncoderWrapper_nativeCreate       // 0x9a9300
Java_org_webrtc_VpxEncoderWrapper_nativeEncode
Java_org_webrtc_VpxEncoderWrapper_nativeInit
Java_org_webrtc_VpxEncoderWrapper_nativeRelease
Java_org_webrtc_VpxEncoderWrapper_nativeSetCallback
```

Не существуют в upstream WebRTC. Это **собственные обёртки** для прямого доступа к VP8/VP9 кодекам из Java, минуя стандартный WebRTC encoder/decoder pipeline. Позволяют:
- Кодировать/декодировать видео вне PeerConnection
- Использовать для server-side recording / transcoding

#### H. BreakpadBridge (OK-специфичный)

```c
Java_org_webrtc_BreakpadBridge_nativeCrash           // 0x9a9a64
Java_org_webrtc_BreakpadBridge_nativeInitBreakpad    // 0x9a9a94
```

Google Breakpad интегрирован напрямую в WebRTC .so (в upstream это отдельная библиотека). `nativeCrash` — принудительный краш для тестирования.

#### I. one_video_calls_audio (Opus файловый I/O)

```c
Java_one_video_calls_audio_opus_FileReader_nativeAudioOpenOpusFile
Java_one_video_calls_audio_opus_FileReader_nativeAudioReadOpusFile
Java_one_video_calls_audio_opus_FileReader_nativeAudioSeekOpusFile
Java_one_video_calls_audio_opus_FileReader_nativeAudioGetTotalPcmDuration
Java_one_video_calls_audio_opus_FileWriter_nativeAudioStartRecord
Java_one_video_calls_audio_opus_FileWriter_nativeAudioWriteFrame
Java_one_video_calls_audio_Utils_nativeAudioGetWaveform
```

Пакет `one.video.calls.audio.opus` — собственный SDK для записи/воспроизведения Opus-файлов. `FileWriter.nativeAudioStartRecord` + `nativeAudioWriteFrame` — **запись аудио звонка в файл** на устройстве.

#### J. ru.ok.android.util.compressor.LZ4

```c
Java_ru_ok_android_util_compressor_LZ4_nativeCompress
Java_ru_ok_android_util_compressor_LZ4_nativeDecompress
Java_ru_ok_android_util_compressor_LZ4_nativeDecompressPartial
```

OK-утилита для LZ4-сжатия, встроенная прямо в WebRTC .so. Используется для сжатия дампов/логов перед отправкой.

### 2.3 Кастомные Field Trials (не в upstream WebRTC)

| Field Trial | Назначение |
|-------------|-----------|
| `WebRTC-OK-StunCustomAttr` | Внедрение серверного маркера в STUN-пакеты для DPI |
| `WebRTC-OK-TurnChannelDataMark` | Маркировка TURN Channel Data для DPI |
| `WebRTC-OK-FrameDropper-Alt` | Альтернативный алгоритм сброса кадров |
| `WebRTC-VK-OpusMaxPlcDurationMs` | Кастомный PLC (Packet Loss Concealment) для Opus |
| `WebRTC-EncoderDataDumpDirectory` | Директория для дампа закодированных данных |
| `WebRTC-Debugging-RtpDump` | Дамп RTP-пакетов |

### 2.4 googAudioMirroring

Строка `googAudioMirroring` (line 25012) — constraint для аудио-источника. В upstream WebRTC это deprecated constraint, но его наличие подтверждает возможность **зеркалирования** аудио-потока (отправка копии аудио на другой приёмник).

---

## 3. Diff 26.15.3 → 26.16.0 (native_diff/)

### Удалено в 26.16.0 (libEnhancementLibShared.so / vk::enh):

```
vk::enh::BCResNetExternalStateKWS          — весь класс (9 методов)
vk::enh::BCResNetKWS                       — весь класс (9 методов)
vk::enh::buildMelBasis                     — построение мел-базиса для KWS
vk::enh::FeatureExtractor                  — весь класс (6 методов)
```

**Итого удалено: 25 функций** — все связаны с on-device KWS (Keyword Spotting).

Это подтверждает заявление о «удалении KWS» в 26.16.0. Однако:
- `vk::enh::KWSFactory::createAlgorithm` — **осталась** в 26.16.0
- `vk::enh::ASRFactory` / `vk::enh::AsrService` — **без изменений**
- `vk::enh::DiarizationEngineFactory` — **без изменений**
- `vk::enh::ProfanityFactory` — **без изменений**

**Вывод**: удалены только конкретные реализации BCResNet-моделей KWS и их feature extractor. Фабрика KWS и весь остальной surveillance-стек (ASR, Diarization, Profanity filter) **на месте**. В libjingle `nativeSetKeywordSpotterParams` тоже **не удалён**.

---

## 4. Поиск tap/sniff/intercept/duplicate/mirror/echo

| Паттерн | Результат |
|---------|-----------|
| `intercept` | ❌ Не найдено |
| `sniff` | ❌ Не найдено |
| `tap` (аудио/видео контекст) | Только `postfilter_tapset` (Opus codec, стандартный) |
| `mirror` | `googAudioMirroring` — зеркалирование аудио-потока |
| `duplicate` | Только стандартные ошибки (duplicate SSRC, duplicate codec) |
| `echo` | Только стандартный AEC (Echo Canceller) |

**Ключевая находка**: `googAudioMirroring` — механизм дублирования аудио-потока. В комбинации с `nativeSubmitDumpRequest` и `nativeDeviceDataIsRecorded` это создаёт полную цепочку для скрытой записи.

---

## 5. Итоговая оценка

### Топ-7 собственных (non-upstream) функций с наибольшим surveillance-потенциалом:

1. **`nativeSubmitDumpRequest`** — серверно-инициируемый дамп аудио/видео звонка. Собственный `dumper.h` в `enhancer_ns/`. Сервер может начать запись в любой момент.

2. **`nativeSetKeywordSpotterParams`** — настройка KWS прямо в audio processing pipeline. Сервер задаёт модель и ключевые слова. Работает в реальном времени во время звонка.

3. **`nativeDeviceDataIsRecorded`** — дублированный аудио-вход, не существующий в upstream. Второй канал записи параллельно основному.

4. **`getDisableEncryption` → `UnencryptedRtpTransport`** — серверное отключение шифрования RTP. Медиа-поток становится plaintext.

5. **`WebRTC-OK-StunCustomAttr`** — внедрение серверного маркера в каждый STUN-пакет. Позволяет DPI/СОРМ идентифицировать конкретный звонок.

6. **`nativeSetPreprocessorParams` + `nativeSetTFLiteLibraryPath`** — сервер загружает произвольные ML-модели в audio pipeline. Модель может быть чем угодно: ASR, KWS, voice fingerprinting.

7. **`VpnPreference.NEVER_USE_VPN`** — принудительный обход VPN для медиа-трафика. Раскрывает реальный IP пользователя.

### Архитектурная схема surveillance-пайплайна:

```
Микрофон → WebRtcAudioRecord
              ├── nativeDataIsRecorded (основной путь)
              └── nativeDeviceDataIsRecorded (дублированный путь)
                     ↓
              enhancer_ns pipeline:
              ├── KWS (keyword detection) → событие на сервер
              ├── ASR (транскрипция) → текст на сервер  
              ├── Animoji (lip-sync)
              └── Dumper → файл на устройстве
                     ↓
              nativeSubmitDumpRequest (серверная команда)
                     ↓
              Opus FileWriter → запись в файл
                     ↓
              LZ4 compress → upload
```

---

## 6. Статус в 26.16.0

| Компонент | Статус |
|-----------|--------|
| `nativeSubmitDumpRequest` / `nativeClearDumpRequests` | ✅ На месте |
| `nativeSetKeywordSpotterParams` | ✅ На месте |
| `nativeDeviceDataIsRecorded` | ✅ На месте |
| `getDisableEncryption` / `UnencryptedRtpTransport` | ✅ На месте |
| `WebRTC-OK-StunCustomAttr` / `TurnChannelDataMark` | ✅ На месте |
| `VpnPreference` | ✅ На месте |
| `enhancer_ns/dumper.h` | ✅ На месте |
| BCResNetKWS / BCResNetExternalStateKWS (в libEnhancementLibShared) | ❌ Удалено |
| KWSFactory (в libEnhancementLibShared) | ✅ На месте |

**Вывод**: «удаление KWS» в 26.16.0 — косметическое. Удалены только конкретные модели нейросети. Вся инфраструктура для keyword spotting, дампа аудио и отключения шифрования **полностью сохранена** в production-бинарнике.
