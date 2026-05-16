---
tags: [calls, microphone, audio-samples, surveillance, webrtc]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/audio/MicrophoneManager.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/audio/internal/MicrophoneManagerImpl.java
related:
  - "[[152-asr-online-manager]]"
  - "[[154-conversation-features-roles]]"
  - "[[70-keyword-spotter-in-calls]]"
---

# MicrophoneManager — доступ к аудио-семплам микрофона

`MicrophoneManager` — интерфейс для управления микрофоном и получения аудио-семплов.

## Методы

| Метод | Что |
|---|---|
| `registerAudioSampleCallback(delay, callback)` | **зарегистрировать callback для аудио-семплов** |
| `removeAudioSampleCallback(callback)` | удалить callback |
| `isMicEnabled` | включён ли микрофон |
| `setMicEnabled(bool)` | включить/выключить микрофон |

## Что важно

1. **`registerAudioSampleCallback(delay, callback)`** — любой компонент SDK может зарегистрировать callback для получения аудио-семплов с микрофона. Это означает, что несколько компонентов одновременно получают доступ к аудио:
   - `KeywordSpotterManagerImpl` (KWS — см. [[70-keyword-spotter-in-calls]])
   - `AsrOnlineManager` (онлайн-транскрипция — см. [[152-asr-online-manager]])
   - `AudioSampleEnergyCalculator` (энергия аудио для talking indicator)

2. **`delay`** — задержка в наносекундах. Разные компоненты могут запрашивать семплы с разной задержкой.

3. **`setMicEnabled(bool)`** — включение/выключение микрофона. Это может быть вызвано через `mute-participant` сигналинг (см. [[153-signaling-commands]]).

## Сводка

`MicrophoneManager` — центральный хаб для доступа к аудио-семплам микрофона. Несколько компонентов одновременно получают аудио: KWS, ASR, energy calculator. `registerAudioSampleCallback` — механизм подписки на аудио-поток.
