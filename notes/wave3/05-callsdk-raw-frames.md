---
tags: [wave3, calls-sdk, audio-dump, video-sink, webrtc, raw-frames, parallel-sink]
status: confirmed
severity: high
sources: [jadx, rz-bin, strings]
related: [[15-on-device-asr-kws-diarization]], [[16-server-pushed-ml-models-in-calls]], [[17-apptracer-uplink]]
---

# Raw Audio/Video Frame Interception в кастомном WebRTC (libjingle_peerconnection_so)

## TL;DR

В кастомном WebRTC от VK/OK обнаружены **три параллельных механизма перехвата сырых аудио-фреймов** и **один механизм для видео**, все production-ready:

1. **`nativeSubmitDumpRequest`** — кастомный JNI-метод (отсутствует в upstream WebRTC), дампит raw PCM аудио из 6 точек audio pipeline в файлы на диск. Длительность `Integer.MAX_VALUE` (бесконечная запись). Может быть вызван удалённо через signaling-команду `collect-debug-dump`.
2. **`AudioRecordSampleHook`** — Java-side параллельный sink на raw PCM ДО передачи в native WebRTC. Получает полный `byte[]` каждого 10ms-буфера с микрофона. Распределяет на `CopyOnWriteArraySet<dyk>` подписчиков.
3. **`nativeDeviceDataIsRecorded`** — второй параллельный аудио-поток (device audio capture через `AudioPlaybackCaptureConfiguration` + `MediaProjection`), записывающий звук ДРУГИХ приложений на устройстве.
4. **`WebRTC-EncoderDataDumpDirectory` / `WebRTC-DecoderDataDumpDirectory`** — field trials для дампа закодированного видео/аудио в файлы. Значение field trials задаётся сервером.

## Находка 1: nativeSubmitDumpRequest — кастомный audio pipeline dumper

### Что это

Метод `PeerConnectionFactory.nativeSubmitDumpRequest(long factory, String path, int durationMs, int[] sources, DumpCallback callback)` — **не существует в upstream WebRTC**. Это кастомное расширение VK/OK.

### DumpSource — 6 точек перехвата

```java
public enum DumpSource {
    IN_ENTER_PROCESSING(10),   // вход в audio processing (raw mic)
    IN_AFTER_NS(20),           // после noise suppression
    IN_AFTER_ANIMOJI(21),      // после animoji processing
    IN_EXIT_PROCESSING(30),    // выход из processing
    OUT_ENTER_PROCESSING(1000), // входящий аудио (от собеседника)
    OUT_EXIT_PROCESSING(1010);  // выходящий после обработки
}
```

Каждая точка пишет raw PCM в отдельный файл: `in_enter_processing`, `in_after_ns`, `in_after_animoji`, `in_exit_processing`, `out_enter_processing`, `out_exit_processing`.

### Как вызывается

1. **Локально** через `DebugManagerImpl.enableFullAudioDump(path)` → `submitDumpRequest(path, Integer.MAX_VALUE, null)` — бесконечная запись ВСЕХ точек.
2. **Через MediaDumpManager** с выбором точек и длительности.
3. **Удалённо** через signaling-команду `collect-debug-dump` (JSON: `{audio: bool, video: bool, duration: int}`). Ответ приходит через signaling callback.

### Native реализация

В `libjingle_peerconnection_so.so` подтверждено:
- Экспорт: `Java_org_webrtc_PeerConnectionFactory_nativeSubmitDumpRequest` @ 0x009b...
- Экспорт: `Java_org_webrtc_PeerConnectionFactory_nativeClearDumpRequests`
- Строки: `dump_file->is_open()`, `dump_file_.is_open()` — файловый I/O
- Путь: `../../modules/audio_processing/enhancer_ns/dumper.h` — кастомный модуль

### DumpCallback — JNI callback в Java

```java
public class NativeDumpCallback implements DumpCallback {
    @CalledByNative
    public void onComplete(String dumpFolderPath) { ... }
    @CalledByNative
    public void onStarted(String dumpFolderPath) { ... }
}
```

Native код вызывает Java через JNI когда запись начата/завершена.

## Находка 2: AudioRecordSampleHook — параллельный Java-side audio sink

### Архитектура

```
Microphone → AudioRecord.read() → byteBuffer
                                      ↓
                          ┌───────────┼───────────────┐
                          ↓           ↓               ↓
              audioRecordSampleHook  audioSamplesReady  nativeDataIsRecorded
              (aeb → CopyOnWriteArraySet<dyk>)         (→ native WebRTC)
```

В `WebRtcAudioRecord.AudioRecordThread.trySendAudioSamples()`:
1. Сначала вызывается `audioRecordSampleHook.onWebRtcAudioRecordSamplesReady(format, channels, sampleRate, byteBuffer.array(), offset, bytesRead)` — **полный raw PCM буфер**
2. Затем `audioSamplesReadyCallback` — копия данных
3. Затем `nativeDataIsRecorded()` — передача в native WebRTC

### Подписчики (dyk → beb)

Класс `aeb` (реализует `AudioRecordSampleHook`) распределяет raw PCM на `CopyOnWriteArraySet<dyk>`:
- `isk` — мониторинг уровня громкости (логирует в "SharedPeerConnectionFac")
- `AudioSampleEnergyCalculator` → `ConversationImpl` — определение "кто говорит"
- `AudioLevelListener` — уровень аудио

Подписчики добавляются динамически через `((CopyOnWriteArraySet) aebVar.b).add(new dyk(interval, listener))` — с настраиваемым интервалом семплирования.

### Критично

Подписчик `dyk` имеет поле `long b` (интервал) и `long c` (следующий вызов). При `b=0` получает КАЖДЫЙ 10ms-буфер. Любой код в приложении может зарегистрировать listener и получать raw PCM с микрофона.

## Находка 3: Device Audio Capture — запись звука других приложений

### Что это

`WebRtcAudioRecord.initDeviceAudioRecord(MediaProjection)` создаёт ВТОРОЙ `AudioRecord` через `AudioPlaybackCaptureConfiguration` (API 29+), который записывает аудио-выход ДРУГИХ приложений (usage: MEDIA + GAME).

### Параллельный поток

В `AudioRecordThread.run()`:
```java
while (keepAlive) {
    trySendDeviceAudioSamples(audioTimestamp2);  // ← device audio (другие приложения)
    trySendAudioSamples(audioTimestamp);          // ← voice audio (микрофон)
}
```

Оба потока работают ПАРАЛЛЕЛЬНО. Device audio отправляется через `nativeDeviceDataIsRecorded()` в native.

### Как активируется

Через `JavaAudioDeviceModule.startDeviceAudioShare(MediaProjection)`. MediaProjection получается из `ScreenCapturerAndroid` (требует user consent через system dialog). Однако если MediaProjection уже получена для screen share, device audio capture запускается автоматически:

```java
if (this.mediaProjection != null && this.deviceAudioRecord == null) {
    initDeviceAudioRecord(this.mediaProjection);
}
```

## Находка 4: WebRTC-EncoderDataDumpDirectory — серверно-управляемый видео-дамп

### Field Trials

В strings обнаружены:
- `WebRTC-EncoderDataDumpDirectory` — путь для дампа закодированных видео-фреймов
- `WebRTC-DecoderDataDumpDirectory` — путь для дампа декодированных фреймов

Field trials задаются через:
1. `PeerConnectionFactory.initializeFieldTrials(String)` — при инициализации
2. `BaseCallParams.fieldTrials` — per-call, приходит с сервера
3. `ConversationFactoryInitParams.bonusFieldTrials` — дополнительные

**Значение field trials приходит с сервера** через параметры звонка. Сервер может включить дамп видео для конкретного пользователя.

## Находка 5: Два параллельных VideoSink на одном VideoTrack

В `zf5.java`:
```java
videoTrack2.addSink(x7dVar);  // sink 1: распределяет фреймы по участникам
videoTrack2.addSink(v7dVar);  // sink 2: отслеживает compactParticipantId
```

Это не covert recording — оба sink'а для UI (маршрутизация видео по участникам в групповом звонке). Но архитектура `IdentityHashMap<VideoSink, Long> sinks` в `VideoTrack` позволяет добавить произвольное количество sink'ов.

## Находка 6: ScreenCapturerAndroid — стандартный, но с нюансом

`ScreenCapturerAndroid` требует `Intent` от `MediaProjectionManager` (user consent). Однако:
- `MediaProjectionService` — foreground service, удерживает MediaProjection
- Полученная MediaProjection переиспользуется для device audio capture
- Нет проверки что пользователь согласился именно на audio capture (consent даётся на screen share)

## Находка 7: enhancer_ns/dumper.h — кастомный модуль в audio processing

Пути в strings подтверждают кастомный audio processing pipeline:
```
../../modules/audio_processing/enhancer_ns/animoji_impl.cc
../../modules/audio_processing/enhancer_ns/animoji_impl.h
../../modules/audio_processing/enhancer_ns/enhancer_ns_impl.h
../../modules/audio_processing/enhancer_ns/dumper.h          ← DUMPER
../../modules/audio_processing/enhancer_ns/kws_impl.cc       ← KWS
../../modules/utility/include/audio_dump_logger.h
```

`dumper.h` — выделенный модуль для записи аудио внутри native audio processing pipeline. Работает на уровне C++ без Java-обёрток.

## Находка 8: disableEncryption в PeerConnectionFactory.Options

```java
public static class Options {
    public boolean disableEncryption;  // ← SRTP encryption off
    @CalledByNative("Options")
    public boolean getDisableEncryption() { return this.disableEncryption; }
}
```

Поле `disableEncryption` отключает SRTP-шифрование медиа-потоков. Хотя в текущем коде не найдено явной установки `= true`, поле public и доступно через reflection или серверные field trials.

## Выводы

### Есть ли параллельный sink? — ДА, три штуки

1. **Audio**: `AudioRecordSampleHook` (aeb) получает raw PCM ПАРАЛЛЕЛЬНО с native WebRTC pipeline
2. **Audio**: `nativeDeviceDataIsRecorded` — параллельный поток device audio
3. **Audio**: `nativeSubmitDumpRequest` — дамп из 6 точек pipeline в файлы, вызываемый удалённо

### Серверный контроль

- `collect-debug-dump` через signaling — удалённый запуск дампа audio+video
- `fieldTrials` с сервера — включение `WebRTC-EncoderDataDumpDirectory`
- `DumpSource` позволяет выбрать точку перехвата (до/после обработки)

### Отличие от upstream WebRTC

Upstream WebRTC имеет только `startAecDump` (для отладки echo cancellation). MAX/VK добавили:
- `nativeSubmitDumpRequest` с 6 точками и callback
- `nativeClearDumpRequests`
- `enhancer_ns/dumper.h` модуль
- `audio_dump_logger.h`
- `DumpSource` / `DumpCallback` / `NativeDumpCallback` Java-классы
- `MediaDumpManager` с remote signaling trigger

Это **production-ready инфраструктура для серверно-управляемого перехвата raw audio** из любой точки processing pipeline.
