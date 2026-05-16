---
tags: [protocol, ws, opcodes, sessions, phone-bind, folders, webapp, profile-delete]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/u0d.java
related:
  - "[[20-ws-protocol-opcodes]]"
  - "[[245-login-ws-response]]"
---

# WS Opcodes — дополнительные (не покрытые в топике 20)

Дополнение к [[20-ws-protocol-opcodes]]. Новые опкоды, обнаруженные при анализе.

## Сессии

| Опкод | Что |
|---|---|
| `SESSIONS_INFO` (91) | **информация о сессиях** |
| `SESSIONS_CLOSE` (92) | **закрыть сессию** |

## Привязка телефона

| Опкод | Что |
|---|---|
| `PHONE_BIND_REQUEST` (93) | запрос привязки телефона |
| `PHONE_BIND_CONFIRM` (94) | подтверждение привязки |
| `PHONE_WEBAPP_SHARE` (97) | **шаринг телефона в WebApp** |

## Профиль

| Опкод | Что |
|---|---|
| `PROFILE_DELETE` (137) | **удаление профиля** |
| `PROFILE_DELETE_TIME` (138) | **время удаления профиля** |
| `NOTIF_PROFILE` (136) | уведомление об изменении профиля |

## WebApp

| Опкод | Что |
|---|---|
| `WEB_APP_INIT_DATA` (139) | **инициализация WebApp** |

## Папки

| Опкод | Что |
|---|---|
| `FOLDERS_GET` (142) | получить папки |
| `FOLDERS_GET_BY_ID` (143) | получить папку по ID |
| `FOLDERS_UPDATE` (144) | обновить папку |
| `FOLDERS_REORDER` (145) | переупорядочить папки |
| `FOLDERS_DELETE` (146) | удалить папку |
| `NOTIF_FOLDERS` (147) | уведомление об изменении папок |

## Уведомления

| Опкод | Что |
|---|---|
| `NOTIF_MSG_REACTIONS_CHANGED` (121) | изменились реакции |
| `NOTIF_MSG_YOU_REACTED` (122) | вы отреагировали |
| `NOTIF_CALLBACK_ANSWER` (123) | ответ на callback |
| `NOTIF_LOCATION` (126) | уведомление о геолокации |
| `NOTIF_LOCATION_REQUEST` (127) | запрос геолокации |
| `NOTIF_MSG_DELAYED` (135) | отложенное сообщение |
| `NOTIF_ASSETS_UPDATE` (128) | обновление ассетов |
| `NOTIF_DRAFT` (129) / `NOTIF_DRAFT_DISCARD` (130) | черновики |

## Прочее

| Опкод | Что |
|---|---|
| `LINK_INFO` (90) | информация о ссылке |
| `GET_INBOUND_CALLS` (95) | входящие звонки |
| `OK_TOKEN` (98) | OK-токен |
| `STICKER_CREATE` (106) | создание стикера |
| `STICKER_SUGGEST` (107) | предложение стикера |
| `VIDEO_CHAT_MEMBERS` (108) | участники видеочата |
| `CHAT_BOT_COMMANDS` (124) | команды бота |
| `BOT_INFO` (125) | информация о боте |
| `CHAT_SEARCH_COMMON_PARTICIPANTS` (134) | поиск общих участников |
| `COMPLAIN_REASONS_GET` (141) | получить причины жалоб |

## Что важно

1. **`SESSIONS_INFO`/`SESSIONS_CLOSE`** — управление активными сессиями. Сервер знает все активные сессии.

2. **`PROFILE_DELETE`/`PROFILE_DELETE_TIME`** — удаление профиля с временем. Это механизм отложенного удаления аккаунта.

3. **`WEB_APP_INIT_DATA`** — инициализация WebApp через WS. Данные для мини-приложений.

4. **`PHONE_WEBAPP_SHARE`** — шаринг телефона в WebApp через WS.

## Сводка

Дополнительные WS опкоды: SESSIONS_INFO/CLOSE, PHONE_BIND_REQUEST/CONFIRM/WEBAPP_SHARE, PROFILE_DELETE/TIME, WEB_APP_INIT_DATA, FOLDERS_GET/UPDATE/REORDER/DELETE, NOTIF_MSG_REACTIONS_CHANGED/YOU_REACTED.
