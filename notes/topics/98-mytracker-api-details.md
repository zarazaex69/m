---
tags: [mytracker, vk-analytics, telemetry, api, attribution]
status: confirmed
sources:
  - work/jadx_base/sources/com/my/tracker/core/TrackerConfig.java
  - work/jadx_base/sources/com/my/tracker/core/handlers/AttributionHandler.java
  - work/apktool_base/smali/com/my/tracker/core/TrackerConfig.smali
related:
  - "[[04-telemetry-endpoints]]"
  - "[[35-mytracker-antifraud-sensors]]"
  - "[[27-hardcoded-keys-audit]]"
---

# MyTracker API — tracker-api.vk-analytics.ru детали

В дополнение к [[04-telemetry-endpoints]] и [[35-mytracker-antifraud-sensors]] — детали MyTracker API.

## Endpoint

`tracker-api.vk-analytics.ru` — основной хост MyTracker. Поддерживает proxy-хост (configurable через `TrackerConfig.setProxyHost()`).

## API пути (из smali TrackerConfig)

| Путь | Что |
|---|---|
| `v3/` | версия 3 API |
| `mobile/v1` | мобильный API v1 |
| `mlapi` | ML API (антифрод, см. [[35-mytracker-antifraud-sensors]]) |
| `beta-ml` | beta ML API |
| `ip4` | IPv4-специфичный endpoint |
| `ts` | timestamp endpoint |

## Attribution

`AttributionHandler.java:43` — при обработке deeplink с `mt_deeplink` параметром клиент парсит URL `https://tracker-api.vk-analytics.ru/?<decoded_params>` и извлекает `mt_deeplink`. Это механизм attribution — отслеживание, откуда пришёл пользователь через deeplink.

## Что важно

1. **`mlapi` и `beta-ml`** — отдельные ML-endpoints для антифрод-системы. Сенсорные данные (5 датчиков из [[35-mytracker-antifraud-sensors]]) уходят на эти endpoints.

2. **`mobile/v1`** — основной мобильный API. Через него уходят attribution-данные, события, install referrer.

3. **Proxy-хост** — MyTracker поддерживает настройку proxy-хоста. Это означает, что трафик MyTracker может быть перенаправлен через произвольный прокси.

4. **`mt_deeplink`** — attribution через deeplink. Сервер знает, по какой ссылке пользователь установил приложение.

## Сводка

MyTracker использует `tracker-api.vk-analytics.ru` с путями `v3/`, `mobile/v1`, `mlapi`, `beta-ml`. Attribution через `mt_deeplink`. Поддерживает proxy-хост. ML-endpoints для антифрод-системы.
