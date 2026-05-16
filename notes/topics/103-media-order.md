---
tags: [media, server-control, pms, ui]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/e.java
  - work/jadx_base/sources/defpackage/c60.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[03-pms-server-flags]]"
---

# media-order — server-controlled порядок медиа в сообщениях

`PmsKey.f174mediaorder` — long, default `0`. Описание из `e.java`:

| Значение | Что |
|---|---|
| `0` | Медиа всегда снизу |
| `1` | Медиа всегда сверху |
| `2` | Медиа сверху только в постах каналов |
| `3` | **Порядок управляется с бека** |

При `media-order=3` — порядок медиа в сообщениях определяется сервером динамически. Это означает, что сервер может менять расположение медиа (фото/видео) относительно текста в сообщениях без обновления клиента.

## Что важно

Это UX-контроль: сервер решает, где показывать медиа в сообщениях. При `media-order=3` — полный server-control над layout сообщений.

## Сводка

`media-order` — 4 режима расположения медиа в сообщениях. При значении `3` — порядок управляется сервером динамически.
