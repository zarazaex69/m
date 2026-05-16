---
tags: [calls, architecture, summary, surveillance, server-control, webrtc]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/
related:
  - "[[179-conversation-interface]]"
  - "[[195-conversation-factory]]"
  - "[[190-conversation-events-listener]]"
  - "[[152-asr-online-manager]]"
  - "[[134-webrtc-field-trials]]"
  - "[[196-debug-media-dump-manager]]"
---

# Calls SDK — архитектурная сводка

Calls SDK (`ru.ok.android.externcalls`, версия `0.1.13`) — полностью самостоятельный модуль поверх кастомного WebRTC. Охватывает топики 101-200.

## Архитектурные слои

```
ConversationFactory
  └── ConversationImpl (Conversation interface)
        ├── Сигналинг (WS + MessagePack)
        ├── WebRTC (кастомный форк, 8 hardcoded + server fieldTrials)
        └── 20+ менеджеров
```

## Менеджеры Conversation (20+)

| Менеджер | Топик | Ключевое |
|---|---|---|
| `AsrManager` | 184 | on-device ASR, запись в файл |
| `AsrOnlineManager` | 152 | real-time транскрипция, `request-asr` |
| `CallsAudioManager` | 199 | аудио-устройства, `onMutedForever` |
| `CameraManager` | 188 | камера, `isCapturingFromFrontCamera` |
| `ChatManager` | 182 | DataChannel, `sendMessage` |
| `ContactCallManager` | 186 | `iAmAnonymous`/`iWasInitiallyAnonymous` |
| `ConversationFeatureManager` | 197 | `enableFeatureForAll`/`enableFeatureForRoles` |
| `DebugManager` | 196 | `enableFullAudioDump`, `requestMediaDump` |
| `FeedbackManager` | 185 | emoji-реакции |
| `KeywordSpotterManager` | 70 | KWS, целевая фраза «не слышу» |
| `MediaConnectionManager` | 189 | `onMediaConnected(isFirstConnection)` |
| `MediaDumpManager` | 196 | 6 точек перехвата аудио-pipeline |
| `MediaMuteManager` | 183 | mute аудио/видео/screenshare |
| `MicrophoneManager` | 156 | `registerAudioSampleCallback` (KWS+ASR+energy) |
| `MLFeaturesManager` | 197 | KWS+NS делегаты, `DownloadService` |
| `NetworkConnectionManager` | 187 | `getTopology()`, `registerBadConnectionCallback` |
| `NoiseSuppressionManager` | 181 | 4 режима NS, ML-модель с сервера |
| `ParticipantStatesManager` | 183 | состояния участников |
| `RateManager` | 198 | `getShouldRateConversation()` |
| `RecordManager` | 180 | запись звонка, `isStream`/`movieId` |
| `ScreenCaptureManager` | 199 | screenshare + захват системного аудио |
| `SessionRoomsManager` | 198 | breakout rooms, `assignParticipantsToRooms` |
| `StereoRoomManager` | 198 | сцена/аудитория, `promoteParticipant` |
| `VideoRenderManager` | 199 | рендереры видео |

## Сигналинг — 24 команды (топик 153)

Ключевые: `mute-participant`, `request-asr`, `enable-feature-for-roles`, `report-network-stat`, `change-media-settings`, `change-simulcast`, `update-display-layout`.

## Статистика — 18+ структур (топики 163-177)

Каждый звонок генерирует: `ConversationStats`, `IceCandidatePairChangedStat` (реальные IP), `AcceptCallStat`, `CallFinishStat`, `NetworkStat`, `RateHint` и др. Всё уходит на `sdk-api.apptracer.ru` и `CallExternalAnalyticsApiRequest`.

## Ключевые индикаторы

1. **Нет E2E** — медиа через серверы VK/OK при `isP2PForbidden=true` или `onMigratedToServerTopology`.
2. **ASR в звонках** — on-device (`AsrManager`) + server-side (`AsrOnlineManager`). Активируется через сигналинг `request-asr`.
3. **KWS** — `KeywordSpotterManager` с целевой фразой «не слышу». Модель загружается с сервера.
4. **`requestMediaDump`** — удалённый дамп аудио+видео через `DebugManager`/`MediaDumpManager` в production.
5. **`onParticipantsDeAnonymized`** — сервер может деанонимизировать участников.
6. **`onMicrophoneForciblyMuted`** — принудительное заглушение микрофона.
7. **Device fingerprint** в каждом API-запросе: `Build.MANUFACTURER`/`MODEL`/`osVersion`/`ispAsOrg`/`locCc`/`locReg`.
8. **`ClientCapabilities`** — 15-битная маска возможностей клиента, hex-encoded, отправляется серверу.
9. **WebRTC field trials** — 8 hardcoded + `bonusFieldTrials` с сервера.
10. **`UploadConfig.DEFAULT_DISABLE_UPLOAD_IN_CALL=true`** — загрузка логов отключена во время звонка (но включается после).

## vchat API — 11 методов (топик 200)

`startConversation`, `joinConversation`, `joinConversationByLink`, `hangupConversation`, `getConversationParams`, `createJoinLink`, `removeJoinLink`, `clientSupportedCodecs`, + 3 ID mapping.
