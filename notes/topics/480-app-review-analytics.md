---
tags: [app-review, rating, telemetry, surveillance, fake-review]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/fg8.java
related:
  - "[[450-fake-inapp-review]]"
  - "[[472-log-controller-event-types]]"
---

# APP_REVIEW.app_review — аналитика оценки приложения

Событие `APP_REVIEW.app_review` отправляется при оценке приложения через `FakeInAppReviewBottomSheet`.

## Структура события

| Поле | Что |
|---|---|
| `session_id` | **ID сессии** (`ri9.N()`) |
| `screen_from` | **экран, с которого открыта оценка** |
| `trigger` | **триггер показа** |
| `mark` | **оценка** (1-5 звёзд) |

## Что важно

1. **`mark`** — оценка пользователя (1-5). Сервер знает, как пользователь оценил приложение.

2. **`trigger`** — причина показа диалога оценки.

3. **`screen_from`** — экран, с которого открыта оценка.

4. Связано с [[450-fake-inapp-review]] — это собственный диалог оценки MAX, не Google Play.

## Сводка

`APP_REVIEW.app_review {session_id, screen_from, trigger, mark}`. Оценка пользователя отправляется напрямую на сервер MAX.
