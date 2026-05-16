---
tags: [telemetry, crash-report, device-info, session, rooted, background, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ij9.java
related:
  - "[[336-ilb-device-data]]"
  - "[[17-apptracer-uplink]]"
---

# Crash Report Device Info — структура данных устройства

`ij9.java` — парсер данных устройства для crash-репортов (Apptracer).

## Поля

| Поле | Что |
|---|---|
| `environment` | **среда** (prod/debug) |
| `buildUuid` | **UUID сборки** |
| `sessionUuid` | **UUID сессии** |
| `device` | устройство |
| `device_id` | **ID устройства** |
| `vendor` | **вендор** |
| `osVersion` | версия OS |
| `inBackground` | **в фоне** |
| `connection` | **тип соединения** |
| `isRooted` | **root-доступ** |
| `packageName` | имя пакета |

## Что важно

1. **`isRooted`** — флаг root-доступа. Apptracer знает, рутировано ли устройство.

2. **`inBackground`** — приложение в фоне. Apptracer знает, было ли приложение в фоне при краше.

3. **`sessionUuid`** — UUID сессии. Если null — генерируется случайный UUID.

4. **`connection`** — тип соединения при краше.

## Сводка

Crash report device info: environment/buildUuid/sessionUuid/device/device_id/vendor/osVersion/inBackground/connection/isRooted.
