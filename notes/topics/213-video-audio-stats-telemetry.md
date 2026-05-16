---
tags: [telemetry, video, audio, media-stats, critlog, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/qjj.java
  - work/jadx_base/sources/defpackage/wb0.java
  - work/jadx_base/sources/defpackage/vb0.java
related:
  - "[[207-critlog-events]]"
---

# VIDEO_STATS + AUDIO_STATS — телеметрия воспроизведения медиа

## VIDEO_STATS — события воспроизведения видео

| Событие | Что |
|---|---|
| `action_play` | начало воспроизведения |
| `first_bytes` | получены первые байты |
| `content_error` | ошибка контента |
| `close_at_empty_buffer` | закрытие при пустом буфере |
| `empty_buffer` | пустой буфер |

### Базовые параметры каждого события

| Поле | Что |
|---|---|
| `at` | тип контента |
| `cached_data` | кэшированные данные |
| `vsid` | ID сессии воспроизведения |
| `vid` | ID видео |
| `cdn_host` | **CDN хост** |
| `ct` | тип контента |
| `place` | место воспроизведения |
| `connection_type` | тип соединения |
| `quality` | качество видео |

## AUDIO_STATS — события воспроизведения аудио

| Событие | Что |
|---|---|
| `first_bytes` | получены первые байты |

## Что важно

1. **`cdn_host`** — сервер знает, с какого CDN-хоста загружается видео.

2. **`vsid`** — ID сессии воспроизведения. Позволяет отслеживать полный lifecycle воспроизведения.

3. **`quality`** — качество видео логируется при каждом событии.

4. **`connection_type`** — тип соединения (WiFi/мобильная) при воспроизведении.

## Сводка

`VIDEO_STATS`: action_play/first_bytes/content_error/close_at_empty_buffer/empty_buffer + vid/vsid/cdn_host/quality/connection_type. `AUDIO_STATS`: first_bytes.
