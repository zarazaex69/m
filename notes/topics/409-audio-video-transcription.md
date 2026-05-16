---
tags: [transcription, audio-messages, video-messages, asr, surveillance, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Protos.java
  - work/jadx_base/sources/ru/ok/tamtam/messages/c.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[376-asr-online-manager]]"
  - "[[399-asr-manager-record]]"
  - "[[408-pmskey-debug-logging-transcription]]"
  - "[[15-on-device-asr-kws-diarization]]"
---

# Транскрипция аудио/видео сообщений

MAX поддерживает транскрипцию аудио и видео сообщений. Управляется сервером.

## Protos.java — поля транскрипции

### Protos.Audio

| Поле | Что |
|---|---|
| `transcription` | **текст транскрипции** аудиосообщения |
| `transcriptionStatus` | статус транскрипции |

### Protos.Video

| Поле | Что |
|---|---|
| `transcription` | **текст транскрипции** видеосообщения |
| `transcriptionStatus` | статус транскрипции |

## Управление (PmsKey)

| PmsKey | Что |
|---|---|
| `enable-audio-messages-transcription` | включить транскрипцию аудиосообщений |
| `enable-video-messages-transcription` | включить транскрипцию видеосообщений |
| `retry-transcribe-attempt` | количество попыток |
| `retry-transcribe-timeout` | таймаут повтора |

## SharedPreferences

`audio.transcription.enabled` (default: `true`) — транскрипция аудио включена по умолчанию.

## Что важно

1. **Транскрипция включена по умолчанию** (`audio.transcription.enabled = true`). Все аудиосообщения транскрибируются.

2. **`transcription` в Protos.Audio/Video** — текст транскрипции хранится в протобуф-объекте сообщения. Это означает, что транскрипция синхронизируется с сервером.

3. **`transcriptionStatus`** — статус транскрипции (в процессе/готово/ошибка).

4. Транскрипция аудио/видео сообщений — это отдельный механизм от транскрипции звонков (AsrOnlineManager).

## Сводка

`Protos.Audio.transcription`/`Protos.Video.transcription` — текст транскрипции в протобуф. `audio.transcription.enabled = true` по умолчанию. PmsKey: `enable-audio-messages-transcription`/`enable-video-messages-transcription`.
