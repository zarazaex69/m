---
tags: [telemetry, mytracker, proto, location, network, cell-info, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/com/my/tracker/core/proto/ExternalProtoWritersSetter.java
related:
  - "[[334-mytracker-event-types]]"
  - "[[330-mytracker-device-params]]"
---

# MyTracker ExternalProtoWriters — внешние данные в протоколе

`ExternalProtoWritersSetter` — интерфейс для добавления внешних данных в MyTracker протокол.

## Типы внешних данных

| Тип | Константа | Что |
|---|---|---|
| `REMOTE_CONFIG_STRING` | 1 | **удалённая конфигурация** |
| `LOCATION_INFO` | 2 | **геолокация** |
| `NETWORK_INFO` | 3 | **информация о сети** |
| `CELL_AND_WIFI_INFO` | 4 | **сотовая сеть и WiFi** |

## Что важно

1. **`LOCATION_INFO`** — геолокация передаётся в MyTracker. Это подтверждение сбора геолокации.

2. **`CELL_AND_WIFI_INFO`** — информация о сотовой сети и WiFi. Это оператор, тип сети, SSID и т.д.

3. **`NETWORK_INFO`** — общая информация о сети.

4. **`REMOTE_CONFIG_STRING`** — удалённая конфигурация. MyTracker может получать конфигурацию с сервера.

## Сводка

MyTracker ExternalProtoWriters: REMOTE_CONFIG_STRING(1)/LOCATION_INFO(2)/NETWORK_INFO(3)/CELL_AND_WIFI_INFO(4).
