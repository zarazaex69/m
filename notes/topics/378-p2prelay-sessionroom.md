---
tags: [p2prelay, sessionroom, calls, network, admin, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/p2prelay/P2PRelaySwitchConfig.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/p2prelay/P2pRelaySwitchTrigger.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/sessionroom/SessionRoomsManager.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/sessionroom/internal/command/SessionRoomCommandExecutor.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/sessionroom/internal/command/SessionRoomAdminCommandExecutor.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/sessionroom/admin/AssignParticipantsToRoomsParams.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/sessionroom/admin/MoveParticipantParams.java
related:
  - "[[101-calls-sdk-managers]]"
  - "[[377-record-manager]]"
---

# P2PRelay + SessionRooms — переключение топологии и комнаты

## P2PRelaySwitchConfig

Конфигурация автоматического переключения P2P → Relay (серверная топология).

| Поле | Что |
|---|---|
| `rttMs` | **порог RTT** (мс) для переключения |
| `rttViolationCount` | количество нарушений RTT до переключения |

### P2pRelaySwitchTrigger

Мониторит `NetworkStat.rttMs`. Если `rttMs >= threshold` N раз подряд → `onSwitchTrigger()` → переключение на relay.

Конфиг приходит с сервера (`getConfig().d()`). При переключении логируется `P2PRelayRequestReason(RTT, threshold, config)`.

**Важно**: при переключении на серверную топологию `AsrOnlineManager` автоматически включает серверный ASR (см. [[376-asr-online-manager]]).

## SessionRoomsManager

Управление «комнатами» внутри звонка (breakout rooms).

### Команды участника

| Команда | Что |
|---|---|
| `joinRoom(roomId)` | войти в комнату |
| `leaveRoom()` | выйти из комнаты |
| `requestAttention()` | запросить внимание (поднять руку) |

### Команды администратора

| Команда | Что |
|---|---|
| `activateRooms(params)` | активировать комнаты |
| `assignParticipantsToRooms(params)` | **назначить участников по комнатам** |
| `moveParticipant(params)` | **переместить участника** в другую комнату |
| `removeRooms(params)` | удалить комнаты |
| `switchRoom(params)` | переключить комнату |
| `updateRooms(params)` | обновить комнаты |

### AssignParticipantsToRoomsParams

`Map<roomId, List<ParticipantId>>` — добавить/удалить участников из комнат.

### MoveParticipantParams

`participantId` + `toRoomId` — переместить конкретного участника.

### OwnRoomsListener

| Callback | Что |
|---|---|
| `onActiveRoomChanged(info)` | активная комната изменилась |
| `onProposedRoomChanged(info)` | предложенная комната изменилась |
| `onRoomRemoved(info)` | комната удалена |
| `onRoomUpdated(info)` | комната обновлена |

## Что важно

1. **P2P → Relay автоматически** — при плохом RTT клиент переключается на серверную топологию. Это означает, что весь трафик начинает идти через сервер.

2. **`moveParticipant`** — администратор может принудительно переместить участника в другую комнату без его согласия.

3. **`requestAttention`** — «поднять руку» в звонке.

4. **Конфиг P2P relay с сервера** — порог RTT задаётся сервером.

## Сводка

`P2pRelaySwitchTrigger`: RTT >= threshold N раз → relay. `SessionRoomsManager`: joinRoom/leaveRoom/requestAttention + admin: assignParticipants/moveParticipant/activateRooms.
