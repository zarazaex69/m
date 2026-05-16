---
tags: [protocol, tasks, video-convert, video-upload, warm-chat-history]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Tasks.java
related:
  - "[[205-tasks-proto]]"
  - "[[276-tasks-fire-time-photo]]"
---

# VideoConvert + VideoUpload + WarmChatHistory Tasks

## VideoConvert

| Поле | Что |
|---|---|
| `srcPath` / `dstPath` | **пути к файлам** |
| `startPosition` / `endPosition` | **позиции обрезки** |
| `quality` | качество (bitrate/height/width/isOriginal) |
| `messageId` | ID сообщения |
| `attachLocalId` | локальный ID вложения |
| `requestId` | ID запроса |

## VideoUpload

| Поле | Что |
|---|---|
| `file` | **путь к файлу** |
| `audio` | **есть ли аудио** |
| `messageId` | ID сообщения |
| `attachLocalId` | локальный ID вложения |
| `requestId` | ID запроса |

## WarmChatHistory

| Поле | Что |
|---|---|
| `chatIds[]` | **ID чатов для прогрева** |
| `lastFailTime` | время последней неудачи |
| `taskId` | ID задачи |

## Что важно

1. **`VideoConvert.srcPath`/`dstPath`** — локальные пути к файлам видео. Это внутренние пути на устройстве.

2. **`VideoConvert.startPosition`/`endPosition`** — позиции обрезки видео (float 0.0-1.0).

3. **`WarmChatHistory.chatIds[]`** — прогрев истории для нескольких чатов. Связано с `chat-history-warm-opts` PmsKey.

## Сводка

`VideoConvert`: srcPath/dstPath/startPosition/endPosition/quality. `VideoUpload`: file/audio. `WarmChatHistory`: chatIds[].
