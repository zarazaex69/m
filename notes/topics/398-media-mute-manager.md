---
tags: [media-mute, calls, admin, force-mute, surveillance, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/media/mute/MediaMuteManager.java
related:
  - "[[391-conversation-events-listener]]"
  - "[[378-p2prelay-sessionroom]]"
  - "[[397-video-sdk-screen-camera]]"
---

# MediaMuteManager — управление медиа-опциями участников

`MediaMuteManager` — управление состоянием аудио/видео/screen share участников звонка.

## Методы

| Метод | Что |
|---|---|
| `updateMediaOptionsForParticipant(states, participantId, roomId, ...)` | **изменить медиа-опции конкретного участника** |
| `updateMediaOptionsForAll(states, roomId, ...)` | **изменить медиа-опции всех участников** |
| `requestToEnableMediaForParticipant(options, participantId, roomId, ...)` | запросить включение медиа у участника |
| `requestToEnableMediaForAll(options, roomId, ...)` | запросить включение медиа у всех |
| `setAudioPlayoutMuted(mute)` | заглушить воспроизведение аудио |
| `getMediaOptionsForCall(roomId)` | получить текущие медиа-опции |

## Что важно

1. **`updateMediaOptionsForParticipant`** — администратор может принудительно изменить медиа-опции (аудио/видео/screen) конкретного участника.

2. **`updateMediaOptionsForAll`** — администратор может изменить медиа-опции всех участников одновременно.

3. **`requestToEnableMediaForParticipant`** — запрос на включение медиа у участника (не принудительно, но запрос отправляется).

4. Это дополняет `onMicrophoneForciblyMuted` из [[391-conversation-events-listener]].

## Сводка

`MediaMuteManager`: `updateMediaOptionsForParticipant`/`updateMediaOptionsForAll` — принудительное изменение медиа-опций. `requestToEnableMediaForParticipant`/`requestToEnableMediaForAll` — запрос включения.
