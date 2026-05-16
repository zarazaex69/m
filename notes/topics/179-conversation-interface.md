---
tags: [calls, conversation-interface, managers, webrtc]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/Conversation.java
related:
  - "[[152-asr-online-manager]]"
  - "[[156-microphone-manager]]"
  - "[[154-conversation-features-roles]]"
  - "[[159-stereo-room-manager]]"
---

# Conversation interface — полный список менеджеров

`Conversation` — центральный интерфейс звонкового SDK. Содержит 30+ менеджеров:

| Менеджер | Что |
|---|---|
| `AsrManager` | on-device ASR |
| `AsrOnlineManager` | онлайн-транскрипция (см. [[152-asr-online-manager]]) |
| `CameraManager` | управление камерой |
| `ChatManager` | чат во время звонка |
| `ContactCallManager` | управление контактами в звонке |
| `ConversationFeatureManager` | фичи с role-based access (см. [[154-conversation-features-roles]]) |
| `FeedbackManager` | обратная связь |
| `KeywordSpotterManager` | KWS (см. [[70-keyword-spotter-in-calls]]) |
| `MediaConnectionManager` | медиа-соединение |
| `MediaMuteManager` | управление mute |
| `MicrophoneManager` | микрофон (см. [[156-microphone-manager]]) |
| `NetworkConnectionManager` | сетевое соединение |
| `NoiseSuppressionManager` | шумоподавление |
| `ParticipantStatesManager` | состояния участников |
| `RateManager` | оценка качества |
| `RecordManager` | запись звонка |
| `StereoRoomManager` | комнаты/роли (см. [[159-stereo-room-manager]]) |
| `UrlSharingManager` | шаринг URL (см. [[157-watch-together-url-sharing]]) |
| `WatchTogetherPlayer` | совместный просмотр (см. [[157-watch-together-url-sharing]]) |
| `DisplayLayoutSender` | отправка layout (см. [[162-perf-stat-display-layout]]) |
| `DebugManager` | debug-менеджер |
| `AnimojiControl` | управление анимодзи |

## Что важно

1. **`DebugManager`** — debug-менеджер в production интерфейсе. Это подтверждение [[05-dev-menu-in-prod]].

2. **`NoiseSuppressionManager`** — отдельный менеджер шумоподавления. Это дополнение к `update-media-modifiers` (см. [[160-update-media-modifiers]]).

3. **`ChatManager`** — чат во время звонка. Это отдельный канал общения внутри звонка.

## Сводка

`Conversation` — 30+ менеджеров. Ключевые: ASR/ASR-online, KWS, Camera, Microphone, NoiseSuppressionManager, RecordManager, FeatureManager, StereoRoomManager, DebugManager.
