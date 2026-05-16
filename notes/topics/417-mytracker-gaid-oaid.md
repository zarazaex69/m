---
tags: [mytracker, gaid, oaid, huawei, google, advertising-id, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/com/my/tracker/core/o/w.java
  - work/jadx_base/sources/com/my/tracker/core/o/z.java
related:
  - "[[385-mytracker-details]]"
  - "[[404-advertising-id-client]]"
---

# MyTracker — GAID + OAID сбор

MyTracker собирает рекламные идентификаторы из двух источников.

## GoogleAdInfoDataProvider (w.java)

```
AdvertisingIdClient.getAdvertisingIdInfo(application)
  → Info.getId()           → GAID
  → isLimitAdTrackingEnabled() → флаг ограничения
```

Логирует: `"GoogleAdInfoDataProvider: AId: " + id`

## HuaweiAdInfoDataProvider (z.java)

```
com.huawei.hms.ads.identifier.AdvertisingIdClient.getAdvertisingIdInfo(application)
  → Info.getId()           → OAID
  → isLimitAdTrackingEnabled() → флаг ограничения
```

Логирует: `"HuaweiAdInfoDataProvider: oaid: " + id`

## Что важно

1. **Два источника** — Google GAID и Huawei OAID. Охватывает устройства без Google Play Services.

2. **`isLimitAdTrackingEnabled() ^ true`** — инвертированный флаг: `true` = трекинг разрешён.

3. При отсутствии HMS — `NoClassDefFoundError` перехватывается тихо.

## Сводка

`GoogleAdInfoDataProvider`: GAID + isLimitAdTrackingEnabled. `HuaweiAdInfoDataProvider`: OAID + isLimitAdTrackingEnabled. Оба передаются в MyTracker.
