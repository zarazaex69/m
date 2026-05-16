---
tags: [network-info, connectivity, vpn-detection, roaming, surveillance, telemetry]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/jf4.java
related:
  - "[[457-root-network-detection]]"
  - "[[458-network-operator-collection]]"
  - "[[508-connectivity-receiver]]"
---

# jf4 — ConnectionInfo (информация о сети)

`jf4` — полная информация о сетевом соединении.

## Методы

| Метод | Что |
|---|---|
| `a()` | `getRestrictBackgroundStatus() != 3` — фоновый трафик не ограничен |
| `b()` | тип сети (eg4) |
| `e()` | **VPN обнаружен** (`hasTransport(4)`) |
| `g()` | **есть соединение** |
| `h()` | **роуминг** (`isNetworkRoaming()`) |

## Определение типа сети (k())

| Условие | Тип |
|---|---|
| `hasTransport(1)` или `hasTransport(3)` | WIFI/ETHERNET |
| `hasTransport(0)` + subtype 1/2/... | CELLULAR (2G/3G/4G/5G) |
| `hasTransport(0)` + `getLinkDownstreamBandwidthKbps()` | CELLULAR с битрейтом |

## Что важно

1. **`e()` VPN detection** — через `hasTransport(4)`.

2. **`h()` roaming** — `TelephonyManager.isNetworkRoaming()`.

3. **`a()` background restriction** — `getRestrictBackgroundStatus() != 3`.

4. **`registerDefaultNetworkCallback`** — подписка на изменения сети.

5. **`RESTRICT_BACKGROUND_CHANGED`** — broadcast при изменении ограничений фонового трафика.

## Сводка

`jf4`: VPN(hasTransport(4)) / roaming(isNetworkRoaming()) / backgroundRestriction / registerDefaultNetworkCallback.
