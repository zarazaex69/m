---
tags: [waiting-room, urlsharing, feedback, stereo, calls, surveillance, admin]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/waiting_room/WaitingRoomParticipants.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/urlsharing/external/UrlSharingManager.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/urlsharing/external/UrlSharingInfo.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/feedback/FeedbackManager.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/feedback/ParticipantFeedback.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/feedback/ParticipantFeedbackSource.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/stereo/StereoRoomManager.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/stereo/hands/StereoHandQueueItem.java
related:
  - "[[101-calls-sdk-managers]]"
  - "[[378-p2prelay-sessionroom]]"
---

# WaitingRoom + UrlSharing + Feedback + StereoRoom

## WaitingRoom

Зал ожидания перед звонком. Только администратор видит список ожидающих.

| Метод/Поле | Что |
|---|---|
| `loadWaitingParticipantIds()` | загрузить список ожидающих (команда `get-waiting-hall`) |
| `PAGE_SIZE = 50` | страница по 50 участников |
| `onMeInWaitingRoomChanged(bool)` | я в зале ожидания |
| `onWaitingRoomParticipantsChanged(update)` | список изменился |
| `isWaitingRoomEnabled` | включён ли зал ожидания |
| `isMeAdmin` | я администратор |

Команда: `get-waiting-hall` (через signaling).

## UrlSharing

Совместный просмотр URL во время звонка.

| Метод | Что |
|---|---|
| `start(url, onSuccess, onError)` | начать показ URL |
| `stop(onSuccess, onError)` | остановить |
| `onUrlSharingStarted(UrlSharingInfo)` | URL показывается (url, initiatorId) |
| `onUrlSharingStopped()` | остановлено |

`UrlSharingInfo`: `url` + `initiatorId` — сервер знает, кто показывает какой URL.

## FeedbackManager

Реакции участников во время звонка.

| Метод | Что |
|---|---|
| `sendFeedback(key, source, onError, onSuccess)` | отправить реакцию |
| `setTimeout(millis)` | таймаут реакции |
| `getOwnCurrentFeedback()` | текущая реакция |

### ParticipantFeedback

| Поле | Что |
|---|---|
| `key` | ключ реакции |
| `participantId` | кто отреагировал |
| `source` | источник: `USER_CLICK` / `GESTURES` / `UNKNOWN` |
| `finishTimeMs` | время окончания |

**`GESTURES`** — реакция через жест. Это означает, что жесты пользователя отслеживаются.

## StereoRoomManager

«Стерео-комната» — режим с повышением прав участников (promotion).

| Команда | Что |
|---|---|
| `requestPromotion()` | запросить повышение прав |
| `acceptPromotion()` | принять повышение |
| `rejectPromotion()` | отклонить повышение |
| `cancelPromotionRequest()` | отменить запрос |
| `promoteParticipant(id)` | **повысить участника** |
| `unpromoteParticipant(id)` | **понизить участника** |
| `revokePromotion(id)` | отозвать повышение |
| `grantAdmin(id)` | **дать права администратора** |
| `revokeAdmin(id)` | **отозвать права администратора** |
| `rejectPromotionRequest(id)` | отклонить запрос участника |
| `getHandsQueue()` | очередь поднятых рук |

### StereoHandQueueItem

`participantId` + `addedTs` — очередь поднятых рук с временными метками.

## Что важно

1. **`UrlSharingInfo(url, initiatorId)`** — сервер знает, какой URL показывается и кем.

2. **`GESTURES` feedback source** — жесты пользователя отслеживаются как источник реакций.

3. **`grantAdmin`/`revokeAdmin`** — администратор может динамически менять права участников.

4. **`get-waiting-hall`** — команда для получения списка ожидающих. Только для администратора.

## Сводка

`WaitingRoom`: get-waiting-hall, PAGE_SIZE=50. `UrlSharing`: start(url)/stop, UrlSharingInfo(url, initiatorId). `Feedback`: sendFeedback(key, source=USER_CLICK/GESTURES/UNKNOWN). `StereoRoom`: promoteParticipant/grantAdmin/revokeAdmin/handsQueue.
