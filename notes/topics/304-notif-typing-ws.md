---
tags: [protocol, ws, notif-typing, typing-event, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/tef.java
  - work/jadx_base/sources/defpackage/mef.java
related:
  - "[[20-ws-protocol-opcodes]]"
  - "[[303-update-message-event]]"
---

# NOTIF_TYPING WS — уведомление о наборе текста

`NOTIF_TYPING` (опкод 110) — уведомление о том, что пользователь набирает сообщение.

## OnTyping структура

| Поле | Что |
|---|---|
| `type` | **тип набора** |
| `isTyping` | **набирает ли** |

## Типы набора (mef)

| Тип | Что |
|---|---|
| `VIDEO_MSG` | запись видео-сообщения |
| `AUDIO_MSG` | запись аудио-сообщения |

## Что важно

1. **`type`** — сервер знает не только то, что пользователь набирает текст, но и то, что он записывает аудио или видео-сообщение.

2. **`VIDEO_MSG`/`AUDIO_MSG`** — отдельные типы для записи медиа-сообщений. Это означает, что сервер уведомляет других участников о записи аудио/видео.

3. `isTypingSendEnabled` (PmsKey) — сервер может отключить отправку typing-уведомлений.

## Сводка

`NOTIF_TYPING`: type(VIDEO_MSG/AUDIO_MSG)/isTyping. Сервер знает, что именно записывает пользователь.
