---
tags: [protocol, audio, protos, schema, transcription, waveform]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Protos.java
related:
  - "[[206-protos-schemas]]"
  - "[[263-protos-video-schema]]"
---

# Protos.Audio — аудио-сообщения

`Protos.Audio` — структура аудио-вложения.

## Статусы транскрипции

`UNKNOWN`(0), `PROCESSING`(1), `SUCCESS`(2), `FAILED`(3), `NOT_SUPPORTED`(4), `MEDIA_NOT_READY`(5).

## Поля

| Поле | Что |
|---|---|
| `audioId` | ID аудио |
| `duration` | длительность |
| `startTime` | время начала воспроизведения |
| `lastStartTimeUpdateTimestamp` | время последнего обновления позиции |
| `transcriptionStatus` | **статус транскрипции** |
| `wave` | **waveform данные** (bytes) |

## Что важно

1. **`transcriptionStatus`** — 5 статусов транскрипции. Связано с `audio.transcription.enabled` (см. [[247-user-settings-full]]).

2. **`wave`** — waveform (форма волны) хранится как bytes. Это визуализация аудио.

3. **`startTime`/`lastStartTimeUpdateTimestamp`** — позиция воспроизведения синхронизируется. Сервер знает, где пользователь остановил воспроизведение.

## Сводка

`Protos.Audio`: audioId/duration/startTime/transcriptionStatus(UNKNOWN/PROCESSING/SUCCESS/FAILED/NOT_SUPPORTED/MEDIA_NOT_READY)/wave(waveform bytes).
