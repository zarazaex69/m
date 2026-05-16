---
tags: [audio, server-control, pms, media]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ezd.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[03-pms-server-flags]]"
  - "[[42-voice-messages-opus-server-controlled]]"
---

# Audio playback PmsKey — server-controlled аудио-воспроизведение

## PmsKey

| Ключ | # | Default | Что |
|---|---|---|---|
| `audio-play-opus` | 10 | false | воспроизводить аудио через OPUS-декодер |
| `audio-download` | 6 | false | включить загрузку аудио |
| `audio-download-fallback` | 7 | **true** | fallback при ошибке загрузки аудио |
| `audio-prefetch` | 11 | false | предзагрузка аудио |
| `audio-play-cache-ttl` | (в qp6) | — | TTL кэша аудио |
| `audio-peaks-count` | (в qp6) | — | количество пиков для waveform |
| `calc-audio-wave` | (в qp6) | — | вычислять waveform аудио |
| `min-duration-save-audio-start-time` | (в rtd) | — | минимальная длительность для сохранения времени начала |
| `valid-interval-audio-start-time` | (в rtd) | — | валидный интервал времени начала |
| `min-duration-playback-speed` | (в rtd) | — | минимальная длительность для изменения скорости |

## Что важно

1. **`audio-download=false` по умолчанию** — загрузка аудио выключена по умолчанию. Сервер включает её через PmsKey.

2. **`audio-download-fallback=true`** — fallback включён по умолчанию. При ошибке загрузки — использовать fallback-метод.

3. **`audio-play-opus`** — server-gated переключение на OPUS-декодер для воспроизведения (в дополнение к `opus-recorder` для записи из [[42-voice-messages-opus-server-controlled]]).

4. **`audio-prefetch`** — server-gated предзагрузка аудио. При включении клиент загружает аудио заранее.

## Сводка

10 PmsKey управляют аудио-воспроизведением: загрузка, декодер, prefetch, waveform, кэш. Все server-controlled.
