---
tags: [calls, events, listener, surveillance, webrtc]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/events/ConversationEventsListener.java
related:
  - "[[179-conversation-interface]]"
  - "[[151-conversation-impl-callbacks]]"
  - "[[175-conversation-end-reason]]"
---

# ConversationEventsListener — полный список событий звонка

`ConversationEventsListener` — центральный интерфейс событий звонка. 30+ событий:

| Событие | Что |
|---|---|
| `onAdminInCallChanged(bool)` | изменился статус администратора |
| `onAnonJoinForbiddenChanged(bool)` | изменился запрет анонимного входа |
| `onCallAccepted()` | звонок принят |
| `onCallAcceptedForAll()` | звонок принят для всех |
| `onCallEnded(CallEndInfo)` | звонок завершён |
| `onCallIsUnfeasibleError(error)` | ошибка невозможности звонка |
| `onCallParticipantsNetworkStatusChanged(list)` | изменился сетевой статус участников |
| `onCallSignalingConnected()` | сигналинг подключён |
| `onCallStartResolutionFailed(list)` | ошибка разрешения участников |
| `onCameraBusy()` | камера занята |
| `onCameraChanged()` | камера изменилась |
| `onConnected()` | подключён |
| `onConversationIdChanged(old, new)` | **ID разговора изменился** |
| `onCustomData(JSONObject)` | кастомные данные через DataChannel |
| `onDestroyed()` | звонок уничтожен |
| `onDisconnected()` | отключён |
| `onJoinLinkUpdated(link)` | ссылка для входа обновлена |
| `onLocalMediaChanged()` | локальные медиа изменились |
| `onMeInWaitingRoomChanged(bool)` | статус зала ожидания |
| `onMicChanged(bool)` | микрофон изменился |
| `onMicrophoneForciblyMuted()` | **микрофон принудительно заглушён** |
| `onMigratedToServerTopology()` | переход на серверную топологию |
| `onMuteChanged(state)` | состояние mute изменилось |
| `onOpponentFingerprintChanged(long)` | DTLS fingerprint изменился |
| `onOpponentRegistered()` | собеседник зарегистрирован |
| `onParticipantsAdded(list)` | участники добавлены |
| `onParticipantsChanged(list)` | участники изменились |
| `onParticipantsDeAnonymized(list, map)` | **участники деанонимизированы** |
| `onParticipantsRemoved(list)` | участники удалены |
| `onPinChanged(participant, bool)` | закреплённый участник изменился |
| `onRateCall(RateCallData)` | запрос оценки звонка |
| `onRolesChanged(participant)` | роли участника изменились |
| `onStateChanged(participant, state)` | состояние участника изменилось |
| `onWaitForAdminEnabled()` | включён режим ожидания администратора |

## Что особенно важно

1. **`onMicrophoneForciblyMuted()`** — принудительное заглушение микрофона. Это server-initiated действие.

2. **`onParticipantsDeAnonymized(list, map)`** — деанонимизация участников. Сервер может раскрыть личность анонимных участников.

3. **`onConversationIdChanged(old, new)`** — ID разговора может меняться во время звонка.

4. **`onMigratedToServerTopology()`** — переход на серверную топологию (медиа через серверы VK/OK).

## Сводка

30+ событий звонка. Ключевые: `onMicrophoneForciblyMuted` (принудительное заглушение), `onParticipantsDeAnonymized` (деанонимизация), `onMigratedToServerTopology`, `onCustomData` (DataChannel).
