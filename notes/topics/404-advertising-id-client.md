---
tags: [gaid, advertising-id, google-play-services, tracking, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/com/google/android/gms/ads/identifier/AdvertisingIdClient.java
related:
  - "[[04-telemetry-endpoints]]"
  - "[[385-mytracker-details]]"
---

# AdvertisingIdClient — Google Advertising ID (GAID)

`AdvertisingIdClient` — получение Google Advertising ID (GAID) через Google Play Services.

## API

| Метод | Что |
|---|---|
| `getAdvertisingIdInfo(context)` | получить GAID + флаг ограничения |
| `Info.getId()` | **GAID** (строка) |
| `Info.isLimitAdTrackingEnabled()` | ограничение рекламного трекинга |

## Что логируется при получении GAID

| Поле | Что |
|---|---|
| `limit_ad_tracking` | `"0"` или `"1"` |
| `ad_id_size` | длина GAID |
| `time_spent` | время получения (мс) |

## Что важно

1. **GAID** — уникальный рекламный идентификатор устройства. Используется MyTracker для трекинга.

2. **`isLimitAdTrackingEnabled()`** — если пользователь ограничил рекламный трекинг, это логируется.

3. **Singleton** — `AdvertisingIdClient` кэшируется как singleton.

4. Подключается к `com.google.android.gms.ads.identifier.service.START` через bind.

## Сводка

`AdvertisingIdClient.getAdvertisingIdInfo(ctx)` → `Info(id=GAID, isLimitAdTrackingEnabled)`. Используется MyTracker. Логирует `limit_ad_tracking`/`ad_id_size`/`time_spent`.
