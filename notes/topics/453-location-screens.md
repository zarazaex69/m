---
tags: [location, google-maps, pick-location, show-location, surveillance, analytics]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/location/map/pick/PickLocationScreen.java
  - work/jadx_base/sources/one/me/location/map/show/ShowLocationScreen.java
related:
  - "[[346-fused-location-provider]]"
  - "[[354-yandex-maps-config]]"
  - "[[363-google-maps-api-key]]"
  - "[[412-protos-location]]"
---

# PickLocationScreen + ShowLocationScreen — карты

## PickLocationScreen — выбор геолокации

Параметры: `chatId`, `lat`, `lon`, `zoom`.

Использует Google Maps (`GoogleMap`). Стили: `google_universal_map_style` / `google_map_night_style`.

## ShowLocationScreen — просмотр геолокации

Параметры: `chatId`, `senderId`, `msgId`, `lat`, `lon`, `zoom`, `sourceTypeId`, `sourceId`.

### Аналитика

При клике "Поделиться геолокацией":
```json
{
  "source_id": long,
  "source_type": int
}
```
Событие: `geolocation_send_click`.

## Что важно

1. **`geolocation_send_click`** — каждый клик "Поделиться геолокацией" логируется с `source_id`/`source_type`.

2. **Google Maps** — используется Google Maps API (ключ `AIzaSyDJbuC3fODS_aR7jcOkoP6qWIsQen9XARI`).

3. **`senderId`/`msgId`** — при просмотре геолокации известен отправитель и сообщение.

## Сводка

`PickLocationScreen(chatId, lat, lon, zoom)`. `ShowLocationScreen(chatId, senderId, msgId, lat, lon, zoom)`. Аналитика: `geolocation_send_click(source_id, source_type)`.
