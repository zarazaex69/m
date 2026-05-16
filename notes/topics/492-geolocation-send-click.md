---
tags: [geolocation, telemetry, surveillance, yandex-maps, location-share]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/x8b.java
related:
  - "[[453-location-screens]]"
  - "[[412-protos-location]]"
---

# geolocation_send_click — аналитика отправки геолокации

Событие `geolocation_send_click` отправляется при клике "Поделиться геолокацией" в чате.

## Структура события

```json
{
  "source_meta": {
    "source_id": long,
    "source_type": int
  }
}
```

## Дополнительно

При открытии геолокации в Яндекс.Картах:

```java
Uri.parse("yandexmaps://maps.yandex.ru")
  .appendQueryParameter("pt", lon + "," + lat)
```

Если Яндекс.Карты не установлены → fallback на `https://yandex.ru/maps`.

## Что важно

1. **`source_id`/`source_type`** — источник геолокации.

2. **Яндекс.Карты** — геолокация открывается в Яндекс.Картах.

3. Событие отправляется через `ok9.f()` (критическое событие).

## Сводка

`geolocation_send_click {source_id, source_type}`. Открытие в Яндекс.Картах (`yandexmaps://`).
