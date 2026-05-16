---
tags: [in-app-review, rating, fake, google-play, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/inappreview/ui/FakeInAppReviewBottomSheet.java
related:
  - "[[12-force-update-killswitch]]"
---

# FakeInAppReviewBottomSheet — фейковый In-App Review

`FakeInAppReviewBottomSheet` — **фейковый** диалог оценки приложения (имитация Google Play In-App Review).

## Что делает

- Показывает иконку приложения
- Название и описание приложения
- Звёздный рейтинг (5 звёзд)
- Кнопки "Отправить" и "Нет, спасибо"

## Что важно

1. **"Fake"** — это не настоящий Google Play In-App Review, а собственная реализация MAX.

2. Оценки собираются напрямую MAX, а не через Google Play.

3. Это позволяет MAX собирать оценки без передачи в Google Play.

## Сводка

`FakeInAppReviewBottomSheet`: собственный диалог оценки приложения (не Google Play In-App Review). Оценки собираются напрямую MAX.
