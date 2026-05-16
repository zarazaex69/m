---
tags: [protocol, call, protos, schema, call-history]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Protos.java
related:
  - "[[206-protos-schemas]]"
  - "[[178-call-type]]"
---

# Protos.Call + CallHistoryState — звонки в сообщениях

## Protos.Call (вложение типа CALL)

Типы звонков: `AUDIO`(2), `VIDEO`(1), `UNKNOWN_CALL_TYPE`(0).

Типы завершения: `HANGUP`(1), `CANCELED`(2), `REJECTED`(3), `MISSED`(4), `UNKNOWN_HANGUP_TYPE`(0).

| Поле | Что |
|---|---|
| `callType` | тип звонка (аудио/видео) |
| `hangupType` | тип завершения |
| `duration` / `durationLong` | **длительность** |
| `contactIds[]` | **ID участников** |

## CallHistoryState

| Поле | Что |
|---|---|
| `backwardMarker` / `forwardMarker` | маркеры пагинации |
| `hasNext` / `hasPrev` | есть ли следующая/предыдущая страница |
| `MissedMessagesItem.ids[]` | **ID пропущенных сообщений** |

## Что важно

1. **`Call.contactIds[]`** — список ID всех участников звонка хранится в сообщении.

2. **`Call.duration`** — длительность звонка хранится в сообщении.

3. **`CallHistoryState.MissedMessagesItem`** — список ID пропущенных сообщений. Сервер знает, какие сообщения пропущены.

## Сводка

`Protos.Call`: callType(AUDIO/VIDEO)/hangupType(HANGUP/CANCELED/REJECTED/MISSED)/duration/contactIds[]. `CallHistoryState`: backwardMarker/forwardMarker/MissedMessagesItem.ids[].
