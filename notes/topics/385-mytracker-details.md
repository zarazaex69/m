---
tags: [mytracker, telemetry, surveillance, anti-fraud, sensors, install-referrer, user-data]
status: confirmed
sources:
  - work/jadx_base/sources/com/my/tracker/MyTracker.java
  - work/jadx_base/sources/com/my/tracker/MyTrackerParams.java
  - work/jadx_base/sources/com/my/tracker/MyTrackerConfig.java
  - work/jadx_base/sources/com/my/tracker/MyTrackerAttribution.java
  - work/jadx_base/sources/com/my/tracker/config/AntiFraudConfig.java
  - work/jadx_base/sources/com/my/tracker/applifecycle/o/b.java
related:
  - "[[04-telemetry-endpoints]]"
  - "[[367-link-interceptor-mytracker]]"
  - "[[333-mytracker-install-referrer]]"
---

# MyTracker — детали SDK

`MyTracker` — трекер VK/OK.ru. Tracker ID: `34982109644049932883`.

## Модули (загружаются динамически)

| Модуль | Что |
|---|---|
| `MyTrackerAppLifecycle` | жизненный цикл приложения |
| `MyTrackerAds` | реклама |
| `MyTrackerAntifraud` | **антифрод** |
| `MyTrackerEnvironment` | окружение |
| `MyTrackerGame` | игровые события |
| `MyTrackerMiniApps` | мини-приложения |
| `MyTrackerPurchase` | покупки |
| `MyTrackerRemoteConfig` | удалённая конфигурация |
| `MyTrackerTimespent` | время использования |
| `MyTrackerUserLifecycle` | жизненный цикл пользователя |

## MyTrackerParams — пользовательские данные

| Параметр | Что |
|---|---|
| `age` | **возраст** |
| `gender` | **пол** (MALE=1/FEMALE=2/UNKNOWN=0/UNSPECIFIED=-1) |
| `email[]` | **email** |
| `phone[]` | **телефон** |
| `okId[]` | **OK ID** |
| `vkId[]` | **VK ID** |
| `vkConnectId[]` | **VK Connect ID** |
| `icqId[]` | **ICQ ID** |
| `customUserId[]` | произвольный ID |
| `customParam[key]` | произвольные параметры |
| `lang` | язык |

## MyTrackerConfig

| Параметр | Что |
|---|---|
| `locationTrackingMode` | NONE=0/CACHED=1/**ACTIVE=2** |
| `isTrackingEnvironmentEnabled` | отслеживание окружения |
| `isTrackingLaunchEnabled` | отслеживание запусков |
| `isTrackingPreinstallEnabled` | отслеживание предустановки |
| `isAutotrackingPurchaseEnabled` | автоотслеживание покупок |
| `isKidMode` | детский режим |
| `antiFraudConfig` | конфиг антифрода |
| `installedPackagesProvider` | **список установленных приложений** |
| `vendorAppPackage` | пакет вендора |
| `proxyHost` | прокси |

## AntiFraudConfig — датчики для антифрода

| Датчик | По умолчанию |
|---|---|
| `useLightSensor` | true |
| `useGyroscope` | true |
| `useMagneticFieldSensor` | true |
| `usePressureSensor` | true |
| `useProximitySensor` | true |

**Все датчики включены по умолчанию** для антифрода.

## Install Referrer

Поддерживается Huawei HMS Install Referrer (`HuaweiReferrerHandler`). Получает `installReferrer`, `installBeginTimestamp`, `installer`.

## Что важно

1. **`installedPackagesProvider`** — MyTracker может получать список установленных приложений.

2. **Все датчики включены** — гироскоп, магнитное поле, давление, освещённость, близость — для «антифрода».

3. **`locationTrackingMode=ACTIVE`** — активное отслеживание геолокации.

4. **10 модулей** — загружаются динамически. Включают `MyTrackerRemoteConfig` (удалённая конфигурация).

5. **`age`/`gender`/`email`/`phone`/`okId`/`vkId`/`vkConnectId`/`icqId`** — полный профиль пользователя передаётся в трекер.

## Сводка

`MyTracker`: 10 модулей (включая RemoteConfig/AntifraudConfig). `MyTrackerParams`: age/gender/email/phone/okId/vkId/vkConnectId/icqId. `AntiFraudConfig`: все датчики включены. `installedPackagesProvider`. Huawei HMS Install Referrer.
