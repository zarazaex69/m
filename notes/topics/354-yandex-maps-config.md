---
tags: [server-control, yandex-maps, map-config, api-key, geocoder, tiles]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/s5l.java
  - work/jadx_base/sources/defpackage/xwk.java
  - work/jadx_base/sources/defpackage/jw7.java
  - work/jadx_base/sources/defpackage/c60.java
related:
  - "[[03-pms-server-flags]]"
  - "[[235-qp6-additional-pmskeys]]"
---

# y-map PmsKey — серверная конфигурация Яндекс.Карт

`y-map` (PmsKey #325) — серверная конфигурация Яндекс.Карт. JSON-объект с URL-шаблонами и API ключом.

## Поля конфигурации (MapUrlConfig)

| Поле | Что |
|---|---|
| `tile` | **URL тайлов карты** |
| `geocoder` | **URL геокодера** |
| `static` | **URL статических карт** |
| `logoLight` | URL логотипа (светлая тема) |
| `logoDark` | URL логотипа (тёмная тема) |

## Используемые endpoints

| Endpoint | Что |
|---|---|
| `https://tiles.api-maps.yandex.ru/v1/tiles/?lang=ru_RU&l=map&...` | **тайлы карты** |
| `https://geocode-maps.yandex.ru/v1?lang=ru_RU&results=1&format=json&geocode=<lon>,<lat>&apikey=<key>` | **геокодирование** |
| `https://static-maps.yandex.ru/v1?lang=ru_RU&maptype=future_map&...` | **статические карты** |

## Что важно

1. **`apikey`** в геокодере — Яндекс.Карты API ключ передаётся с сервера через PmsKey `y-map`. Ключ не hardcoded.

2. **Сервер контролирует** все URL Яндекс.Карт. Можно заменить на другой сервис.

3. **Геокодирование** — при отправке геолокации в чат, координаты отправляются на `geocode-maps.yandex.ru`.

## Сводка

`y-map` PmsKey: tile/geocoder/static/logoLight/logoDark. Яндекс.Карты API ключ приходит с сервера. Геокодер: `geocode-maps.yandex.ru/v1?...&apikey=<server_key>`.
