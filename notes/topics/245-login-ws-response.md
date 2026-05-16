---
tags: [protocol, login, ws, session-init, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/dyb.java
  - work/jadx_base/sources/defpackage/ul9.java
related:
  - "[[20-ws-protocol-opcodes]]"
  - "[[244-log-sensitive-server-flag]]"
  - "[[57-ws-session-config-fingerprint]]"
---

# LOGIN WS опкод — структура ответа

`LOGIN` (опкод 19) — ответ сервера на вход в систему. Содержит полное состояние сессии.

## Поля ответа LOGIN

| Поле | Что |
|---|---|
| `config` | **конфигурация** (PmsKey и др.) |
| `drafts` | черновики |
| `presence` | статусы присутствия |
| `contacts` | **контакты** |
| `messages` | сообщения |
| `profile` | профиль пользователя |
| `updates` | обновления |
| `time` | серверное время |
| `calls` | история звонков |
| `chats` | чаты |
| `unsafe` | небезопасный режим |
| `log-full` | **включить полное логирование** |
| `log-sensitive` | **включить логирование чувствительных данных** |

## LOGIN.Response поля

| Поле | Что |
|---|---|
| `profile` | профиль |
| `token` | **токен авторизации** |
| `chatMarker` | маркер чата |
| `videoChatHistory` | история видеозвонков |
| `resetAt` | время сброса |
| `contactInfos` | информация о контактах |
| `config` | конфигурация |

## Что важно

1. **`log-full`/`log-sensitive`** — сервер может включить детальное логирование прямо в ответе на LOGIN.

2. **`contacts`** — контакты передаются в ответе на LOGIN. Сервер знает все контакты при каждом входе.

3. **`config`** — конфигурация (PmsKey) передаётся при LOGIN. Это первичная загрузка серверных флагов.

## Сводка

LOGIN ответ: config/drafts/presence/contacts/messages/profile/updates/time/calls/chats + log-full/log-sensitive. Полное состояние сессии при каждом входе.
