---
tags: [calls, capabilities, hex, server-control, features]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/capabilities/ClientCapabilities.java
related:
  - "[[143-start-conversation-api]]"
  - "[[191-conversation-participant]]"
---

# ClientCapabilities — битовая маска возможностей клиента

`ClientCapabilities` — hex-encoded битовая маска возможностей клиента. Отправляется в `vchat.startConversation` и `vchat.joinConversationByLink`.

## Биты возможностей

| Бит | Константа | Что |
|---|---|---|
| 0 | `BIT_SCREEN_TRACK_PRODUCER` | может делиться экраном |
| 1 | `BIT_VIDEO_TRACKS` | поддерживает видео |
| 2 | `BIT_WAITING_HALL` | поддерживает зал ожидания |
| 3 | `BIT_FILTER_DEFAULTS` | фильтры по умолчанию |
| 4 | `BIT_SCREEN_TRACK_CONSUMER` | может смотреть screenshare |
| 5 | `BIT_ADMIN_MUTE_NOTIFY` | уведомление о принудительном mute |
| 6 | `BIT_WATCH_MOVIE` | поддерживает watch together |
| 8 | `BIT_SESSION_ROOMS` | поддерживает комнаты сессии |
| 9 | `BIT_VMOJI` | поддерживает анимодзи |
| 10 | `BIT_CALL_TO_CONTACTS` | звонки контактам |
| 11 | `BIT_AUDIENCE_MODE` | режим аудитории |
| 14 | `BIT_SESSION_STATE_UPDATES` | обновления состояния сессии |
| 15 | `BIT_ADD_PARTICIPANT` | может добавлять участников |
| 16 | `BIT_USE_P2P_RELAY` | поддерживает P2P relay |
| 17 | `BIT_WAIT_FOR_ADMIN` | поддерживает ожидание администратора |

## Что важно

1. **Hex-encoded** — возможности передаются как hex-строка. Сервер декодирует и знает точные возможности каждого клиента.

2. **`BIT_USE_P2P_RELAY`** — клиент сообщает, поддерживает ли P2P relay. Это влияет на решение сервера о маршрутизации.

3. **`BIT_ADMIN_MUTE_NOTIFY`** — клиент поддерживает уведомление о принудительном mute. Это связано с `onMicrophoneForciblyMuted` (см. [[190-conversation-events-listener]]).

## Сводка

`ClientCapabilities` — 15 битов возможностей. Hex-encoded. Сервер знает точные возможности каждого участника звонка.
