---
tags: [database, stickers, profile, schema]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/sk.java
related:
  - "[[216-local-db-tables]]"
  - "[[269-protos-sticker]]"
---

# stickers + sticker_sets + profile DB схемы

## stickers

| Поле | Что |
|---|---|
| `sticker_id` | серверный ID стикера |
| `width` / `height` | размер |
| `url` | URL стикера |
| `update_time` | время обновления |
| `mp4_url` | URL MP4 (анимированный) |
| `first_url` | URL первого кадра |
| `preview_url` | URL превью |
| `tags` | **теги** |
| `sticker_type` | тип (STATIC/LIVE/POSTCARD/LOTTIE) |
| `set_id` | ID набора |
| `lottie_url` | URL Lottie-анимации |
| `audio` | аудио-стикер |
| `author_type` | тип автора |
| `video_url` | URL видео |

## sticker_sets

| Поле | Что |
|---|---|
| `id` | ID набора |
| `name` | название |
| `icon_url` | URL иконки |
| `author_id` | **ID автора** |
| `created_time` / `updated_time` | времена |
| `link` | ссылка |
| `stickers` | стикеры (BLOB) |
| `draft` | черновик |

## profile

| Поле | Что |
|---|---|
| `id` | локальный ID |
| `server_id` | серверный ID |
| `profile` | **BLOB — данные профиля** |

## Сводка

`stickers`: 16 полей (url/mp4_url/lottie_url/audio/tags). `sticker_sets`: name/icon_url/author_id/link. `profile`: id/server_id/profile(BLOB).
