---
tags: [telemetry, mytracker, endpoints, vk-analytics, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/com/my/tracker/core/TrackerConfig.java
  - work/jadx_base/sources/com/my/tracker/core/handlers/AttributionHandler.java
related:
  - "[[04-telemetry-endpoints]]"
  - "[[231-mytracker-params]]"
  - "[[232-mytracker-config-antifraud]]"
---

# MyTracker — все endpoints

`tracker-api.vk-analytics.ru` — основной хост MyTracker.

## Endpoints

| URL | Что |
|---|---|
| `https://tracker-api.vk-analytics.ru/v3/` | **основной API** |
| `https://ip4.tracker-api.vk-analytics.ru/` | **IP-адрес** (IPv4) |
| `https://ts.tracker-api.vk-analytics.ru/mobile/v1` | **timestamp** |
| `https://mlapi.tracker-api.vk-analytics.ru/` | **ML API** |
| `https://beta-ml.tracker-api.vk-analytics.ru/` | **Beta ML API** |

## Attribution

`https://tracker-api.vk-analytics.ru/?...` — attribution URL с параметром `mt_deeplink`.

## Что важно

1. **`/v3/`** — основной API. Все события отправляются сюда.

2. **`ip4.`** — отдельный субдомен для IPv4. Это для обхода IPv6-проблем.

3. **`ts.`** — timestamp сервер. Синхронизация времени.

4. **`mlapi.`/`beta-ml.`** — ML API. MyTracker использует ML для антифрода.

5. **`mt_deeplink`** — параметр attribution deeplink.

## Сводка

MyTracker endpoints: `tracker-api.vk-analytics.ru/v3/`(main) + `ip4.`(IPv4) + `ts./mobile/v1`(timestamp) + `mlapi.`(ML) + `beta-ml.`(Beta ML).
