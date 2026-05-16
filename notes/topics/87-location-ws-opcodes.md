---
tags: [location, ws, server-control, surveillance, live-location]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/iqg.java
  - work/jadx_base/sources/defpackage/ck9.java
  - work/jadx_base/sources/defpackage/bk9.java
  - work/jadx_base/sources/defpackage/u0d.java
related:
  - "[[19-fcm-push-payload]]"
  - "[[34-yandex-maps-integration]]"
  - "[[85-notif-ws-opcodes]]"
---

# LOCATION WS-опкоды — live location и запросы геолокации

## WS-опкоды

| Опкод | Enum # | WS code | Что |
|---|---|---|---|
| `LOCATION_SEND` | 103 | 125 | отправить геолокацию |
| `LOCATION_REQUEST` | 104 | 126 | **запросить геолокацию** (client→server) |
| `LOCATION_STOP` | 102 | 124 | остановить трансляцию геолокации |
| `NOTIF_LOCATION` | 126 | 147 | server→client: обновление геолокации |
| `NOTIF_LOCATION_REQUEST` | 127 | 148 | server→client: **запрос геолокации** |

## Структуры (из Tasks.proto)

`LocationRequest`:
- `requestId` — ID запроса
- `messageId` — ID сообщения
- `liveLocation` — bool: live location или разовая

`LocationStop`:
- `requestId` — ID запроса
- `chatId` — ID чата
- `messageId` — ID сообщения

## Что важно

1. **`NOTIF_LOCATION_REQUEST(148)`** — сервер может запросить геолокацию пользователя по WS. Это второй канал (помимо FCM `LocationRequest` из [[19-fcm-push-payload]]). Два независимых механизма запроса геолокации.

2. **`liveLocation=true`** — live location: клиент периодически отправляет обновления координат. Сервер получает непрерывный поток геолокации пользователя.

3. **`LOCATION_STOP`** с `chatId` и `messageId` — остановка трансляции привязана к конкретному сообщению в конкретном чате. Сервер знает, в каком чате пользователь делился live location.

4. В сочетании с [[34-yandex-maps-integration]] — координаты уходят и на MAX-сервер (через WS), и на Yandex (через HTTP). Два независимых получателя геолокации.

## Сводка

5 LOCATION WS-опкодов. `NOTIF_LOCATION_REQUEST` — сервер может запросить геолокацию по WS. `liveLocation` — непрерывный поток координат. Координаты уходят на MAX-сервер (WS) и Yandex (HTTP) независимо.
