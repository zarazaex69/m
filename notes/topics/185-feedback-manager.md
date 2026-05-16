---
tags: [calls, feedback, reactions, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/feedback/FeedbackManager.java
related:
  - "[[159-stereo-room-manager]]"
  - "[[179-conversation-interface]]"
---

# FeedbackManager — обратная связь в звонках (реакции)

`FeedbackManager` — управление обратной связью (реакциями) участников во время звонка.

## Методы

| Метод | Что |
|---|---|
| `sendFeedback(key, source)` | **отправить реакцию** |
| `setTimeout(millis)` | установить таймаут реакции |
| `getOwnCurrentFeedback()` | получить текущую реакцию |
| `addListener(listener)` | слушатель реакций |

## Что важно

1. **`sendFeedback(key, source)`** — `key` — тип реакции (например, «👍», «❤️», «🎉»). `source` — источник реакции (участник).

2. **`setTimeout`** — реакция автоматически исчезает через `millis` миллисекунд.

3. **`onFeedback(event)`** в `StereoRoomManager` (см. [[159-stereo-room-manager]]) — callback при получении реакции от участника.

4. Это механизм «поднятой руки» или emoji-реакций во время вебинара/звонка.

## Сводка

`FeedbackManager` — emoji-реакции в звонках. `sendFeedback(key, source)` + `setTimeout`. Сервер знает, кто и когда отправил реакцию.
