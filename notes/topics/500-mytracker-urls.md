---
tags: [mytracker, tracker-urls, vk-analytics, surveillance, telemetry]
status: confirmed
sources:
  - work/jadx_base/sources/com/my/tracker/core/TrackerConfig.java
  - work/jadx_base/sources/com/my/tracker/core/o/t0.java
related:
  - "[[385-mytracker-details]]"
  - "[[417-mytracker-gaid-oaid]]"
  - "[[418-mytracker-installed-packages]]"
---

# TrackerConfig — URL-адреса MyTracker

`TrackerConfig` — конфигурация MyTracker. Базовый хост: `tracker-api.vk-analytics.ru`.

## URL-адреса (по умолчанию)

| Поле | URL | Что |
|---|---|---|
| `t` (trackerUrl) | `https://tracker-api.vk-analytics.ru/v3/` | **основной трекер** |
| `u` (ipv4TrackerUrl) | `https://ip4.tracker-api.vk-analytics.ru/` | **IPv4 трекер** |
| `x` (timeSpentUrl) | `https://ts.tracker-api.vk-analytics.ru/mobile/v1` | **time spent** |
| `v` (mlProdTrackerUrl) | `https://mlapi.tracker-api.vk-analytics.ru/` | **ML production** |
| `w` (mlBetaTrackerUrl) | `https://beta-ml.tracker-api.vk-analytics.ru/` | **ML beta** |

## Кастомный хост

`setProxyHost(host)` — можно задать кастомный хост. По умолчанию: `tracker-api.vk-analytics.ru`.

## Что важно

1. **5 URL-адресов** — основной трекер, IPv4, time spent, ML prod, ML beta.

2. **`tracker-api.vk-analytics.ru`** — домен VK Analytics.

3. **`mlapi`/`beta-ml`** — ML-эндпоинты для антифрода.

4. **`setProxyHost`** — хост можно изменить программно.

## Сводка

`TrackerConfig`: 5 URL на `tracker-api.vk-analytics.ru`: v3/(основной)/ip4/(IPv4)/ts/mobile/v1(time spent)/mlapi/(ML prod)/beta-ml/(ML beta).
