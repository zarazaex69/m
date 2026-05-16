---
tags: [mytracker, preinstall, attribution, oem, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/com/my/tracker/applifecycle/o/e.java
related:
  - "[[35-mytracker-antifraud-sensors]]"
  - "[[98-mytracker-api-details]]"
  - "[[21-shared-prefs-map]]"
---

# MyTracker PreInstall — детекция предустановленных приложений

`com.my.tracker.applifecycle.o.e` — `PreInstallHandler`. Детектирует, было ли приложение предустановлено производителем устройства (OEM preinstall).

## Механизм

1. Проверяет `vendorAppPackage` — пакет вендорного приложения (задаётся через `TrackerConfig.getVendorAppPackage()`).
2. Если `preinstallRead=false` (ещё не проверяли) — читает ресурсы вендорного приложения через `PackageManager.getResourcesForApplication(vendorAppPackage)`.
3. Извлекает referrer из ресурсов вендорного приложения.
4. Устанавливает `preinstallRead=true` (однократная проверка).
5. Отправляет referrer на сервер.

## System properties

Дополнительно проверяет системные свойства:
- `ro.mytracker.preinstall.path` — путь к preinstall-файлу MyTracker
- `ro.appsflyer.preinstall.path` — путь к preinstall-файлу AppsFlyer (third-party)

## Что важно

1. **OEM preinstall detection** — MyTracker определяет, было ли MAX предустановлено производителем устройства. Это важно для attribution: если MAX предустановлен на устройстве, это другой канал распространения.

2. **`ro.mytracker.preinstall.path`** — системное свойство Android. Это означает, что производители устройств могут прописать путь к preinstall-файлу в системных свойствах. MyTracker читает этот файл и отправляет данные на сервер.

3. **`ro.appsflyer.preinstall.path`** — MyTracker также поддерживает AppsFlyer preinstall path. Это означает, что если устройство настроено для AppsFlyer, MyTracker тоже это детектирует.

4. **`preinstallRead` в SharedPreferences** — уже упомянуто в [[21-shared-prefs-map]]. Однократная проверка при первом запуске.

## Сводка

MyTracker детектирует OEM preinstall через ресурсы вендорного приложения и системные свойства `ro.mytracker.preinstall.path` / `ro.appsflyer.preinstall.path`. Referrer отправляется на `tracker-api.vk-analytics.ru`. Однократная проверка при первом запуске.
