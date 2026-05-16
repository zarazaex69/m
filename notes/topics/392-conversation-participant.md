---
tags: [calls-participant, conversation-participant, capabilities, deanon, network-status, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/ConversationParticipant.java
related:
  - "[[390-client-capabilities]]"
  - "[[391-conversation-events-listener]]"
  - "[[376-asr-online-manager]]"
---

# ConversationParticipant — участник звонка

`ConversationParticipant` — объект участника звонка. Содержит полный профиль состояния.

## Поля

| Поле | Что |
|---|---|
| `externalId` | внешний ID (ParticipantId) |
| `internalId` | внутренний ID (hv1) |
| `localParticipantId` | локальный ID |
| `callParticipant` | объект участника с сервера (mv1) |
| `capabilities` | **ClientCapabilities** участника |

## Методы (из callParticipant)

| Метод | Что |
|---|---|
| `getCapabilities()` | **возможности участника** (hex-decoded) |
| `getNetworkStatus()` | **статус сети** участника |
| `getAudioOptionState()` | состояние аудио |
| `getVideoOptionState()` | состояние видео |
| `getScreenshareOptionState()` | состояние захвата экрана |
| `getWatchTogetherOptionState()` | состояние WatchTogether |
| `getMovies()` | список фильмов |
| `getAcceptCallEpochMs()` | время принятия звонка |
| `getAcceptedCallClientType()` | **тип клиента** при принятии |
| `getAcceptedCallPlatform()` | **платформа** при принятии |
| `isAdmin()` | администратор |
| `isCreator()` | создатель |
| `isPrimarySpeaker()` | основной говорящий |
| `isTalking()` | говорит ли сейчас |
| `isAudioEnabled()` | аудио включено |
| `isVideoEnabled()` | видео включено |
| `isScreenCaptureEnabled()` | захват экрана включён |
| `isAnimojiEnabled()` | анимоджи включено |
| `isConnected()` | подключён |
| `isCallAccepted()` | звонок принят |

## deAnonymize

`deAnonymize(mv1, oldId, newId, localIdMappings)` — деанонимизация: замена anonymous ID на реальный.

## Что важно

1. **`getCapabilities()`** — сервер знает возможности каждого участника.

2. **`getNetworkStatus()`** — сервер знает статус сети каждого участника.

3. **`getAcceptedCallClientType()`/`getAcceptedCallPlatform()`** — тип клиента и платформа при принятии звонка.

4. **`isTalking()`** — сервер знает, кто говорит в данный момент.

5. **`deAnonymize`** — механизм деанонимизации участников.

## Сводка

`ConversationParticipant`: externalId/internalId/capabilities/networkStatus/audioState/videoState/screenshareState. `deAnonymize(oldId→newId)`. `getAcceptedCallClientType()`/`getAcceptedCallPlatform()`.
