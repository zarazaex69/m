---
tags: [protocol, ws, audio-play, send-vote, polls, call-info, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/seh.java
  - work/jadx_base/sources/defpackage/tgb.java
related:
  - "[[296-ws-opcodes-final]]"
  - "[[265-protos-poll-photo-file]]"
---

# AUDIO_PLAY + SEND_VOTE WS

## AUDIO_PLAY (опкод 152) — воспроизведение аудио

| Поле | Что |
|---|---|
| `opus` | **Opus формат** |
| `mp3` | MP3 формат |
| `m4a` | M4A формат |
| `messagesReactions` | реакции на сообщения |
| `conversation_id` | ID разговора |
| `join_link` | ссылка для входа |
| `chat_id` | ID чата |
| `callName` | **имя звонка** |
| `callerId` | **ID звонящего** |
| `type` | тип |
| `token_refresh_ts` / `token_lifetime_ts` | токен |
| `info` / `uploaderType` | информация |

## SEND_VOTE (опкод 154) — голосование в опросе

| Поле | Что |
|---|---|
| `trackId` | ID трекинга |
| `chat` | данные чата |
| `state` | **состояние опроса** |

## Что важно

1. **`AUDIO_PLAY`** — многофункциональный опкод. Содержит данные о звонке (`callName`/`callerId`) и аудио-форматах.

2. **`SEND_VOTE.state`** — состояние опроса после голосования. Сервер возвращает обновлённое состояние.

3. **`AUDIO_PLAY.messagesReactions`** — реакции на сообщения передаются вместе с аудио-данными.

## Сводка

`AUDIO_PLAY`: opus/mp3/m4a/callName/callerId/messagesReactions. `SEND_VOTE`: trackId/chat/state.
