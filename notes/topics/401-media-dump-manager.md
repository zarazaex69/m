---
tags: [media-dump, debug, audio-dump, video-dump, surveillance, server-control, calls]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/dev/MediaDumpManager.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/dev/internal/MediaDumpManagerImpl.java
related:
  - "[[101-calls-sdk-managers]]"
  - "[[397-video-sdk-screen-camera]]"
  - "[[394-audio-sdk-kws-mic-proximity]]"
---

# MediaDumpManager — дамп медиа в звонках

`MediaDumpManager` — управление дампом аудио/видео в звонках. Присутствует в **production** сборке.

## Методы

| Метод | Что |
|---|---|
| `requestMediaDump(durationSeconds, audio, video, listener)` | **запросить дамп медиа** (через signaling) |
| `recordAudioDump(durationSeconds, sources, listener)` | **записать локальный аудио-дамп** |
| `cancelAudioDumpRecord()` | отменить запись |
| `setLocalAudioDumpLocation(path)` | установить путь для дампа |

## Источники аудио (Source)

| Источник | Файл | Что |
|---|---|---|
| `IN_ENTER_PROCESSING` | `in_enter_processing` | вход до обработки |
| `IN_AFTER_NS` | `in_after_ns` | вход после NS |
| `IN_AFTER_ANIMOJI` | `in_after_animoji` | вход после анимоджи |
| `IN_EXIT_PROCESSING` | `in_exit_processing` | вход после всей обработки |
| `OUT_ENTER_PROCESSING` | `out_enter_processing` | выход до обработки |
| `OUT_EXIT_PROCESSING` | `out_exit_processing` | выход после обработки |

## requestMediaDump — команда через signaling

```json
{
  "audio": true/false,
  "video": true/false,
  "duration": N
}
```

Команда: `collect-debug-dump`. Ответ: `{type: "response", response: "collect-debug-dump"}`.

## recordAudioDump — локальная запись

Файл: `calldump_YYYY-MM-DD HH:mm:ss/` в `dumpLocation`. Использует нативный WebRTC `DumpCallback`.

## Что важно

1. **`requestMediaDump` в production** — команда для дампа аудио+видео доступна в production сборке. Сервер может запросить дамп через signaling.

2. **6 источников аудио** — дамп захватывает аудио на разных стадиях обработки (до/после NS, до/после анимоджи).

3. **`collect-debug-dump`** — команда через signaling. Сервер инициирует дамп.

4. **`durationSeconds`** — длительность дампа задаётся сервером.

## Сводка

`MediaDumpManager.requestMediaDump(duration, audio, video)` → signaling `collect-debug-dump {audio, video, duration}`. 6 источников аудио. В production сборке.
