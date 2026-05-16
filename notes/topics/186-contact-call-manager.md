---
tags: [calls, anonymous, contact, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/contacts/ContactCallManager.java
related:
  - "[[144-hangup-join-api]]"
  - "[[179-conversation-interface]]"
---

# ContactCallManager — анонимность в звонках

`ContactCallManager` — управление контактами и анонимностью в звонке.

## Методы/свойства

| Метод | Что |
|---|---|
| `iAmAnonymous` | **текущий пользователь анонимен** |
| `iWasInitiallyAnonymous` | **был ли анонимен при входе** |
| `addContactCallListener(listener)` | слушатель событий |

## Что важно

1. **`iAmAnonymous`** — текущий статус анонимности. Пользователь может присоединиться к звонку анонимно (через `anonymToken` из [[144-hangup-join-api]]).

2. **`iWasInitiallyAnonymous`** — был ли пользователь анонимен при входе. Это означает, что статус анонимности может меняться во время звонка (например, пользователь авторизовался).

3. Это связано с `isAnon` в `ConversationStats` (см. [[163-conversation-stats]]) — статистика собирается и для анонимных участников.

## Сводка

`ContactCallManager` — `iAmAnonymous`/`iWasInitiallyAnonymous`. Анонимные участники могут присоединяться через `anonymToken`. Статус анонимности может меняться во время звонка.
