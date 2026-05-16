---
tags: [presence, server-control, pms]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/pl5.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[32-presence-server-controlled]]"
  - "[[03-pms-server-flags]]"
---

# Presence PmsKey — полная карта (дополнение к topic 32)

В дополнение к [[32-presence-server-controlled]] — полный список presence-PmsKey с описаниями из `pl5.java`.

## Полный список

| Ключ | # | Default | Описание |
|---|---|---|---|
| `presence-ttl` | (в rtd) | 300 | TTL presence (секунды) |
| `presence-view-port` | (в qp6) | false | «Presence viewport» — обновлять только видимые чаты |
| `presence-external` | (в qp6) | false | «Presence external» — запрашивать presence для внешних пользователей |
| `presence-seen-eq` | (в qp6) | false | «Presence не-legacy сравнение» — новый алгоритм сравнения |
| `presence-stat` | (в qp6) | false | «Presence stat» — статистика presence |
| `presence-keep-bg-cache` | (в qp6) | — | хранить кэш presence в фоне |
| `notif-typing-presence` | (в qp6) | — | обновлять presence при typing-уведомлении |
| `presence-offline-move-timeout` | (в qp6) | — | таймаут перехода в offline |
| `presence-offline-log` | (в qp6) | — | логировать offline-переходы |

## Что важно

1. **`presence-external`** — server-gated запрос presence для внешних пользователей (не в контактах). При включении клиент запрашивает статус онлайн для незнакомцев.

2. **`presence-seen-eq`** — «не-legacy сравнение» — новый алгоритм определения, изменился ли presence. Это оптимизация, но server-gated.

3. **`presence-stat`** — server-gated статистика presence. При включении клиент собирает и отправляет статистику presence-событий.

4. **`presence-ttl=300`** — presence обновляется каждые 5 минут. Сервер контролирует этот интервал.

## Сводка

9 presence-PmsKey. Ключевые: `presence-external` (запрос presence незнакомцев), `presence-stat` (статистика), `presence-ttl=300s`. Все server-controlled.
