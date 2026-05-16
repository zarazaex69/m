---
tags: [protocol, ws, voters-list, org-info, livestream, polls, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/jlb.java
  - work/jadx_base/sources/defpackage/qb5.java
related:
  - "[[265-protos-poll-photo-file]]"
  - "[[296-ws-opcodes-final]]"
---

# VOTERS_LIST_BY_ANSWER + ORG_INFO + CHAT_LIVESTREAM_INFO WS

## VOTERS_LIST_BY_ANSWER (опкод 155) — список проголосовавших

| Поле | Что |
|---|---|
| `voteCount` | **количество голосов** |
| `voters` | **список проголосовавших** |
| `marker` | маркер пагинации |

## ORG_INFO (опкод 159) — информация об организации

| Поле | Что |
|---|---|
| `organizations` | **список организаций** |
| `trackId` | ID трекинга |
| `email` | email |
| `chatReactionsSettings` | настройки реакций |
| `hash` | хэш |
| `user` | данные пользователя |
| `query_id` | ID запроса |
| `url` | URL |

## CHAT_LIVESTREAM_INFO (опкод 160) — информация о live-стриме

| Поле | Что |
|---|---|
| `liveStreams` | **live-стримы** |

## Что важно

1. **`VOTERS_LIST_BY_ANSWER.voters`** — список пользователей, проголосовавших за конкретный ответ. Сервер знает, кто как голосовал.

2. **`ORG_INFO`** — информация об организации включает email и данные пользователя. Это корпоративная функция.

3. **`CHAT_LIVESTREAM_INFO.liveStreams`** — список live-стримов в чате.

## Сводка

`VOTERS_LIST_BY_ANSWER`: voteCount/voters/marker. `ORG_INFO`: organizations/email/user/hash. `CHAT_LIVESTREAM_INFO`: liveStreams.
