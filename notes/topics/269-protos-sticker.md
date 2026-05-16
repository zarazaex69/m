---
tags: [protocol, sticker, protos, schema, animation]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Protos.java
related:
  - "[[206-protos-schemas]]"
  - "[[257-protos-attach-schema]]"
---

# Protos.Sticker — стикеры

`Protos.Sticker` — структура стикера.

## Типы стикеров

`STATIC`(1), `LIVE`(2), `POSTCARD`(3), `LOTTIE`(4), `UNKNOWN_TYPE`(0).

## Типы авторов

`SYSTEM`(1), `USER`(2), `UNKNOWN`(0).

## Поля

| Поле | Что |
|---|---|
| `stickerId` / `setId` | ID стикера/набора |
| `stickerType` | тип (STATIC/LIVE/POSTCARD/LOTTIE) |
| `authorType` | тип автора |
| `height` / `width` | размер |
| `audio` | **аудио-стикер** |
| `updateTime` | время обновления |

## AnimationProperties

| Поле | Что |
|---|---|
| `duration` | длительность |
| `fps` | кадры в секунду |
| `framesCount` | количество кадров |
| `replayDelay` | задержка повтора |

## Сводка

`Sticker`: stickerId/setId/stickerType(STATIC/LIVE/POSTCARD/LOTTIE)/authorType/audio. `AnimationProperties`: duration/fps/framesCount/replayDelay.
