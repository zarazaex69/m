---
tags: [calls, opus, audio-recording, native, file-write]
status: confirmed
sources:
  - work/jadx_base/sources/one/video/calls/audio/opus/FileWriter.java
  - work/jadx_base/sources/one/video/calls/audio/opus/OpusError.java
related:
  - "[[184-asr-manager]]"
  - "[[196-debug-media-dump-manager]]"
  - "[[135-calls-sdk-upload-config]]"
---

# Opus FileWriter — нативная запись аудио в файл

`one.video.calls.audio.opus.FileWriter` — нативный (JNI) класс для записи Opus-аудио в файл.

## Методы

| Метод | Что |
|---|---|
| `startRecord(path, sampleRate, channels)` | **начать запись** в файл |
| `nativeAudioWriteFrame(ByteBuffer, size)` | записать фрейм |
| `close()` | закрыть файл |

## Что важно

1. **`nativeAudioStartRecord(path, sampleRate, channels)`** — нативный метод. Открывает Opus-файл для записи по произвольному пути.

2. Используется в `AsrManager` (топик [[184-asr-manager]]) для записи аудио в файл перед отправкой на сервер.

3. Также используется в `MediaDumpManager` (топик [[196-debug-media-dump-manager]]) для дампа аудио-pipeline.

4. `@CalledByNative` — конструктор вызывается из нативного кода (JNI).

## Сводка

`FileWriter` — нативная запись Opus-аудио в файл. `startRecord(path, sampleRate, channels)` → `nativeAudioWriteFrame` → `close`. Используется в ASR и MediaDump.
