---
tags: [debug, calls, audio-dump, video-settings, stat-listener, surveillance, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/dev/DebugManager.java
related:
  - "[[401-media-dump-manager]]"
  - "[[382-conversation-stats]]"
---

# DebugManager — отладочный менеджер звонков

`DebugManager` — отладочный менеджер в production сборке.

## Методы

| Метод | Что |
|---|---|
| `enableFullAudioDump(dumpPath)` | **включить полный дамп аудио** |
| `getMediaDumpManager()` | получить MediaDumpManager |
| `registerStatListener(listener, period, unit)` | **подписаться на статистику** с периодом |
| `removeStatListener(listener)` | отписаться |
| `reportError(error)` | сообщить об ошибке |
| `setVideoSettingsOverride(override)` | **переопределить настройки видео** |
| `updateCameraToggles(updater)` | обновить переключатели камеры |

## VideoSettingsOverride

| Поле | Что |
|---|---|
| `isMaxDimensionOverrideEnabled` | переопределение максимального разрешения |
| `maxDimension` | максимальное разрешение |

## Что важно

1. **`enableFullAudioDump`** — полный дамп аудио в production сборке.

2. **`registerStatListener(period, unit)`** — периодическая статистика звонка. Период задаётся произвольно.

3. **`setVideoSettingsOverride`** — переопределение настроек видео (разрешение).

4. **`updateCameraToggles`** — обновление переключателей камеры через callback.

## Сводка

`DebugManager`: `enableFullAudioDump(path)`, `registerStatListener(period)`, `setVideoSettingsOverride(maxDimension)`. В production сборке.
