---
tags: [calls, debug, dump, surveillance, dev-in-prod]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/dev/DebugManager.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/dev/MediaDumpManager.java
related:
  - "[[61-leakcanary-gost-debug-flags]]"
  - "[[179-conversation-interface]]"
  - "[[135-calls-sdk-upload-config]]"
---

# DebugManager + MediaDumpManager — дамп медиа в звонках

Оба интерфейса доступны через `Conversation` в production-сборке.

## DebugManager

| Метод | Что |
|---|---|
| `enableFullAudioDump(dumpPath)` | **полный дамп аудио** в файл |
| `getMediaDumpManager()` | получить MediaDumpManager |
| `getUnderlyingCall()` | доступ к нижнему уровню звонка |
| `registerStatListener(listener, period, unit)` | периодическая статистика |
| `setVideoSettingsOverride(override)` | переопределить настройки видео |
| `updateCameraToggles(updater)` | обновить переключатели камеры |
| `reportError(error)` | отправить ошибку |

`VideoSettingsOverride`: `isMaxDimensionOverrideEnabled` / `maxDimension` — принудительное ограничение разрешения видео.

## MediaDumpManager

| Метод | Что |
|---|---|
| `recordAudioDump(durationSec, sources, listener)` | **записать аудио-дамп** |
| `requestMediaDump(durationSec, audio, video, listener)` | **запросить медиа-дамп** (аудио + видео) |
| `setLocalAudioDumpLocation(path)` | путь для локального дампа |
| `cancelAudioDumpRecord()` | отменить запись |

### Source — точки перехвата аудио

| Константа | Что |
|---|---|
| `IN_ENTER_PROCESSING` | вход до обработки |
| `IN_AFTER_NS` | после шумоподавления |
| `IN_AFTER_ANIMOJI` | после анимодзи |
| `IN_EXIT_PROCESSING` | выход после обработки |
| `OUT_ENTER_PROCESSING` | исходящий до обработки |
| `OUT_EXIT_PROCESSING` | исходящий после обработки |

## Что важно

1. **`requestMediaDump(durationSec, audio=true, video=true, listener)`** — запрос дампа аудио И видео на заданную длительность. Это `RemoteMediaDumpRequestListener` — дамп может быть запрошен **удалённо** (сервером).

2. **6 точек перехвата аудио** — до/после NS, до/после анимодзи, до/после кодека. Полный pipeline аудио-обработки доступен для дампа.

3. **`enableFullAudioDump(dumpPath)`** — полный дамп всего аудио в файл. В production-сборке.

4. **`getUnderlyingCall()`** — прямой доступ к нижнему уровню звонка (обфусцированный `ra1`).

## Сводка

`DebugManager`/`MediaDumpManager` в production. `requestMediaDump` — удалённый запрос дампа аудио+видео. 6 точек перехвата аудио-pipeline. `enableFullAudioDump` — полный дамп в файл.
