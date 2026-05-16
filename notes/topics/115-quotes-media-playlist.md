---
tags: [messaging, server-control, pms, markdown, media]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/e.java
  - work/jadx_base/sources/defpackage/unc.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[03-pms-server-flags]]"
---

# quotes-enabled и media-playlist-enabled — server-gated фичи сообщений

## quotes-enabled (#247)

`PmsKey.f247quotesenabled` — bool, default `false`. Описание: «Отображение и отправка markdown quote элемента».

При включении — пользователи могут использовать markdown-цитаты (`>`) в сообщениях. Это server-gated включение markdown-форматирования.

## media-playlist-enabled (#175)

`PmsKey.f175mediaplaylistenabled` — bool, default `false`. Включает плейлист медиа (последовательное воспроизведение медиа в чате).

## Что важно

1. **`quotes-enabled`** — markdown-цитаты выключены по умолчанию. Сервер включает их. Это означает, что сервер контролирует, какие элементы форматирования доступны пользователям.

2. **`media-playlist-enabled`** — плейлист медиа выключен по умолчанию. При включении пользователь может последовательно воспроизводить медиа в чате.

## Сводка

`quotes-enabled` — server-gated markdown-цитаты. `media-playlist-enabled` — server-gated медиа-плейлист. Оба выключены по умолчанию.
