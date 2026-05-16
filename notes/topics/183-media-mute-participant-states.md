---
tags: [calls, mute, media-options, server-control, participants]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/media/mute/MediaMuteManager.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/participant/state/ParticipantStatesManager.java
related:
  - "[[158-change-media-settings]]"
  - "[[153-signaling-commands]]"
  - "[[179-conversation-interface]]"
---

# MediaMuteManager и ParticipantStatesManager

## MediaMuteManager

| Метод | Что |
|---|---|
| `getMediaOptionsForCall(roomId)` | получить медиа-опции для комнаты |
| `requestToEnableMediaForAll(mediaTypes, roomId)` | **запросить включение медиа для всех** |
| `requestToEnableMediaForParticipant(mediaTypes, participantId, roomId)` | **запросить включение медиа для участника** |
| `updateMediaOptionsForAll(options, roomId)` | **обновить медиа-опции для всех** |
| `updateMediaOptionsForParticipant(options, participantId, roomId)` | **обновить медиа-опции для участника** |

## ParticipantStatesManager

`ParticipantStateChange`:
- `participantId` — ID участника
- `isOn` — включено ли состояние
- `timestamp` — время изменения

Callback `onParticipantStateChanged(manager, event)` — при изменении состояния участника.

## Что важно

1. **`updateMediaOptionsForAll`** — администратор может изменить медиа-опции для всех участников одновременно. Это более мощный механизм, чем `mute-participant` (см. [[153-signaling-commands]]).

2. **`requestToEnableMediaForParticipant`** — запрос на включение медиа для конкретного участника. Это означает, что администратор может попросить участника включить камеру/микрофон.

3. **`ParticipantStateChange`** с `timestamp` — история изменений состояний участников с временными метками.

## Сводка

`MediaMuteManager` — управление медиа-опциями для всех/конкретного участника. `ParticipantStatesManager` — история изменений состояний с timestamp.
