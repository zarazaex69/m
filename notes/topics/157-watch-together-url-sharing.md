---
tags: [calls, watch-together, url-sharing, server-control, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/watch_together/internal/commands/WatchTogetherCommandExecutorImpl.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/urlsharing/external/internal/commands/UrlSharingCommandsExecutorImpl.java
related:
  - "[[153-signaling-commands]]"
  - "[[154-conversation-features-roles]]"
---

# WatchTogether и URL Sharing — совместный просмотр в звонках

## WatchTogether (add-movie/update-movie/remove-movie)

`add-movie` сигналинг-команда содержит:
- `movieId` — ID видео
- `gain` — громкость
- `moveToAdminOnHangup` — передать управление администратору при завершении

`update-movie` — обновить состояние видео (позиция, пауза, и т.п.).
`remove-movie` — удалить видео.

## URL Sharing (start-url-sharing/stop-url-sharing)

`start-url-sharing` — начать шаринг URL.
`stop-url-sharing` — остановить шаринг URL.

## Что важно

1. **`movieId`** — сервер знает, какое видео смотрят участники звонка.

2. **`moveToAdminOnHangup`** — при завершении звонка управление видео передаётся администратору. Это означает, что видео продолжает воспроизводиться после ухода создателя.

3. **`start-url-sharing`** — шаринг URL в звонке. Сервер знает, какие URL шарятся во время звонков.

4. **`MOVIE_SHARE` фича** (см. [[154-conversation-features-roles]]) — `onlyAdminCanShareMovie` контролирует, кто может шарить видео.

## Сводка

WatchTogether: `add-movie` (movieId/gain/moveToAdminOnHangup), `update-movie`, `remove-movie`. URL Sharing: `start-url-sharing`/`stop-url-sharing`. Сервер знает, какие видео и URL шарятся в звонках.
