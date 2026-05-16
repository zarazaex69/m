---
tags: [location, gps, geolocation, surveillance, protos, live-location]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Protos.java
related:
  - "[[346-fused-location-provider]]"
  - "[[19-fcm-push-payload]]"
  - "[[345-exif-gps-metadata]]"
---

# Protos.Location + Protos.LocationInfo — геолокация в протоколе

## Protos.Location — сообщение с геолокацией

| Поле | Тип | Что |
|---|---|---|
| `latitude` | double | **широта** |
| `longitude` | double | **долгота** |
| `altitude` | double | высота |
| `accuracy` | float | точность |
| `bearing` | float | направление |
| `livePeriod` | long | **период live-трансляции** (мс) |
| `endTime` | long | время окончания |
| `deviceId` | String | **ID устройства** |
| `corrupted` | boolean | повреждённые данные |

## Protos.LocationInfo — информация о геолокации

| Поле | Тип | Что |
|---|---|---|
| `latitude` | double | **широта** |
| `longitude` | double | **долгота** |
| `altitude` | double | высота |
| `accuracy` | float | точность |
| `bearing` | float | направление |
| `speed` | float | **скорость** |
| `time` | long | timestamp |

## Что важно

1. **`livePeriod`** — live-трансляция геолокации. Пользователь может транслировать своё местоположение в реальном времени.

2. **`deviceId`** в Location — ID устройства передаётся вместе с геолокацией.

3. **`speed`** в LocationInfo — скорость движения пользователя.

4. **`bearing`** — направление движения.

5. Связано с FCM push `LocationRequest` (см. [[19-fcm-push-payload]]) — сервер может запросить геолокацию через push.

## Сводка

`Protos.Location`: latitude/longitude/altitude/accuracy/bearing/livePeriod/endTime/deviceId. `Protos.LocationInfo`: latitude/longitude/altitude/accuracy/bearing/speed/time.
