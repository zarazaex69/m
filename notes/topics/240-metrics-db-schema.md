---
tags: [database, metrics, tracing, spans, performance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/crc.java
  - work/jadx_base/sources/defpackage/t54.java
related:
  - "[[216-local-db-tables]]"
  - "[[17-apptracer-uplink]]"
---

# metrics DB схема — трассировка производительности

`metrics` — таблица метрик производительности (distributed tracing).

## Поля

| Поле | Что |
|---|---|
| `traceId` | **ID трассировки** (PK) |
| `metricName` | имя метрики |
| `lastUpdatedTime` | время последнего обновления |
| `spanAndPropertiesDump` | **BLOB — дамп span и свойств** |
| `attempt` | попытка отправки |
| `isMarkedAsFailed` | помечена как неудачная |

## Что важно

1. **`spanAndPropertiesDump`** — BLOB с полным дампом span и свойств. Это distributed tracing данные (аналог OpenTelemetry).

2. **`attempt`** — счётчик попыток отправки. Метрики повторно отправляются при неудаче.

3. **`traceId`** — уникальный ID трассировки. Позволяет связывать события на клиенте и сервере.

## Сводка

`metrics`: traceId/metricName/lastUpdatedTime/spanAndPropertiesDump(BLOB)/attempt/isMarkedAsFailed. Distributed tracing с retry.
