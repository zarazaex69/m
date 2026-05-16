---
tags: [protocol, tasks, video-play, chat-clear, chat-delete, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Tasks.java
related:
  - "[[205-tasks-proto]]"
  - "[[272-tasks-msg-delete]]"
---

# VideoPlay + ChatClear + ChatDelete Tasks

## VideoPlay

| Поле | Что |
|---|---|
| `videoId` | ID видео |
| `messageId` / `messageServerId` | ID сообщения |
| `chatServerId` | ID чата |
| `place` | **место воспроизведения** |
| `token` | токен |
| `attachLocalId` | локальный ID вложения |
| `saveToGallery` | **сохранить в галерею** |
| `startDownload` | начать загрузку |
| `requestId` | ID запроса |

## ChatClear

| Поле | Что |
|---|---|
| `chatId` / `chatServerId` | ID чата |
| `forAll` | **для всех** |
| `lastEventTime` | время последнего события |
| `requestId` | ID запроса |

## ChatDelete

| Поле | Что |
|---|---|
| `chatId` / `chatServerId` | ID чата |
| `forAll` | **для всех** |
| `lastEventTime` | время последнего события |
| `requestId` | ID запроса |

## Что важно

1. **`VideoPlay.place`** — сервер знает, откуда воспроизводится видео (чат, галерея, и т.д.).

2. **`VideoPlay.saveToGallery`** — сервер знает, сохранил ли пользователь видео в галерею.

3. **`ChatClear.forAll`/`ChatDelete.forAll`** — очистка/удаление чата для всех участников.

## Сводка

`VideoPlay`: videoId/place/saveToGallery/startDownload. `ChatClear`/`ChatDelete`: forAll/lastEventTime.
