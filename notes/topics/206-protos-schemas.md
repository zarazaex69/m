---
tags: [protocol, protobuf, messages, location, video-call, live-stream]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Protos.java
related:
  - "[[203-live-location]]"
  - "[[20-ws-protocol-opcodes]]"
  - "[[205-tasks-proto]]"
---

# Protos.java — схемы сообщений (protobuf nano)

`ru.ok.tamtam.nano.Protos` — protobuf nano-схемы для сообщений и данных. 50+ типов.

## Ключевые схемы

### Location (вложенная в Attach)

| Поле | Что |
|---|---|
| `latitude` / `longitude` | координаты |
| `altitude` | высота |
| `accuracy` | точность |
| `bearing` | направление |
| `speed` | скорость |
| `livePeriod` | период живой геолокации |
| `startTime` / `endTime` | время начала/конца |
| `ttl` | время жизни |
| `zoom` | масштаб |
| `corrupted` | флаг повреждённых данных |

### VideoConversation (вложенная в Attach)

| Поле | Что |
|---|---|
| `conversationId` | ID звонка |
| `joinLink` | ссылка для входа |
| `mediaCallType` | тип звонка |
| `approxParticipantCount` | приблизительное число участников |
| `previewParticipantIds` | ID участников для превью |
| `startedAt` | время начала |
| `type` | `UNKNOWN`(0) / `BY_LINK`(1) / `FROM_CHAT`(2) |

### LogEvent

| Поле | Что |
|---|---|
| `event` | имя события |
| `params` | параметры (bytes) |
| `sessionId` | ID сессии |
| `time` | время |

### Полный список типов

`AdminParticipant`, `AnimationProperties`, `Answer`/`AnswerStats`, `App`, `Attach`/`Attaches`, `Audio`, `BotsInfo`, `Button`/`Buttons`, `Call`/`CallHistoryState`, `ChannelInfo`, `Chat`/`ChatMedia`/`ChatOptions`/`ChatReactionsSettings`/`ChatSettings`/`ChatSubject`, `Chunk`, `Contact`/`ContactName`, `Content`, `Control`, `ConvertOptions`, `File`, `GroupChatInfo`/`GroupOptions`, `InlineKeyboard`, `LastInputMedia`, `LinkAttributes`, `LiveStream`, `Location`/`LocationInfo`, `LogEvent`, `LongList`, `MenuButton`, `MessageElement`/`MessageElements`/`MessagePreview`/`MessageReactions`/`MessageReactionWithCount`, `MissedMessagesItem`, `PendingUploadData`, `Photo`, `Poll`, `Present`, `PushMessage`, `Quality`, `ReactionData`, `Rect`, `ReplyButton`/`ReplyButtons`/`ReplyKeyboard`, `RestrictionsInfo`, `Result`, `Section`, `SelfProfile`, `SendAction`, `Share`, `StartMessage`, `State`, `Sticker`, `Video`/`VideoCollage`/`VideoConversation`, `Widget`.

## Что важно

1. **`Location.ttl`** — время жизни геолокации. Сервер управляет, как долго геолокация видна.

2. **`VideoConversation.previewParticipantIds`** — ID участников звонка видны в превью сообщения.

3. **`LogEvent`** — события логируются с `sessionId` и `params` (bytes). Это внутренний лог-формат.

## Сводка

50+ protobuf nano схем. Ключевые: `Location`(ttl/livePeriod/corrupted), `VideoConversation`(conversationId/joinLink/previewParticipantIds), `LogEvent`(event/params/sessionId).
