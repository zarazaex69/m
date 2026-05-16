---
tags: [in-app-review, server-control, pms, google-play, fake]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/inappreview/ui/FakeInAppReviewBottomSheet.java
  - work/jadx_base/sources/defpackage/ld7.java
  - work/jadx_base/sources/defpackage/jg8.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[03-pms-server-flags]]"
  - "[[05-dev-menu-in-prod]]"
---

# In-App Review — server-controlled триггеры + FakeInAppReview

## in-app-review-triggers (#?)

`PmsKey.f136inappreviewtriggers` — long, server-pushed битовая маска условий, при которых показывать запрос оценки приложения (Google Play In-App Review API).

`ld7.java` — `ForwardInAppReviewData(triggeredConditions=..., screen=...)` — объект с условиями, которые сработали, и экраном, на котором показывается запрос.

Это означает: **сервер контролирует, когда и при каких условиях пользователю показывается запрос оценки в Google Play**. Сервер может включить показ после определённых действий (первый звонок, N-е сообщение, и т.п.).

## fake-in-app-review

`PmsKey.f113fakeinappreview` — bool, server-gated. `FakeInAppReviewBottomSheet` — кастомный bottom sheet, который имитирует Google Play In-App Review UI, но **не является настоящим Google Play диалогом**.

Это означает: при `fake-in-app-review=true` сервер показывает **поддельный диалог оценки** вместо настоящего Google Play. Пользователь думает, что оценивает приложение в Play Store, но на самом деле оценка уходит на серверы MAX.

Дополнительные SharedPreferences флаги:
- `isDisableInAppReviewTimeCondition` — отключить временное условие (для тестирования)
- `isEnableInAppReviewNotFromMarketBuild` — включить для не-Play сборок

## Что важно

1. **`FakeInAppReviewBottomSheet`** — это не просто тестовый инструмент. Это production-ready компонент, который может быть включён через `fake-in-app-review` PmsKey. Пользователь не может отличить его от настоящего Google Play диалога.

2. **Сервер контролирует триггеры** — когда показывать запрос оценки. Это позволяет показывать запрос в «правильный» момент (после позитивного опыта) для максимизации рейтинга.

3. **Fake review** — если оценки собираются через `FakeInAppReviewBottomSheet`, они не попадают в Google Play, а уходят на серверы MAX. Это может использоваться для внутренней аналитики удовлетворённости.

## Сводка

`in-app-review-triggers` — server-controlled битовая маска условий показа запроса оценки. `fake-in-app-review` — server-gated поддельный диалог оценки, имитирующий Google Play UI. Оценки через fake-диалог уходят на серверы MAX, а не в Google Play.
