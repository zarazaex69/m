---
tags: [mytracker, antiFraud, sensors, installed-apps, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/com/my/tracker/MyTrackerConfig.java
  - work/jadx_base/sources/com/my/tracker/config/AntiFraudConfig.java
  - work/jadx_base/sources/com/my/tracker/core/o/f.java
related:
  - "[[04-telemetry-endpoints]]"
  - "[[231-mytracker-params]]"
  - "[[47-mytracker-antifraud]]"
---

# MyTracker AntiFraud — сенсоры и установленные приложения

## AntiFraudConfig — сенсоры

| Сенсор | По умолчанию |
|---|---|
| `useLightSensor` | если доступен |
| `useGyroscope` | true |
| `useMagneticFieldSensor` | true |
| `usePressureSensor` | true |
| `useProximitySensor` | если доступен |

## MyTrackerConfig — дополнительные параметры

| Параметр | Что |
|---|---|
| `getAntiFraudConfig()` | конфигурация AntiFraud |
| `getApkPreinstallParams()` | параметры предустановки APK |
| `getLocationTrackingMode()` | режим отслеживания геолокации |
| `getVendorAppPackage()` | пакет вендорского приложения |
| `getBufferingPeriod()` / `getForcingPeriod()` | периоды буферизации/принудительной отправки |
| `InstalledPackagesProvider.getInstalledPackages()` | **список установленных приложений** |

## InstalledPackagesProvider

`MyTrackerConfig.InstalledPackagesProvider` — интерфейс для получения списка установленных приложений. MyTracker может получить полный список пакетов на устройстве.

## Что важно

1. **`getInstalledPackages()`** — MyTracker может читать список установленных приложений. Это передаётся на `tracker-api.vk-analytics.ru`.

2. **5 сенсоров** для AntiFraud: гироскоп, магнитное поле, давление, освещённость, proximity. Уже покрыто в [[47-mytracker-antifraud]].

3. **`getApkPreinstallParams()`** — параметры предустановки. Это для устройств с предустановленным MAX.

## Сводка

`AntiFraudConfig`: 5 сенсоров (гироскоп/магнитное поле/давление/освещённость/proximity). `InstalledPackagesProvider`: список установленных приложений. `getApkPreinstallParams`: параметры предустановки.
