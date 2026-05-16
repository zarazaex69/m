---
tags: [sim-operator, mcc-mnc, fingerprint, timezone, network-type, surveillance, telemetry]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/yk2.java
related:
  - "[[458-network-operator-collection]]"
  - "[[416-device-info-vm5-cgj]]"
  - "[[04-telemetry-endpoints]]"
---

# yk2 — сбор данных устройства для телеметрии

`yk2` — сбор данных устройства при каждом запросе к телеметрии.

## Собираемые данные

| Поле | Что |
|---|---|
| `sdk-version` | `Build.VERSION.SDK_INT` |
| `fingerprint` | **`Build.FINGERPRINT`** |
| `tz-offset` | **смещение часового пояса** (секунды) |
| `net-type` | тип сети |
| `mobile-subtype` | подтип мобильной сети |
| `mcc_mnc` | **MCC+MNC** (код страны + код оператора) |
| `application_build` | номер сборки приложения |

## Что важно

1. **`Build.FINGERPRINT`** — уникальный fingerprint сборки Android. Позволяет точно идентифицировать устройство.

2. **`mcc_mnc`** — `getSimOperator()` → MCC+MNC. Код страны и оператора SIM-карты.

3. **`tz-offset`** — смещение часового пояса в секундах.

4. **`net-type`/`mobile-subtype`** — тип и подтип сети.

## Сводка

`yk2`: `Build.FINGERPRINT` + `mcc_mnc(getSimOperator())` + `tz-offset` + `sdk-version` + `net-type`/`mobile-subtype` + `application_build`.
