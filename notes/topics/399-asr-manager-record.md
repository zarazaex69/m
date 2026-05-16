---
tags: [asr, asr-record, calls, surveillance, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/asr/AsrManager.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/asr/AsrInfo.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/asr/listener/AsrRecordListener.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/asr/internal/commands/AsrCommandsExecutorImpl.java
related:
  - "[[376-asr-online-manager]]"
  - "[[377-record-manager]]"
  - "[[101-calls-sdk-managers]]"
---

# AsrManager — запись ASR (on-device)

`AsrManager` — управление записью ASR (on-device, отдельно от серверного `AsrOnlineManager`).

## Методы

| Метод | Что |
|---|---|
| `startRecord(fileName, sessionRoomId, onSuccess, onError)` | начать запись ASR |
| `stopRecord(sessionRoomId, onSuccess, onError)` | остановить запись ASR |
| `addAsrRecordListener(listener)` | подписаться на события |

## Команды (signaling)

| Команда | Параметры |
|---|---|
| `asr-start` | `{fileName}` |
| `asr-stop` | — |

## AsrInfo

| Поле | Что |
|---|---|
| `initiator` | **ConversationParticipant** — кто начал запись |
| `movieId` | ID видео (связь с RecordManager) |

## AsrRecordListener

| Callback | Что |
|---|---|
| `onAsrRecordStarted(AsrInfo)` | запись ASR началась |
| `onAsrRecordStopped()` | запись ASR остановлена |

## Что важно

1. **`asr-start {fileName}`** — команда через signaling. Имя файла передаётся серверу.

2. **`AsrInfo.movieId`** — ASR-запись связана с видеозаписью звонка (RecordManager).

3. **`AsrInfo.initiator`** — сервер знает, кто начал ASR-запись.

4. Это **отдельный** от `AsrOnlineManager` механизм. `AsrManager` — on-device запись, `AsrOnlineManager` — серверная транскрипция в реальном времени.

## Сводка

`AsrManager.startRecord(fileName, roomId)` → `asr-start {fileName}`. `AsrInfo(initiator, movieId)`. Отдельно от серверного `AsrOnlineManager`.
