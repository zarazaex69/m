---
tags: [calls, participant, surveillance, capabilities, network-status]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/ConversationParticipant.java
related:
  - "[[190-conversation-events-listener]]"
  - "[[155-call-roles]]"
  - "[[179-conversation-interface]]"
---

# ConversationParticipant — структура участника звонка

`ConversationParticipant` — полная информация об участнике звонка.

## Поля/методы

| Метод | Что |
|---|---|
| `getExternalId()` | внешний ID (ParticipantId) |
| `getInternalId()` | внутренний ID |
| `getAcceptCallEpochMs()` | **время принятия звонка** (ms) |
| `getAcceptedCallClientType()` | **тип клиента** при принятии |
| `getAcceptedCallPlatform()` | **платформа** при принятии |
| `getCapabilities()` | **возможности клиента** |
| `getNetworkStatus()` | **сетевой статус** |
| `getAudioOptionState()` | состояние аудио |
| `getVideoOptionState()` | состояние видео |
| `getScreenshareOptionState()` | состояние screenshare |
| `getWatchTogetherOptionState()` | состояние watch together |
| `getMovies()` | список видео (watch together) |
| `isAdmin()` | администратор ли |
| `isCreator()` | создатель ли |
| `isPrimarySpeaker()` | основной докладчик ли |
| `isAudioEnabled()` | аудио включено |
| `isVideoEnabled()` | видео включено |
| `isScreenCaptureEnabled()` | screenshare включён |
| `isTalking()` | говорит ли |
| `isAnimojiEnabled()` | анимодзи включён |
| `isReported()` | пожаловались ли |
| `hasRegisteredPeers()` | есть ли зарегистрированные peers |

## Что важно

1. **`getAcceptedCallClientType()` и `getAcceptedCallPlatform()`** — сервер знает тип клиента и платформу каждого участника при принятии звонка.

2. **`getCapabilities()`** — возможности клиента (кодеки, фичи). Сервер знает полные возможности каждого участника.

3. **`getNetworkStatus()`** — сетевой статус участника. Сервер знает качество сети каждого участника.

4. **`isReported()`** — на участника пожаловались. Это означает, что статус жалобы виден другим участникам.

5. **`isTalking()`** — говорит ли участник. Это определяется через `AudioSampleEnergyCalculator` (см. [[156-microphone-manager]]).

## Сводка

`ConversationParticipant` — 20+ полей. Ключевые: `getAcceptedCallClientType`/`getAcceptedCallPlatform` (платформа при входе), `getCapabilities` (возможности), `getNetworkStatus`, `isTalking`, `isReported`.
