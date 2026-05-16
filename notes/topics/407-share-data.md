---
tags: [share, data-types, vcard, contact, messages, files]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/android/util/share/ShareData.java
related:
  - "[[06-contacts]]"
  - "[[10-webapp-jsbridge]]"
---

# ShareData — типы данных для шаринга

`ShareData` — Parcelable объект для передачи данных при шаринге в MAX.

## Типы (type)

| Константа | Значение | Что |
|---|---|---|
| `TEXT` | 0 | текст |
| `IMAGES` | 1 | изображения |
| `VIDEOS` | 2 | видео |
| `SHARES` | 3 | шаринг |
| `FILES` | 4 | файлы |
| `VCARD` | 5 | **vCard (контакт)** |
| `MESSAGES` | 6 | сообщения |
| `CONTACT` | 7 | контакт |
| `STICKER_SET` | 8 | набор стикеров |

## Поля

| Поле | Что |
|---|---|
| `text` | текст |
| `images` | список URI изображений |
| `videos` | список URI видео |
| `files` | список URI файлов |
| `shares` | список URI для шаринга |
| `ids` | список Long ID |
| `vcard` | **vCard строка** |

## Что важно

1. **`VCARD`** — шаринг контактов в формате vCard. Содержит полные данные контакта.

2. **`MESSAGES`** — шаринг сообщений. Пересылка сообщений между чатами.

3. **`toString()`** — в debug-режиме (`ct4.b()`) логирует images, иначе `"****"`.

## Сводка

`ShareData`: 9 типов (TEXT/IMAGES/VIDEOS/FILES/VCARD/MESSAGES/CONTACT/STICKER_SET). Поля: text/images/videos/files/vcard/ids.
