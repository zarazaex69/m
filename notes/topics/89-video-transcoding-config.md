---
tags: [media, video, server-control, pms, transcoding]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/e2h.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[03-pms-server-flags]]"
  - "[[39-libffmpg-version-license]]"
---

# video-transcoding-class и one-video-uploader-config — серверный контроль видео

## video-transcoding-class (#302)

`PmsKey.f302videotranscodingclass` — JSON-массив, default `[2,3]`. Описание из `e2h.java`:
- «Данные указывать в формате JsonArray [1,2,3]»
- `1` = low
- `2` = average
- `3` = high

Это **server-controlled список классов транскодирования видео**. Сервер задаёт, какие классы качества доступны для транскодирования видео. Default `[2,3]` = average + high.

## one-video-uploader-config

`PmsKey.f215onevideouploaderconfig` — JSON-объект, default `{"audio": 0, "video": 0}`. Описание:
- «Данные указывать в формате JsonObject { "audio": 0|1|2, "video": 0|1|2 }»

Это конфиг загрузчика видео. Значения 0/1/2 — вероятно, режимы загрузки (0=стандартный, 1=ускоренный, 2=другой).

## Что важно

1. **`video-transcoding-class`** — сервер контролирует, в каком качестве транскодируются видео. Если сервер уберёт `3` (high) из списка, пользователи не смогут отправлять видео в высоком качестве.

2. **`one-video-uploader-config`** — сервер контролирует режим загрузки видео. Это влияет на скорость и надёжность загрузки.

3. В сочетании с `libffmpg.so` (FFmpeg n4.4.3, см. [[39-libffmpg-version-license]]) — транскодирование происходит через FFmpeg, параметры которого задаются сервером.

## Сводка

`video-transcoding-class` — server-pushed список доступных классов качества видео (low/average/high). `one-video-uploader-config` — server-pushed режим загрузчика видео. Сервер контролирует качество и режим загрузки видеоконтента.
