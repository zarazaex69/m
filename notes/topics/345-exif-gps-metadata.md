---
tags: [surveillance, exif, gps, photo-metadata, location]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/od6.java
  - work/jadx_base/sources/defpackage/jd6.java
related:
  - "[[203-live-location]]"
  - "[[23-camera-mic-screen-entry-points]]"
---

# EXIF данные — GPS и метаданные фото

MAX читает EXIF-данные из фотографий, включая GPS-координаты.

## GPS EXIF поля

| Поле | Что |
|---|---|
| `GPSLatitude` / `GPSLatitudeRef` | **широта** |
| `GPSLongitude` / `GPSLongitudeRef` | **долгота** |
| `GPSAltitude` / `GPSAltitudeRef` | **высота** |
| `GPSTimeStamp` | **время GPS** |
| `GPSSpeedRef` | скорость |
| `GPSTrackRef` | направление движения |
| `GPSImgDirectionRef` | направление съёмки |
| `GPSDestBearingRef` | направление к цели |
| `GPSDestDistanceRef` | расстояние до цели |

## Другие EXIF поля

`DateTimeOriginal`, `Make`, `Model`, `Software`, `CameraOwnerName`, `BodySerialNumber`, `LensModel`, `LensSerialNumber` и 100+ других.

## Что важно

1. **GPS координаты** из EXIF фотографий. При отправке фото в чат MAX может читать GPS из EXIF.

2. **`CameraOwnerName`/`BodySerialNumber`** — имя владельца камеры и серийный номер.

3. **`LensSerialNumber`** — серийный номер объектива.

## Сводка

EXIF: GPSLatitude/GPSLongitude/GPSAltitude/GPSTimeStamp + CameraOwnerName/BodySerialNumber/LensSerialNumber.
