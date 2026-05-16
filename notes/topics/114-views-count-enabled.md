---
tags: [channels, views, server-control, pms, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/pl5.java
  - work/jadx_base/sources/defpackage/v7b.java
  - work/jadx_base/sources/defpackage/sic.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[36-channels-feature-gated]]"
  - "[[03-pms-server-flags]]"
---

# views-count-enabled — счётчик просмотров в каналах

`PmsKey.f303viewscountenabled` — bool, default `false`. Описание из `pl5.java`: «Включить просмотры в каналах».

## Что делает

При `views-count-enabled=true`:
1. Клиент показывает счётчик просмотров для постов в каналах.
2. При просмотре поста (`M0 != 0` — message ID) — клиент отправляет на сервер факт просмотра.

`v7b.java:1707` — при включённом флаге и непустом message ID — отправляет событие просмотра.

## Что важно

1. **Сервер знает, какие посты в каналах просматривал пользователь** — при включённом `views-count-enabled` каждый просмотр поста логируется.

2. **Server-gated** — сервер включает счётчик просмотров только для определённых каналов или пользователей.

3. **Связь с `channels-enabled`** (см. [[36-channels-feature-gated]]) — каналы сами по себе server-gated, и счётчик просмотров тоже.

## Сводка

`views-count-enabled` — server-gated счётчик просмотров постов в каналах. При включении сервер знает, какие посты просматривал пользователь.
