---
tags: [surveillance, health-stats, network-stats, uid, battery-snapshot]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ktb.java
related:
  - "[[319-traffic-stats-uid]]"
  - "[[316-battery-snapshot]]"
---

# HealthStats API — детальная сетевая статистика

`SystemHealthManager.takeMyUidSnapshot()` — детальная статистика по UID через HealthStats API (Android 9+).

## Измерения (HealthStatsKeys)

| Ключ | Что |
|---|---|
| `10048` | мобильный RX bytes |
| `10049` | мобильный TX bytes |
| `10024` | мобильный idle |
| `10050` | WiFi RX bytes |
| `10051` | WiFi TX bytes |
| `10016` | WiFi idle |

## Fallback

Если HealthStats недоступен → fallback на `TrafficStats.getUidRxBytes/TxBytes` (см. [[319-traffic-stats-uid]]).

## Что важно

1. **`takeMyUidSnapshot()`** — снимок статистики именно для UID приложения. Более точный, чем TrafficStats.

2. **Idle time** — время простоя сети. Это дополнительная метрика.

3. Данные используются в `BatterySnapshot.mobileNet`/`wifiNet`.

## Сводка

`HealthStats.takeMyUidSnapshot()`: mobile(rx/tx/idle)/wifi(rx/tx/idle). Fallback на TrafficStats при недоступности.
