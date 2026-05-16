---
tags: [deeplinks, mytracker, attribution, link-interceptor, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/android/deeplink/LinkInterceptorActivity.java
related:
  - "[[355-deeplink-routes-full]]"
  - "[[333-mytracker-install-referrer]]"
---

# LinkInterceptorActivity — перехват deeplink-ов с MyTracker

`LinkInterceptorActivity` — Activity для перехвата deeplink-ов. Интегрирована с MyTracker.

## Логика

1. Получает URI из Intent
2. **`MyTracker.handleDeeplink(intent)`** — передаёт deeplink в MyTracker для атрибуции
3. Парсирует результат как URI
4. Передаёт в `LinkInterceptorWidget`

## Что важно

1. **`MyTracker.handleDeeplink`** — каждый deeplink проходит через MyTracker. Это означает, что MyTracker знает все deeplink-переходы пользователя.

2. **`queryParameterNames`** — параметры deeplink-а обрабатываются. Это может включать UTM-параметры.

3. Логируется URI до и после обработки MyTracker.

## Сводка

`LinkInterceptorActivity`: URI → `MyTracker.handleDeeplink(intent)` → парсинг → `LinkInterceptorWidget`. Все deeplink-и проходят через MyTracker.
