---
tags: [protocol, video, protos, schema, transcription, live-stream]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Protos.java
related:
  - "[[206-protos-schemas]]"
  - "[[257-protos-attach-schema]]"
---

# Protos.Video + Quality — видео в сообщениях

## Protos.Video

| Поле | Что |
|---|---|
| `videoId` | ID видео |
| `duration` | длительность |
| `height` / `width` (в Quality) | разрешение |
| `live` | **live-стрим** |
| `startTime` | время начала |
| `previewData` | данные превью |
| `thumbhashData` | ThumbHash |
| `transcriptionStatus` | **статус транскрипции** |
| `audioGroupIndex` / `audioTrackIndex` | аудио-дорожка |
| `ignoreAutoplay` | игнорировать автовоспроизведение |
| `isThumbnailInCache` | превью в кэше |

## Quality (вложенная в Video)

| Поле | Что |
|---|---|
| `bitrate` | битрейт |
| `height` / `width` | разрешение |
| `ordinal` | порядковый номер |
| `isOriginal` | оригинальное качество |

## VideoCollage

| Поле | Что |
|---|---|
| `count` | количество видео в коллаже |

## Что важно

1. **`transcriptionStatus`** — статус транскрипции видео-сообщения. Связано с `enable-video-messages-transcription` (см. [[246-ws-session-fingerprint-extended]]).

2. **`live`** — флаг live-стрима. Связано с `liveStreamsEnabled` PmsKey.

3. **`thumbhashData`** — ThumbHash для placeholder видео.

## Сводка

`Protos.Video`: videoId/duration/live/startTime/transcriptionStatus/thumbhashData. `Quality`: bitrate/height/width/isOriginal.
