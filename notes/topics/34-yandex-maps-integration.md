---
tags: [yandex, maps, geolocation, third-party, telemetry]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/c60.java
  - work/jadx_base/sources/defpackage/jw7.java
  - work/jadx_base/sources/defpackage/xwk.java
related:
  - "[[04-telemetry-endpoints]]"
  - "[[03-pms-server-flags]]"
  - "[[19-fcm-push-payload]]"
---

# Yandex Maps — три endpoint-а для геолокации

В [[04-telemetry-endpoints]] упоминались Yandex-хосты в outbound-каналах. Здесь — конкретное использование.

## Endpoints

| Endpoint | Назначение | Где |
|---|---|---|
| `https://static-maps.yandex.ru/v1?lang=ru_RU&maptype=future_map&scale=1.5&size=<W>,<H>&z=<zoom>...` | растровое превью карты для embed в сообщения с локацией | `defpackage/c60.java` (3 места) |
| `https://tiles.api-maps.yandex.ru/v1/tiles/?lang=ru_RU&l=map&projection=web_mercator&maptype=future_map&...` | тайл-сервер для интерактивной карты | `defpackage/jw7.java` |
| `https://geocode-maps.yandex.ru/v1?lang=ru_RU&results=1&format=json&geocode=<lat>,<lon>&apikey=<key>` | reverse geocoding (координаты → адрес) | `defpackage/xwk.java` |

Параметры:
- `lang=ru_RU` (язык интерфейса карты)
- `maptype=future_map` (стиль карты — Yandex-внутренний)
- `projection=web_mercator` (стандарт для slippy maps)

## Apikey

`xwk.b` — поле класса, задаётся через конструктор третьим параметром `String str`. Откуда конкретно поступает значение — нужно проследить call-site (вероятно, через DI из конфига/manifest meta-data). Я не нашёл его захардкоженным в res/values/strings.xml или AndroidManifest.xml; видимо, инжектится из server config или собран на этапе сборки.

## Что значит

1. **Координаты пользователя при отправке локации передаются Yandex-у через `geocode-maps`** (для перевода в адрес) и через `static-maps` (для генерации превью).
2. То же — для входящих локаций: когда другой пользователь делится координатами, MAX генерирует превью через Yandex-static-maps. Yandex видит факт открытия карты этим пользователем и регион.
3. **Tile-сервер `tiles.api-maps.yandex.ru`** — просмотр карты внутри MAX (например, при выборе локации для отправки). Каждое действие пана/зума — отдельный tile-запрос.

## Скептический разбор

- Любое приложение с картами «куда-то» отправляет координаты — это норма (Google Maps, OpenStreetMap, Apple Maps так же).
- Нюанс: **Yandex Maps — российский сервис, владелец Yandex (через долю в VK Holding и обмен активами 2025 года — напрямую связан с холдингом VK)**. Координаты пользователей MAX не утекают «за рубеж», но они и не остаются только в инфраструктуре MAX — они уходят в инфраструктуру Yandex.
- Запросы идут с устройства напрямую на Yandex (не через прокси MAX-сервера), что значит:
    - IP-адрес пользователя видит Yandex.
    - Yandex может использовать `apikey` для группировки запросов «от MAX-клиента» отдельно.
    - cookie/трекинг Яндекса (если есть) применяется.

PmsKey `y-map` (см. [[03-pms-server-flags]] §1) — серверный switch для использования Yandex Maps. Сервер может включить альтернативную реализацию (например, OK-овский tile-server) — но в текущей сборке только `y-map`-путь видим в коде.

PmsKey `send-location-enabled` — отдельный switch для отправки локации в чат. Если выключен — UI отправки локации не появляется, и Yandex запросы, связанные с outbound location, не делаются. Но входящие локации (превью на чужие сообщения) идут независимо.

## Сводка

MAX 26.15.3 использует Yandex Maps как единственный картографический бэкенд (стиль `future_map`, RU-локализация по умолчанию). Координаты пользователя при отправке локации проходят через `geocode-maps.yandex.ru` для resolve в адрес. Это значит, что **Yandex отдельно видит факт обмена локациями в MAX** — отдельно от MAX-сервера.
