---
tags: [calls, rooms, stereo, promotion, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/sessionroom/SessionRoomsManager.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/stereo/StereoRoomManager.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/rate/RateManager.java
related:
  - "[[141-rate-call-data]]"
  - "[[155-call-roles]]"
  - "[[179-conversation-interface]]"
---

# SessionRoomsManager + StereoRoomManager + RateManager

## RateManager

| Метод | Что |
|---|---|
| `getRateHints()` | список подсказок для оценки |
| `getShouldRateConversation()` | показывать ли диалог оценки |

`getShouldRateConversation()` = `!getRateHints().isEmpty()` — диалог показывается только если сервер прислал `RateHint` (см. [[141-rate-call-data]]).

## SessionRoomsManager — комнаты в конференции

Breakout rooms (разбивка участников по комнатам):

| Метод | Что |
|---|---|
| `activateRooms(params, ...)` | активировать комнаты |
| `assignParticipantsToRooms(params, ...)` | **назначить участников по комнатам** |
| `joinRoom(roomId, ...)` | войти в комнату |
| `getOwnActiveRoom()` | текущая активная комната |
| `getOwnProposedRoom()` | предложенная комната |
| `addListener(OwnRoomsListener)` | слушатель событий |

`OwnRoomsListener`: `onActiveRoomChanged` / `onProposedRoomChanged` / `onRoomRemoved` / `onRoomUpdated`.

## StereoRoomManager — управление ролями в стерео-комнате

Стерео-комнаты — формат «сцена + аудитория» (аналог Clubhouse):

| Метод | Что |
|---|---|
| `requestPromotion(...)` | запросить повышение на сцену |
| `acceptPromotion(...)` | принять повышение |
| `rejectPromotion(...)` | отклонить повышение |
| `promoteParticipant(id, ...)` | **повысить участника** |
| `unpromoteParticipant(id, ...)` | **понизить участника** |
| `grantAdmin(id, ...)` | дать права администратора |
| `revokeAdmin(id, ...)` | забрать права администратора |
| `revokePromotion(id, ...)` | отозвать повышение |
| `getIsMePromoted()` | повышен ли текущий пользователь |
| `getHandsQueue()` | очередь поднятых рук |

## Что важно

1. **`assignParticipantsToRooms`** — администратор может принудительно распределить участников по комнатам.

2. **`StereoRoomManager`** — полная система управления ролями «сцена/аудитория». Администратор контролирует, кто может говорить.

3. **`getHandsQueue()`** — очередь поднятых рук. Сервер знает, кто хочет говорить.

## Сводка

`RateManager` — `getShouldRateConversation()` на основе серверных `RateHint`. `SessionRoomsManager` — breakout rooms с `assignParticipantsToRooms`. `StereoRoomManager` — управление ролями сцена/аудитория с `promoteParticipant`/`getHandsQueue`.
