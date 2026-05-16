---
tags: [protocol, ws, opcodes, polls, transcription, audio-play, org-info, livestream]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/u0d.java
related:
  - "[[286-ws-opcodes-additional]]"
  - "[[20-ws-protocol-opcodes]]"
---

# WS Opcodes — финальный список (148-160)

Дополнение к [[286-ws-opcodes-additional]]. Последние опкоды (148-160).

## Новые опкоды

| Опкод | Имя | Что |
|---|---|---|
| 148 | `AUTH_QR_APPROVE` | **авторизация через QR-код** |
| 149 | `NOTIF_BANNERS` | уведомление о баннерах |
| 150 | `BANNERS_GET` | получить баннеры |
| 151 | `CHAT_SUGGEST` | **предложение чата** |
| 152 | `AUDIO_PLAY` | воспроизведение аудио |
| 153 | `MSG_DELIVERY` | **статус доставки сообщения** |
| 154 | `SEND_VOTE` | **отправить голос** (опрос) |
| 155 | `VOTERS_LIST_BY_ANSWER` | **список проголосовавших по ответу** |
| 156 | `GET_POLL_UPDATES` | получить обновления опроса |
| 157 | `TRANSCRIBE_MEDIA` | **транскрибировать медиа** |
| 158 | `NOTIF_TRANSCRIPTION` | **уведомление о транскрипции** |
| 159 | `ORG_INFO` | информация об организации |
| 160 | `CHAT_LIVESTREAM_INFO` | информация о live-стриме |

## Что важно

1. **`AUTH_QR_APPROVE`** — авторизация через QR-код. Это механизм входа с другого устройства.

2. **`VOTERS_LIST_BY_ANSWER`** — список проголосовавших по конкретному ответу. Сервер знает, кто как проголосовал.

3. **`TRANSCRIBE_MEDIA`/`NOTIF_TRANSCRIPTION`** — транскрипция медиа через WS. Клиент запрашивает транскрипцию, сервер уведомляет о результате.

4. **`MSG_DELIVERY`** — статус доставки сообщения. Сервер уведомляет о доставке.

5. **`CHAT_SUGGEST`** — предложение чата. Сервер может предлагать чаты пользователю.

## Итого WS опкодов

160 опкодов (0-160). Полный список охватывает все аспекты протокола MAX.

## Сводка

Финальные WS опкоды: AUTH_QR_APPROVE/NOTIF_BANNERS/BANNERS_GET/CHAT_SUGGEST/AUDIO_PLAY/MSG_DELIVERY/SEND_VOTE/VOTERS_LIST_BY_ANSWER/GET_POLL_UPDATES/TRANSCRIBE_MEDIA/NOTIF_TRANSCRIPTION/ORG_INFO/CHAT_LIVESTREAM_INFO.
