---
tags: [protocol, ws, external-callback, phone-bind, bot, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/byb.java
related:
  - "[[13-deeplinks-idp]]"
  - "[[286-ws-opcodes-additional]]"
---

# EXTERNAL_CALLBACK + PHONE_BIND_REQUEST WS

## EXTERNAL_CALLBACK — внешний callback (IdP)

| Поле | Что |
|---|---|
| `botId` | **ID бота** |
| `startParam` | **стартовый параметр** |
| `chats` | чаты |

## PHONE_BIND_REQUEST — запрос привязки телефона

| Поле | Что |
|---|---|
| `blockingDuration` | **длительность блокировки** |
| `trackId` | ID трекинга |
| `codeLength` | **длина кода** |

## Что важно

1. **`EXTERNAL_CALLBACK.botId`/`startParam`** — внешний callback передаёт ID бота и стартовый параметр. Это механизм IdP (см. [[13-deeplinks-idp]]).

2. **`PHONE_BIND_REQUEST.blockingDuration`** — при привязке телефона сервер может заблокировать повторные попытки на заданное время.

3. **`PHONE_BIND_REQUEST.codeLength`** — длина кода подтверждения. Сервер управляет длиной кода.

## Сводка

`EXTERNAL_CALLBACK`: botId/startParam/chats. `PHONE_BIND_REQUEST`: blockingDuration/trackId/codeLength.
