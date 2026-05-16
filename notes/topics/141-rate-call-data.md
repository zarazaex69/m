---
tags: [calls, quality-rating, server-control, questions]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/rate/RateCallData.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/rate/Question.java
related:
  - "[[88-call-rate-quality-rating]]"
  - "[[140-conversation-params]]"
---

# RateCallData — server-pushed вопросы для оценки звонка

`RateCallData` — данные для диалога оценки звонка, приходящие с сервера через `vchat.getConversationParams`.

## Структура

- `maxRateForQuestion` — максимальная оценка (например, 5 для 5-звёздочной шкалы)
- `questions` — список `Question` объектов

`Question`:
- `index` — индекс вопроса
- `title` — **текст вопроса**

## Что важно

1. **Сервер задаёт вопросы для оценки звонка**. Это означает, что сервер может изменить вопросы без обновления клиента. Например, добавить вопрос «Были ли проблемы с качеством?» или «Оцените звонок».

2. **`maxRateForQuestion`** — сервер задаёт шкалу оценки. Может быть 5 (звёзды), 10 (баллы), или другое.

3. В сочетании с [[88-call-rate-quality-rating]] — полный server-controlled диалог оценки: условия показа (`android.rating.limits`), вопросы (`RateCallData`), и шкала (`maxRateForQuestion`).

## Сводка

`RateCallData` — server-pushed вопросы и шкала для диалога оценки звонка. Сервер полностью контролирует содержимое диалога оценки.
