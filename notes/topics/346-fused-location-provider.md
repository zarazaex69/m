---
tags: [surveillance, location, gps, fused-location, google-play-services]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/bu7.java
related:
  - "[[203-live-location]]"
  - "[[345-exif-gps-metadata]]"
---

# Fused Location Provider — получение GPS через Google Play Services

`bu7.java` — получение геолокации через Google Fused Location Provider.

## Данные

```java
Location location = (Location) task.f();
sj9 sj9Var = new sj9(
    location.getLatitude(),   // широта
    location.getLongitude(),  // долгота
    location.getAltitude(),   // высота
    location.getAccuracy(),   // точность
    location.getBearing(),    // направление
    location.getSpeed()       // скорость
);
```

## Что важно

1. **Все 6 параметров** — широта, долгота, высота, точность, направление, скорость. Это полный набор данных геолокации.

2. Используется `com.google.android.gms.tasks.Task` — Google Play Services Fused Location Provider.

3. Результат передаётся в `sj9` (LocationData) — та же структура, что и в [[203-live-location]].

## Сводка

Fused Location: latitude/longitude/altitude/accuracy/bearing/speed → sj9(LocationData).
