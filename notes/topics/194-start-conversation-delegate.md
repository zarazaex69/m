---
tags: [calls, start-conversation, delegate, callee-ids]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/api/delegate/StartConversationDelegate.java
related:
  - "[[143-start-conversation-api]]"
  - "[[193-call-info]]"
---

# StartConversationDelegate — делегат старта звонка

`StartConversationDelegate` — интерфейс для старта звонка. Используется в `ConversationFactory`.

## Params

| Поле | Что |
|---|---|
| `conversationId` | ID разговора |
| `calleeIds` | **список ID вызываемых** |
| `chatId` | ID чата |
| `isVideo` | видеозвонок |
| `internalParams` | внутренние параметры |

## Что важно

1. **`calleeIds`** — список ID пользователей, которым звонят. Сервер знает, кому звонит пользователь.

2. **`chatId`** — звонок привязан к конкретному чату. Это означает, что звонок является частью переписки.

3. **`internalParams`** — внутренние параметры. Это может быть дополнительная конфигурация, специфичная для конкретного типа звонка.

## Сводка

`StartConversationDelegate.Params`: `conversationId`/`calleeIds`/`chatId`/`isVideo`/`internalParams`. Сервер знает, кому звонит пользователь и в каком чате.
