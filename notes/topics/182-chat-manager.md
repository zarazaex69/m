---
tags: [calls, chat, surveillance, webrtc, datachannel]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/chat/ChatManager.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/chat/message/OutboundMessage.java
related:
  - "[[179-conversation-interface]]"
  - "[[151-conversation-impl-callbacks]]"
---

# ChatManager — чат во время звонка

`ChatManager` — управление чатом внутри звонка (через WebRTC DataChannel).

## Методы

| Метод | Что |
|---|---|
| `sendMessage(OutboundMessage)` | отправить сообщение |
| `addListener(ChatManagerListener)` | слушатель сообщений |
| `removeListener(listener)` | удалить слушатель |

## OutboundMessage

| Поле | Что |
|---|---|
| `participantId` | получатель (null = всем) |
| `text` | **текст сообщения** |

## Что важно

1. **Чат через DataChannel** — сообщения передаются через WebRTC DataChannel, а не через WS-сервер. Это означает, что сообщения чата в звонке могут идти напрямую P2P.

2. **`participantId`** — можно отправить сообщение конкретному участнику (приватное) или всем (null).

3. **`onCustomData`** (см. [[151-conversation-impl-callbacks]]) — callback для входящих данных через DataChannel. `ChatManager` использует тот же механизм.

4. Если P2P relay включён (см. [[125-p2p-relay-webtransport]]) — DataChannel трафик идёт через серверы VK/OK. `Calls-SDK-LogDCTraffic` (см. [[135-callssdk-opus-dred-fec-flags]]) — логирование DataChannel трафика.

## Сводка

`ChatManager` — чат в звонке через DataChannel. `OutboundMessage`: `participantId` + `text`. При P2P relay — трафик через серверы VK/OK.
