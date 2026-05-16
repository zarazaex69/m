---
tags: [ws, protocol, server-push, notifications, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/u0d.java
related:
  - "[[20-ws-protocol-opcodes]]"
  - "[[19-fcm-push-payload]]"
  - "[[84-transcribe-media-ws-opcode]]"
---

# NOTIF_* WS-опкоды — серверные push-уведомления по WS

Все `NOTIF_*` опкоды — это **server-to-client push** по WS-соединению. Сервер инициирует отправку без запроса клиента.

## Полный список NOTIF-опкодов

| Опкод | Enum # | WS code | Что |
|---|---|---|---|
| `NOTIF_MESSAGE` | 109 | 128 | новое сообщение |
| `NOTIF_TYPING` | 110 | 129 | пользователь печатает |
| `NOTIF_MARK` | 111 | 130 | отметка прочитанного |
| `NOTIF_CONTACT` | 112 | 131 | изменение контакта |
| `NOTIF_PRESENCE` | 113 | 132 | изменение presence-статуса |
| `NOTIF_CONFIG` | 114 | 134 | **изменение конфигурации** (PmsKey update) |
| `NOTIF_CHAT` | 115 | 135 | изменение чата |
| `NOTIF_ATTACH` | 116 | 136 | изменение вложения |
| `NOTIF_CALL_START` | 117 | 137 | начало звонка |
| `NOTIF_CONTACT_SORT` | 118 | 139 | сортировка контактов |
| `NOTIF_MSG_DELETE_RANGE` | 119 | 140 | удаление диапазона сообщений |
| `NOTIF_MSG_DELETE` | 120 | 142 | удаление сообщения |
| `NOTIF_MSG_REACTIONS_CHANGED` | 121 | 155 | изменение реакций |
| `NOTIF_MSG_YOU_REACTED` | 122 | 156 | пользователь поставил реакцию |
| `NOTIF_CALLBACK_ANSWER` | 123 | 143 | ответ на callback-кнопку |
| `NOTIF_LOCATION` | 126 | 147 | обновление геолокации |
| `NOTIF_LOCATION_REQUEST` | 127 | 148 | **запрос геолокации** (см. [[19-fcm-push-payload]]) |
| `NOTIF_ASSETS_UPDATE` | 128 | 150 | обновление ассетов |
| `NOTIF_DRAFT` | 129 | 152 | синхронизация черновика |
| `NOTIF_DRAFT_DISCARD` | 130 | 153 | отмена черновика |
| `NOTIF_MSG_DELAYED` | 135 | 154 | отложенное сообщение |
| `NOTIF_PROFILE` | 136 | 159 | изменение профиля |
| `NOTIF_FOLDERS` | 147 | 277 | изменение папок |
| `NOTIF_BANNERS` | 149 | 292 | **server-pushed баннеры** (см. [[44-informer-banners-fakeboss-livestreams]]) |
| `NOTIF_TRANSCRIPTION` | 158 | 293 | **результат транскрипции** (см. [[84-transcribe-media-ws-opcode]]) |

## Что важно

1. **`NOTIF_CONFIG(134)`** — сервер может в любой момент обновить конфигурацию клиента (PmsKey) по WS без перезапуска. Это основной механизм server-pushed конфигурации.

2. **`NOTIF_LOCATION_REQUEST(148)`** — сервер может запросить геолокацию пользователя по WS. Это подтверждает [[19-fcm-push-payload]] (LocationRequest через FCM) — два независимых канала запроса геолокации.

3. **`NOTIF_DRAFT(152)` и `NOTIF_DRAFT_DISCARD(153)`** — черновики синхронизируются с сервером в реальном времени. Сервер знает содержимое черновиков до их отправки.

4. **`NOTIF_BANNERS(292)`** — server-pushed баннеры через WS (в дополнение к периодической синхронизации из [[44-informer-banners-fakeboss-livestreams]]).

5. **`NOTIF_TRANSCRIPTION(293)`** — результат транскрипции приходит по WS (подтверждение [[84-transcribe-media-ws-opcode]]).

## Сводка

25 NOTIF-опкодов — полный server-to-client push-канал. Ключевые: `NOTIF_CONFIG` (real-time PmsKey update), `NOTIF_LOCATION_REQUEST` (запрос геолокации), `NOTIF_DRAFT` (синхронизация черновиков), `NOTIF_BANNERS` (server-pushed UI).
