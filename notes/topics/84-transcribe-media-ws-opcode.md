---
tags: [transcription, ws, server-control, surveillance, audio]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/qvi.java
  - work/jadx_base/sources/defpackage/u0d.java
related:
  - "[[20-ws-protocol-opcodes]]"
  - "[[42-voice-messages-opus-server-controlled]]"
  - "[[79-additional-onelog-categories]]"
---

# TRANSCRIBE_MEDIA (WS 202) — серверная транскрипция аудио/видео

`TRANSCRIBE_MEDIA` — WS-опкод 202 (enum #157). Ответ сервера парсится в `qvi.java`.

## Структура ответа

Сервер возвращает MessagePack-объект с полями:
- `transcription` — строка с текстом транскрипции
- `transcriptionStatus` — byte-статус (через `bzl.a(bN)` → `swi` enum)

Результат: `pvi(transcription, transcriptionStatus)`.

## Что это значит

1. **Серверная транскрипция** — клиент отправляет аудио/видео на сервер, сервер возвращает текст. Это означает, что **содержимое голосовых и видеосообщений уходит на серверы MAX для транскрипции**.

2. **`transcriptionStatus`** — статус транскрипции (успех, ошибка, в процессе). Это позволяет клиенту показывать прогресс транскрипции.

3. В сочетании с `AUDIO_TRANSCRIPTION` OneLog (см. [[79-additional-onelog-categories]]) — сервер знает: какие сообщения транскрибировались, как долго ждали, и успешно ли. Плюс сам текст транскрипции хранится на сервере.

4. **PmsKey `enable-audio-messages-transcription`** и **`enable-video-messages-transcription`** — server-gated включение. Сервер контролирует, для каких пользователей включена транскрипция.

5. **`audio-transcription-locales`** (#12) — whitelist языков. Сервер решает, какие языки транскрибировать.

## Скептический разбор

- Серверная транскрипция — стандартная фича (Telegram, WhatsApp делают то же самое).
- Что специфично: в государственном мессенджере без E2E-шифрования — содержимое голосовых сообщений уходит на серверы, которые находятся под юрисдикцией РФ.
- `transcriptionStatus` как byte — возможно, статусы включают «в очереди», «обрабатывается», «готово», «ошибка».

## Сводка

`TRANSCRIBE_MEDIA(202)` — WS-опкод для серверной транскрипции. Ответ содержит `transcription` (текст) и `transcriptionStatus`. Содержимое голосовых/видеосообщений уходит на серверы MAX. Server-gated через PmsKey + whitelist локалей.
