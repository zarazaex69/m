---
tags: [api, endpoints, server-control, network, dev-menu]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/r58.java
  - work/jadx_base/sources/defpackage/tk6.java
  - work/jadx_base/sources/defpackage/vsl.java
  - work/jadx_base/sources/defpackage/v48.java
related:
  - "[[04-telemetry-endpoints]]"
  - "[[05-dev-menu-in-prod]]"
  - "[[12-force-update-killswitch]]"
---

# api.oneme.ru — основной API endpoint и тестовые серверы

## Основной endpoint

`api.oneme.ru:443` (TLS) — основной API-сервер MAX. Это WS-сервер и HTTP API одновременно.

## Тестовые серверы (из dev-меню)

`r58.java:23` — список серверов в dev-меню:
- `api.oneme.ru` — production
- `api-test.oneme.ru` — тестовый
- `api-tg.oneme.ru` — TG-тестовый (вероятно Telegram-совместимый тест)
- `api-test2.oneme.ru` — второй тестовый

`h9.java` — deeplink-обработчик принимает хосты: `max.ru`, `api-test.oneme.ru`, `api-tg.oneme.ru`.

## Что важно

1. **`api-tg.oneme.ru`** — тестовый сервер с суффиксом `tg`. Это может быть Telegram-совместимый API или просто тестовый сервер с именем разработчика. Требует дополнительного анализа.

2. **Dev-меню позволяет переключиться на тестовые серверы** (см. [[05-dev-menu-in-prod]]). При переключении на `api-test.oneme.ru` все данные пользователя уходят на тестовый сервер.

3. **`api.oneme.ru:443`** — единственный production endpoint. Все WS-соединения, HTTP API, OneLog, vchat.* — всё через этот хост.

4. **`f58.java:148`** — HostReachabilityChecker (см. [[24-host-reachability-probe]]) проверяет `api.oneme.ru` и тестовые серверы.

## Сводка

Основной API: `api.oneme.ru:443`. Тестовые серверы в dev-меню: `api-test.oneme.ru`, `api-tg.oneme.ru`, `api-test2.oneme.ru`. Deeplink-обработчик принимает тестовые хосты. `api-tg.oneme.ru` — неизвестное назначение.
