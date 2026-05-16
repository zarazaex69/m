---
tags: [protocol, ws, notif-call-start, incoming-call, rejected-participants, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/mv6.java
related:
  - "[[20-ws-protocol-opcodes]]"
  - "[[143-start-conversation-api]]"
---

# NOTIF_CALL_START WS — уведомление о входящем звонке

`NOTIF_CALL_START` (опкод 117) — серверное уведомление о входящем звонке.

## Поля

| Поле | Что |
|---|---|
| `conversation_id` | **ID разговора** |
| `rejectedParticipants` | **отклонившие участники** |
| `internalCallerParams` | **внутренние параметры звонящего** |

## Что важно

1. **`rejectedParticipants`** — список участников, отклонивших звонок. Сервер знает, кто отклонил.

2. **`internalCallerParams`** — внутренние параметры звонящего. Это может включать тип клиента, возможности и т.д.

3. **`conversation_id`** — ID разговора для присоединения к звонку.

## Сводка

`NOTIF_CALL_START`: conversation_id/rejectedParticipants/internalCallerParams.
