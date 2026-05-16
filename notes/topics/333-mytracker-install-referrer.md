---
tags: [telemetry, mytracker, install-referrer, huawei, google-play, attribution, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/com/my/tracker/applifecycle/o/b.java
  - work/jadx_base/sources/com/my/tracker/applifecycle/o/a.java
related:
  - "[[209-install-referrer]]"
  - "[[329-mytracker-advertising-ids]]"
---

# MyTracker Install Referrer — источник установки

MyTracker собирает данные об источнике установки через два SDK.

## Huawei HMS Install Referrer (b.java)

| Данные | Что |
|---|---|
| `getInstallReferrer()` | **строка referrer** |
| `getInstallBeginTimestampSeconds()` | **время начала установки** |
| `getReferrerClickTimestampSeconds()` | **время клика на рекламу** |
| `getInstaller()` | **имя установщика** |

## Google Play Install Referrer (a.java)

Аналогичные данные через Google Play Install Referrer API.

## Что важно

1. **`installReferrer`** — строка referrer из рекламной кампании (UTM-параметры и т.д.).

2. **`installBeginTimestampSeconds`** — точное время начала установки.

3. **`referrerClickTimestampSeconds`** — время клика на рекламу. Это данные для атрибуции рекламных кампаний.

4. Поддерживаются оба: Google Play и Huawei AppGallery.

## Сводка

MyTracker Install Referrer: installReferrer/installBeginTimestamp/referrerClickTimestamp/installer. Google + Huawei.
