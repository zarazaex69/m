---
tags: [dps, apptracer, telephony, network-operator, surveillance, calls-analytics]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/zu5.java
related:
  - "[[04-telemetry-endpoints]]"
  - "[[362-z8f-dps-metadata]]"
  - "[[375-call-analytics-sender]]"
---

# zu5 — DPS (Apptracer) клиент

`zu5` — клиент DPS (Apptracer). Идентифицирован через z8f-декодер: `"DPS"`.

## Конфигурация (zu5.a)

| Поле | Что |
|---|---|
| `apiKey` | **API ключ DPS** |
| `application` | Application |
| `deviceIdSupplier` | **поставщик Device ID** |
| `userIdSupplier` | **поставщик User ID** |
| `clientVersion` | версия клиента |
| `httpClient` | HTTP клиент |
| `executorService` | пул потоков (4 потока) |
| `tlsCheckEnabled` | проверка TLS |
| `foregroundDetectionEnabled` | обнаружение foreground |

## Сбор данных сети

```java
TelephonyManager telephonyManager = ...;
mrfVar = telephonyManager.getNetworkOperator() + ':' + telephonyManager.getNetworkOperatorName();
```

**`getNetworkOperator()`** + **`getNetworkOperatorName()`** — MCC+MNC и имя оператора. Собирается при каждой отправке данных.

## Что важно

1. **`getNetworkOperator()`/`getNetworkOperatorName()`** — DPS собирает MCC+MNC и имя оператора сети.

2. **`deviceIdSupplier`** — DPS использует Device ID.

3. **`userIdSupplier`** — DPS использует User ID.

4. **4 потока** — пул из 4 потоков для отправки данных.

5. **`"DPS"`** — идентифицирован через z8f-декодер.

## Сводка

`zu5` (DPS): `getNetworkOperator()+getNetworkOperatorName()` при каждой отправке. `deviceIdSupplier`/`userIdSupplier`. 4 потока.
