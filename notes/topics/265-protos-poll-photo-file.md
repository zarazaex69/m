---
tags: [protocol, poll, photo, file, protos, schema, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Protos.java
related:
  - "[[206-protos-schemas]]"
  - "[[257-protos-attach-schema]]"
---

# Protos.Poll + Photo + File — вложения

## Protos.Poll

| Поле | Что |
|---|---|
| `pollId` | ID опроса |
| `settings` | настройки |
| `version` | версия |

**Poll.Answer**: `answerId`.

**Poll.AnswerStats**: `userId` + `timestamp` — **сервер знает, кто и когда проголосовал**.

**Poll.State**: `result[]` / `total` / `voterPreviewIds[]` — **ID пользователей для превью голосовавших**.

## Protos.Photo

| Поле | Что |
|---|---|
| `photoId` | ID фото |
| `height` / `width` | размер |
| `gif` | GIF |
| `previewData` | данные превью |
| `thumbhashData` | ThumbHash |

## Protos.File

| Поле | Что |
|---|---|
| `fileId` | ID файла |
| `size` | размер |

## Что важно

1. **`Poll.AnswerStats.userId`** — сервер знает, кто проголосовал в опросе (если не анонимный).

2. **`Poll.State.voterPreviewIds[]`** — ID пользователей для превью голосовавших. Видны другим участникам.

3. **`Photo.gif`** — фото может быть GIF.

## Сводка

`Poll.AnswerStats`: userId/timestamp (кто и когда голосовал). `Poll.State.voterPreviewIds[]`. `Photo`: photoId/height/width/gif/thumbhashData. `File`: fileId/size.
