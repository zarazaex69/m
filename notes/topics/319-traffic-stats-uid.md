---
tags: [surveillance, network-stats, traffic-stats, uid, battery-snapshot]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ktb.java
related:
  - "[[316-battery-snapshot]]"
  - "[[318-proc-filesystem-reads]]"
---

# TrafficStats — сбор сетевой статистики по UID

`ktb.java` — сбор сетевой статистики приложения через `TrafficStats`.

## Что собирается

| Метод | Что |
|---|---|
| `TrafficStats.getUidRxBytes(uid)` | **входящий трафик** приложения (bytes) |
| `TrafficStats.getUidTxBytes(uid)` | **исходящий трафик** приложения (bytes) |

## Структура (itb)

| Поле | Что |
|---|---|
| `mobile` | мобильный трафик (rx/tx/idle) |
| `wifi` | WiFi трафик (rx/tx/idle) |

## Что важно

1. **`getUidRxBytes`/`getUidTxBytes`** — трафик по UID приложения. Это точный трафик именно MAX, не всего устройства.

2. Данные используются в `BatterySnapshot.mobileNet`/`wifiNet` (см. [[316-battery-snapshot]]).

3. Также используется `HealthStats` API (Android 9+) для более детальной статистики.

## Сводка

`TrafficStats.getUidRxBytes/TxBytes(uid)` — входящий/исходящий трафик MAX по UID. Используется в BatterySnapshot.
