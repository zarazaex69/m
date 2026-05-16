---
tags: [calls, call-type, stats]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/Conversation.java
related:
  - "[[173-call-finish-init-stat]]"
  - "[[172-conversation-started-signaling-stat]]"
---

# Conversation.CallType — типы звонков

`Conversation.CallType` enum:

| Тип | Что |
|---|---|
| `Incoming` | входящий звонок |
| `Outgoing` | исходящий звонок |
| `Join` | присоединение к существующему звонку |

Используется в `call_init.source` и `call_start.labels`. Сервер знает тип каждого звонка.

`Join` — присоединение по ссылке (см. [[144-hangup-join-api]]).
