---
tags: [telemetry, mytracker, device-params, screen, timezone, free-space, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/com/my/tracker/core/o/m.java
related:
  - "[[329-mytracker-advertising-ids]]"
  - "[[328-mytracker-init]]"
---

# MyTracker DeviceParamsDataProvider — параметры устройства

`DeviceParamsDataProvider` (`m.java`) — сбор параметров устройства для MyTracker.

## Собираемые данные

| Параметр | Что |
|---|---|
| `Build.DEVICE` | устройство |
| `Build.MANUFACTURER` | производитель |
| `Build.MODEL` | модель |
| `Build.VERSION.RELEASE` | версия Android |
| `Build.VERSION.SDK_INT` | SDK версия |
| `packageName` | имя пакета |
| `versionName` / `versionCode` | версия приложения |
| `language` (app) | язык приложения |
| `language` (system) | системный язык |
| `timezone` | **часовой пояс** (displayName + ID) |
| `screenWidth` / `screenHeight` | разрешение экрана |
| `densityDpi` / `density` / `xdpi` | плотность пикселей |
| `totalSpace` / `freeSpace` | **дисковое пространство** |
| `hasSystemFeature("android.hardware.touchscreen")` | **тачскрин** |
| `currentModeType` | **режим UI** (телефон/планшет/TV) |

## Что важно

1. **`timezone`** — часовой пояс с полным именем и ID. Сервер знает часовой пояс пользователя.

2. **`freeSpace`** — свободное место на диске. Сервер знает, сколько места на устройстве.

3. **`currentModeType`** — режим UI (телефон/планшет/TV). Сервер знает тип устройства.

4. **`hasSystemFeature("android.hardware.touchscreen")`** — есть ли тачскрин.

## Сводка

MyTracker DeviceParams: Build.DEVICE/MANUFACTURER/MODEL/VERSION + screen(width/height/density) + timezone + freeSpace + currentModeType.
