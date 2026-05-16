---
tags: [network-type, connectivity, surveillance, telemetry, broadcast-receiver]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/gr.java
related:
  - "[[457-root-network-detection]]"
  - "[[458-network-operator-collection]]"
---

# gr — Connectivity BroadcastReceiver (тип сети)

`gr` — `BroadcastReceiver` для `android.net.conn.CONNECTIVITY_CHANGE`.

## Определение типа сети

| NetworkInfo.type | Subtype | Результат |
|---|---|---|
| 0 (MOBILE) | 20 | 3 (HSPA+) |
| 0 (MOBILE) | другие | по subtype |
| 1 (WIFI) | — | WIFI |
| 4/5 | — | WiMAX/Bluetooth |
| 6 | — | другой |
| 9 | — | 7 |
| другой | — | 8 |

## Дополнительно

- `AUDIO_BECOMING_NOISY` — наушники отключены
- `BATTERY_CHANGED` — уровень батареи
- `TIMEZONE_CHANGED` — изменение часового пояса
- `MEDIA_BUTTON` — кнопка медиа

## Что важно

1. **`CONNECTIVITY_CHANGE`** — изменение типа сети отслеживается.

2. **`TelephonyManager`** — при мобильной сети дополнительно запрашивается TelephonyManager.

3. **`BATTERY_CHANGED`** — уровень батареи отслеживается через broadcast.

## Сводка

`gr`: CONNECTIVITY_CHANGE → тип сети (MOBILE/WIFI/...). BATTERY_CHANGED → уровень батареи. TIMEZONE_CHANGED.
