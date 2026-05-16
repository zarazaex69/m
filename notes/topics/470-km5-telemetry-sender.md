---
tags: [telemetry, km5, event-sender, surveillance, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/km5.java
related:
  - "[[467-telemetry-event-types]]"
  - "[[465-devnull-server-config]]"
  - "[[468-startup-report]]"
---

# km5 — Telemetry Event Sender

`km5` — отправщик событий телеметрии. Отправляет события типа `jm5` на сервер.

## Структура события

```java
km5.a(km5Var, jm5Var, f, f2, ..., str, str2, ..., i, i2)
```

Поля события:

| Поле | Что |
|---|---|
| `value` | float значение 1 |
| `value2`..`value16` | float значения 2-16 |
| `valueStr`..`valueStr6` | строковые значения 1-6 |
| `valueStr16` | строковое значение 16 |

## Логика

1. Проверяет `DevNullServerConfig.a(eventType)` — включён ли тип события
2. Если включён → отправляет через `ok9.h(analytics, "DEV", eventType, data, i3)`

## Что важно

1. **16 float значений** — каждое событие может содержать до 16 числовых значений.

2. **6 строковых значений** — каждое событие может содержать до 6 строковых значений.

3. **`"DEV"`** — события отправляются в канал `"DEV"`.

4. Дедупликация через хэш-таблицу (bloom filter).

## Сводка

`km5.a(type, value1..16, valueStr1..6)` → `ok9.h("DEV", eventType, data)`. Дедупликация через bloom filter.
