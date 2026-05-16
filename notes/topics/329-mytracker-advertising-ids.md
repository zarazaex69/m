---
tags: [telemetry, mytracker, advertising-id, gaid, huawei-hms, device-id, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/com/my/tracker/core/o/w.java
  - work/jadx_base/sources/com/my/tracker/core/o/z.java
  - work/jadx_base/sources/com/my/tracker/core/proto/a.java
related:
  - "[[328-mytracker-init]]"
  - "[[232-mytracker-config-antifraud]]"
---

# MyTracker — сбор рекламных идентификаторов

MyTracker собирает рекламные идентификаторы через два SDK.

## Google GAID

```java
// w.java
AdvertisingIdClient.getAdvertisingIdInfo(application)
```

`com.google.android.gms.ads.identifier.AdvertisingIdClient` — Google Advertising ID (GAID).

## Huawei OAID

```java
// z.java
AdvertisingIdClient.getAdvertisingIdInfo(application)
```

`com.huawei.hms.ads.identifier.AdvertisingIdClient` — Huawei Open Advertising ID (OAID).

## Дополнительные идентификаторы (proto/a.java)

| Идентификатор | Что |
|---|---|
| `android_id` | **Android ID** |
| `mac` | **MAC-адрес** |

## Что важно

1. **Два рекламных ID** — Google GAID и Huawei OAID. MAX поддерживает оба.

2. **`android_id`** — уникальный идентификатор устройства Android.

3. **`mac`** — MAC-адрес. На Android 10+ возвращает рандомизированный MAC.

## Сводка

MyTracker собирает: Google GAID + Huawei OAID + android_id + mac.
