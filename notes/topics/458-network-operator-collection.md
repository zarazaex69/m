---
tags: [network-operator, telephony, vpn-detection, connection-type, surveillance, telemetry]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/f58.java
related:
  - "[[415-dps-client-zu5]]"
  - "[[457-root-network-detection]]"
  - "[[356-api-oneme-ru-servers]]"
---

# f58 — сбор данных оператора и типа соединения

`f58` — сбор данных о сети при каждом запросе к API.

## Собираемые данные

```java
ws9Var.put("connection_type", bf4Var.g() ? bf4Var.b().a : 1);
ws9Var.put("vpn", 1);  // если VPN
telephonyManager.getNetworkOperator() + ":" + telephonyManager.getNetworkOperatorName()
```

## Серверы

```java
List.of("api.oneme.ru", j58.b, j58.f, j58.h, j58.d, ...)
```

Список серверов для запросов.

## Что важно

1. **`connection_type`** — тип соединения передаётся в каждом запросе.

2. **`vpn`** — флаг VPN передаётся в каждом запросе.

3. **`getNetworkOperator() + ":" + getNetworkOperatorName()`** — MCC+MNC и имя оператора.

4. Данные передаются в запросах к `api.oneme.ru` и другим серверам.

## Сводка

`f58`: `connection_type`/`vpn` + `getNetworkOperator():getNetworkOperatorName()` в каждом запросе к API.
