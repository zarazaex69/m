---
tags: [conversation, calls-sdk, interface, surveillance, admin, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/Conversation.java
related:
  - "[[101-calls-sdk-managers]]"
  - "[[391-conversation-events-listener]]"
  - "[[392-conversation-participant]]"
  - "[[398-media-mute-manager]]"
---

# Conversation — главный интерфейс SDK звонков

`Conversation` — главный интерфейс SDK звонков. Агрегирует все менеджеры.

## CallType

`Incoming` / `Outgoing` / `Join`

## State

`None` → `Preparing` → `Starting` → `Connecting` → `Connected` → `Finished`

## Ключевые методы

| Метод | Что |
|---|---|
| `sendData(participant, JSONObject)` | **отправить произвольные данные участнику** |
| `changeMyState(Map<String,String>)` | **изменить своё состояние** |
| `muteAll()` | **заглушить всех** |
| `setMuteState(participantId, state)` | **изменить mute участника** |
| `grantRoles(participantId, revoke, roles...)` | **выдать/отозвать роли** |
| `promoteParticipant(participantId, promote)` | **повысить/понизить участника** |
| `removeParticipant(participantId, ban)` | **удалить/забанить участника** |
| `addParticipant(externalId, unban, ...)` | добавить участника |
| `addParticipantByLink(link, ...)` | добавить по ссылке |
| `queryChatHistory(offset, count, callback)` | **запросить историю чата** |
| `setAnonJoinForbidden(bool)` | запретить анонимный вход |
| `setWaitingRoomEnabled(bool)` | включить зал ожидания |
| `setFeedbackEnabled(bool)` | включить реакции |
| `pinParticipant(participantId, pin)` | закрепить участника |
| `hangup(parameters)` | завершить звонок |
| `createJoinLink(...)` / `removeJoinLink(...)` | управление ссылкой |

## Менеджеры (getters)

AsrManager / AsrOnlineManager / KeywordSpotterManager / MicrophoneManager / NoiseSuppressionManager / ChatManager / MediaConnectionManager / ContactCallManager / DebugManager / ConversationFeatureManager / FeedbackManager / MediaMuteManager / NetworkConnectionManager / RateManager / RecordManager / SessionRoomsManager / StereoRoomManager / UrlSharingManager / CameraManager / DisplayLayoutSender / ScreenCaptureManager / VideoRenderManager / WatchTogetherPlayer

## Что важно

1. **`sendData(participant, JSONObject)`** — отправка произвольных JSON-данных участнику. Это канал для произвольных команд.

2. **`changeMyState(Map<String,String>)`** — изменение произвольного состояния.

3. **`muteAll()`** — администратор может заглушить всех участников.

4. **`queryChatHistory`** — запрос истории чата внутри звонка.

5. **`getUnderlyingCall()`** — доступ к нижнеуровневому объекту звонка.

## Сводка

`Conversation`: 23 менеджера. Ключевые: sendData(JSON)/muteAll/setMuteState/grantRoles/promoteParticipant/removeParticipant(ban)/queryChatHistory/setAnonJoinForbidden.
