---
tags: [onelog, telemetry, surveillance, location, search, inline-buttons]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/location/map/show/ShowLocationScreen.java
  - work/jadx_base/sources/defpackage/x8b.java
  - work/jadx_base/sources/defpackage/cm3.java
  - work/jadx_base/sources/defpackage/vl3.java
  - work/jadx_base/sources/defpackage/vl8.java
  - work/jadx_base/sources/one/me/chats/search/ChatsListSearchScreen.java
related:
  - "[[31-onelog-event-categories]]"
  - "[[34-yandex-maps-integration]]"
---

# ok9.f() — дополнительные OneLog события (geolocation, search, inline buttons)

`ok9.f(operation, attributes)` — упрощённый вариант `ok9.h()` без явной категории. Используется для событий, которые не попадают в именованные категории.

## Найденные события

| Событие | Атрибуты | Что |
|---|---|---|
| `geolocation_send_click` | `source_meta` | **нажатие кнопки «отправить геолокацию»** |
| `search_click` | `source_meta` | клик на результат поиска |
| `search_click_more_button` | — | нажатие «показать больше» в поиске |
| `inline_button_click` | `source_meta` | нажатие inline-кнопки в сообщении |

## Что важно

1. **`geolocation_send_click`** — сервер знает, когда пользователь нажимает кнопку отправки геолокации. В сочетании с [[34-yandex-maps-integration]] (координаты уходят на Yandex) — сервер знает и факт отправки, и координаты.

2. **`inline_button_click`** — нажатие inline-кнопок в сообщениях (кнопки ботов). Сервер знает, на какие кнопки ботов нажимает пользователь.

3. **`search_click`** — клик на результат поиска. В сочетании с [[48-search-opcodes-server-controlled]] (`MSG_SEARCH_TOUCH`) — два независимых механизма логирования кликов по результатам поиска.

## Сводка

`ok9.f()` — дополнительные OneLog события: `geolocation_send_click`, `search_click`, `inline_button_click`. Сервер знает каждое нажатие кнопки отправки геолокации и каждый клик на inline-кнопку бота.
