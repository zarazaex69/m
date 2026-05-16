---
tags: [protocol, message-elements, protos, schema, mentions, reactions]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Protos.java
related:
  - "[[206-protos-schemas]]"
  - "[[257-protos-attach-schema]]"
---

# Protos — MessageElement, ReactionData, PendingUploadData

## MessageElement — форматирование текста

Типы элементов:
`USER_MENTION`(0), `GROUP_MENTION`(1), `STRONG`(2), `MONOSPACED`(3), `EMPHASIZED`(4), `LINK`(5), `STRIKETHROUGH`(6), `UNDERLINE`(7), `HEADING`(8), `CODE`(9), `ANIMOJI`(10), `QUOTE`(11).

Поля: `entityId` (ID упомянутого), `from` (позиция), `length` (длина), `type`.

## ReactionData

| Поле | Что |
|---|---|
| `reaction` | **реакция** (emoji/стикер) |
| `type` | `EMOJI`(0) / `STICKER`(1) |

## PendingUploadData

| Поле | Что |
|---|---|
| `pendingMap` | **Map<Long, String>** — ожидающие загрузки |

## Что важно

1. **`MessageElement.entityId`** — ID упомянутого пользователя/группы. Сервер знает все упоминания в сообщениях.

2. **`ReactionData.type`** — реакции могут быть emoji или стикерами.

3. **`PendingUploadData.pendingMap`** — Map незавершённых загрузок. Хранится локально для восстановления.

## Сводка

`MessageElement`: 12 типов форматирования + entityId/from/length. `ReactionData`: reaction/type(EMOJI/STICKER). `PendingUploadData`: pendingMap(Map<Long,String>).
