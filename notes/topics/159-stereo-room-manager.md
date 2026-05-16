---
tags: [calls, stereo-rooms, roles, promotion, hands, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/stereo/internal/StereoRoomManagerImpl.java
related:
  - "[[153-signaling-commands]]"
  - "[[155-call-roles]]"
  - "[[154-conversation-features-roles]]"
---

# StereoRoomManager — комнаты, роли и поднятые руки в звонках

`StereoRoomManager` — управление «стерео-комнатами» (rooms) в групповых звонках. Это механизм разделения участников на подгруппы.

## Методы

| Метод | Что |
|---|---|
| `requestPromotion()` | запросить повышение роли (поднять руку) |
| `cancelPromotionRequest()` | отменить запрос |
| `acceptPromotion()` | принять повышение |
| `rejectPromotion()` | отклонить повышение |
| `promoteParticipant(participantId)` | повысить участника |
| `revokePromotion(participantId)` | отозвать повышение |
| `grantAdmin(participantId)` | дать права администратора |
| `revokeAdmin(participantId)` | отозвать права администратора |
| `isMePromoted` | повышен ли текущий пользователь |
| `isMeInWaitingRoom` | в зале ожидания ли |

## GrantRolesRequest

`grantRoles(participantId, revoke, roles[], onSuccess, onError)` — выдать/отозвать роли участнику.

## Callbacks

- `onAttendee(event)` — изменение статуса участника
- `onHandUp(event)` — поднятая рука
- `onFeedback(event)` — обратная связь
- `onPromotionUpdated(event)` — изменение статуса повышения

## Что важно

1. **`requestPromotion`** — «поднять руку» для получения слова. Это механизм модерации в вебинарах.

2. **`grantAdmin`/`revokeAdmin`** — динамическое изменение прав администратора во время звонка.

3. **`handsQueue`** — очередь поднятых рук. Сервер знает, кто и когда поднял руку.

4. **`switch-room`** сигналинг (см. [[153-signaling-commands]]) — переключение между комнатами.

## Сводка

`StereoRoomManager` — управление комнатами, ролями и поднятыми руками. `grantAdmin`/`revokeAdmin` — динамическое изменение прав. `requestPromotion`/`promoteParticipant` — механизм модерации вебинаров.
