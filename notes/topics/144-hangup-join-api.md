---
tags: [calls, api, hangup, join, reason]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/api/request/HangupConversation.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/api/request/JoinConversationByLink.java
related:
  - "[[143-start-conversation-api]]"
  - "[[96-vchat-api-methods]]"
---

# vchat.hangupConversation и vchat.joinConversationByLink

## vchat.hangupConversation

Параметры: `conversationId`, `reason` (enum), `anonymToken`.

`reason` — причина завершения звонка. Сервер знает, почему завершился каждый звонок (пользователь повесил трубку, ошибка сети, и т.п.).

## vchat.joinConversationByLink

Параметры: `joinLink`, `isVideo`, `peerId`, `anonymToken`, `capabilities` (hex), `payload`, `protocolVersion=6`.

`peerId` — long. Клиент отправляет свой peer ID при присоединении по ссылке.

## Что важно

1. **`reason` в hangup** — сервер получает причину завершения каждого звонка. Это детальная аналитика.

2. **`peerId`** — при присоединении по ссылке клиент отправляет peer ID. Это идентификатор участника в WebRTC-сессии.

3. **`anonymToken`** — анонимный токен для присоединения по ссылке без аутентификации. Это позволяет присоединиться к звонку без аккаунта MAX.

## Сводка

`vchat.hangupConversation` — причина завершения звонка. `vchat.joinConversationByLink` — присоединение по ссылке с peerId, capabilities, anonymToken.
