---
tags: [protocol, ws, transcription, audio, video, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/qvi.java
  - work/jadx_base/sources/defpackage/y0c.java
related:
  - "[[264-protos-audio-schema]]"
  - "[[263-protos-video-schema]]"
  - "[[296-ws-opcodes-final]]"
---

# TRANSCRIBE_MEDIA + NOTIF_TRANSCRIPTION WS

## TRANSCRIBE_MEDIA (опкод 157) — запрос транскрипции

| Поле | Что |
|---|---|
| `transcription` | **текст транскрипции** |
| `transcriptionStatus` | **статус транскрипции** |

## NOTIF_TRANSCRIPTION (опкод 158) — уведомление о транскрипции

| Поле | Что |
|---|---|
| `messageId` | **ID сообщения** |
| `chat_id` | ID чата |
| `transcription` | **текст транскрипции** |
| `mediaId` | **ID медиа** |
| `transcriptionStatus` | **статус транскрипции** |

## Что важно

1. **`NOTIF_TRANSCRIPTION.transcription`** — сервер отправляет текст транскрипции через WS. Это означает, что транскрипция происходит на сервере.

2. **`NOTIF_TRANSCRIPTION.messageId`/`mediaId`** — транскрипция привязана к конкретному сообщению и медиа.

3. Это подтверждение серверной транскрипции аудио/видео сообщений. Сервер получает аудио/видео и возвращает текст.

## Сводка

`TRANSCRIBE_MEDIA`: transcription/transcriptionStatus. `NOTIF_TRANSCRIPTION`: messageId/chat_id/transcription/mediaId/transcriptionStatus. Серверная транскрипция аудио/видео.
