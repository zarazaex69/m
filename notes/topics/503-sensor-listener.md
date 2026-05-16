---
tags: [sensors, proximity, light, surveillance, antifraud]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ioe.java
related:
  - "[[385-mytracker-details]]"
  - "[[394-audio-sdk-kws-mic-proximity]]"
---

# ioe — Sensor Listener (датчики для антифрода)

`ioe` — `SensorEventListener` для датчиков близости и освещённости.

## Датчики

| Тип | Константа | Что |
|---|---|---|
| `8` | `TYPE_PROXIMITY` | **датчик близости** |
| `5` | `TYPE_LIGHT` | **датчик освещённости** |

## Логика

### Proximity (тип 8)

```java
boolean z = sensorEvent.values[0] < Math.min(sensor.getMaximumRange(), 3.0d);
```

Если значение < min(maxRange, 3.0) → объект близко.

### Light (тип 5)

```java
boolean z2 = sensorEvent.values[0] < sensor.getMaximumRange();
```

Если значение < maxRange → освещённость ниже максимума.

## Что важно

1. **Proximity + Light** — два датчика для антифрода.

2. Используется в `koe` (ProximityHelper) для звонков.

3. Данные датчиков используются MyTracker для антифрода.

## Сводка

`ioe`: TYPE_PROXIMITY(8) + TYPE_LIGHT(5). Proximity: значение < min(maxRange, 3.0). Light: значение < maxRange.
