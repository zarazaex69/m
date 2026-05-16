---
tags: [telemetry, device-info, build-info, cpu, network-operator, installer, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ilb.java
related:
  - "[[330-mytracker-device-params]]"
  - "[[04-telemetry-endpoints]]"
---

# ilb.java — расширенный сбор данных устройства

`ilb.java` — сбор расширенных данных об устройстве для телеметрии.

## Собираемые данные

| Поле | Что |
|---|---|
| `board` | **Build.BOARD** |
| `brand` | **Build.BRAND** |
| `cpuABI` | **Build.SUPPORTED_ABIS** (список ABI) |
| `device` | Build.DEVICE |
| `manufacturer` | Build.MANUFACTURER |
| `model` | Build.MODEL |
| `cpuCount` | **количество CPU ядер** |
| `osVersionSdkInt` | SDK версия |
| `osVersionRelease` | версия Android |
| `networkOperatorName` | **имя оператора сети** |
| `installer` | **имя установщика** |

## Что важно

1. **`cpuABI`** — список поддерживаемых ABI (arm64-v8a, armeabi-v7a и т.д.). Это детальная информация об архитектуре.

2. **`cpuCount`** — количество CPU ядер. Это характеристика устройства.

3. **`networkOperatorName`** — имя оператора сети (МТС, Мегафон и т.д.).

4. **`installer`** — имя установщика (Google Play, RuStore и т.д.).

## Сводка

`ilb.java`: board/brand/cpuABI/device/manufacturer/model/cpuCount/osVersion/networkOperatorName/installer.
