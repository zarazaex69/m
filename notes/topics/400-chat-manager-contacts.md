---
tags: [calls-chat, in-call-chat, messages, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/chat/ChatManager.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/chat/message/InboundMessage.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/chat/message/OutboundMessage.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/contacts/ContactCallManager.java
related:
  - "[[391-conversation-events-listener]]"
  - "[[392-conversation-participant]]"
---

# ChatManager + ContactCallManager — чат и контакты в звонках

## ChatManager — чат внутри звонка

| Метод | Что |
|---|---|
| `sendMessage(OutboundMessage, onSuccess, onError)` | отправить сообщение |
| `addListener(ChatManagerListener)` | подписаться на входящие |

### InboundMessage

| Поле | Что |
|---|---|
| `senderId` | **ParticipantId** отправителя |
| `text` | **текст сообщения** |
| `isDirect` | личное сообщение |

### OutboundMessage

| Поле | Что |
|---|---|
| `participantId` | получатель (null = всем) |
| `text` | текст сообщения |

## ContactCallManager — анонимность в звонках

| Метод | Что |
|---|---|
| `getIAmAnonymous()` | я анонимный участник |
| `getIWasInitiallyAnonymous()` | я был анонимным изначально |
| `onContactCallMyAnonChanged()` | изменение статуса анонимности |

## Что важно

1. **Чат внутри звонка** — сообщения проходят через сервер. Сервер знает содержимое чата.

2. **`isDirect`** — личные сообщения внутри звонка.

3. **`getIAmAnonymous()`/`getIWasInitiallyAnonymous()`** — отслеживание анонимности участника. Сервер знает, был ли участник анонимным.

## Сводка

`ChatManager.sendMessage(OutboundMessage(participantId, text))`. `InboundMessage(senderId, text, isDirect)`. `ContactCallManager`: getIAmAnonymous/getIWasInitiallyAnonymous.
