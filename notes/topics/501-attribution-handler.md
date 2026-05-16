---
tags: [mytracker, attribution, referrer, deeplink, surveillance, telemetry]
status: confirmed
sources:
  - work/jadx_base/sources/com/my/tracker/core/handlers/AttributionHandler.java
related:
  - "[[385-mytracker-details]]"
  - "[[405-install-referrer]]"
  - "[[406-mytracker-user-lifecycle]]"
  - "[[500-mytracker-urls]]"
---

# AttributionHandler — атрибуция установки MyTracker

`AttributionHandler` — обработчик атрибуции установки.

## Методы

| Метод | Что |
|---|---|
| `handleReferrerAttribution(referrer)` | **обработка install referrer** |
| `handleServerAttribution(json)` | **обработка серверной атрибуции** |

## handleReferrerAttribution

1. Парсит referrer как URL: `https://tracker-api.vk-analytics.ru/?{referrer}`
2. Извлекает параметр `mt` — deeplink
3. Сохраняет в SharedPreferences `"attribution"`
4. Вызывает `AttributionListener.onReceiveAttribution(deeplink)`

## handleServerAttribution

1. Парсит JSON `{attribution: {...}}`
2. Извлекает deeplink
3. Сохраняет и вызывает listener

## Что важно

1. **`tracker-api.vk-analytics.ru`** — referrer парсится как URL этого домена.

2. **`mt` параметр** — deeplink извлекается из параметра `mt`.

3. **`attribution` SharedPreferences** — атрибуция сохраняется локально.

4. **Однократная атрибуция** — `AtomicBoolean` предотвращает повторную атрибуцию.

## Сводка

`AttributionHandler`: referrer → `https://tracker-api.vk-analytics.ru/?{referrer}` → `mt` параметр → deeplink → `attribution` SharedPreferences.
