---
tags: [protocol, tasks, types, background-tasks]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/eed.java
related:
  - "[[205-tasks-proto]]"
  - "[[241-tasks-recent-db-schemas]]"
---

# Task Types — полный список типов фоновых задач

`eed.java` — enum типов задач с маппингом на protobuf ID.

## Все типы (46+)

| Тип | ID | Что |
|---|---|---|
| `TYPE_UNKNOWN` | 0 | неизвестный |
| `TYPE_MSG_DELETE` | 1 | удаление сообщения |
| `TYPE_MSG_SEND` | 2 | отправка сообщения |
| `TYPE_PROFILE` | 3 | обновление профиля |
| `TYPE_CONTACT_UPDATE` | 4 | обновление контакта |
| `TYPE_CONFIG` | 5 | конфигурация |
| `TYPE_CHAT_DELETE` | 6 | удаление чата |
| `TYPE_CHATS_LIST` | 7 | список чатов |
| `TYPE_MSG_EDIT` | 8 | редактирование сообщения |
| `TYPE_CHAT_CLEAR` | 9 | очистка чата |
| `TYPE_VIDEO_PLAY` | 10 | воспроизведение видео |
| `TYPE_CHAT_MARK` | 11 | отметка чата |
| `TYPE_SYNC_CHAT_HISTORY` | 12 | синхронизация истории |
| `TYPE_CHAT_UPDATE` | 13 | обновление чата |
| `TYPE_CHAT_LEAVE` | 14 | выход из чата |
| `TYPE_CHAT_CREATE` | 15 | создание чата |
| `TYPE_MSG_SHARE_PREVIEW` | 16 | превью шаринга |
| `TYPE_CHAT_MEMBERS_UPDATE` | 17 | обновление участников |
| `TYPE_CHAT_SUBSCRIBE` | 18 | подписка на чат |
| `TYPE_CHAT_PIN_SET_VISIBILITY` | 19 | видимость закреплённого |
| `TYPE_FILE_DOWNLOAD_CMD` | 20 | команда загрузки файла |
| `TYPE_REMOVE_CONTACT_PHOTO` | 21 | удаление фото контакта |
| `TYPE_MSG_DELETE_RANGE` | 22 | удаление диапазона сообщений |
| `TYPE_CHAT_COMPLAIN` | 23 | жалоба на чат |
| `TYPE_MSG_SEND_CALLBACK` | 24 | callback отправки |
| `TYPE_SUSPEND_BOT` | 25 | приостановка бота |
| `TYPE_LOCATION_REQUEST` | 26 | **запрос геолокации** |
| `TYPE_CHANGE_PROFILE_OR_CHAT_PHOTO` | 27 | смена фото |
| `TYPE_LOCATION_STOP` | 28 | **остановка геолокации** |
| `TYPE_ASSETS_ADD/LIST_MODIFY/REMOVE/MOVE` | 29-32 | управление ассетами |
| `TYPE_CHAT_HIDE` | 33 | скрытие чата |
| `TYPE_DRAFT_SAVE/DISCARD` | 34-35 | черновики |
| `TYPE_MSG_REACT/CANCEL_REACTION` | 36-37 | реакции |
| `TYPE_UPDATE_FIRE_TIME` | 38 | обновление времени уведомлений |
| `TYPE_CHANGE_CHAT_PHOTO` | 39 | смена фото чата |
| `TYPE_STAT_CRIT_EVENT` | 40 | **критическое событие статистики** |
| `TYPE_COMPLAIN` | 41 | жалоба |
| `TYPE_CHAT_PERSONAL_CONFIG` | 42 | персональная конфигурация чата |
| `TYPE_WARM_CHAT_HISTORY` | 43 | прогрев истории |
| `TYPE_CHAT_MARK_BATCH` | 44 | пакетная отметка |
| `TYPE_CHAT_DELETE_BATCH` | 45 | пакетное удаление |

## Сводка

46 типов задач. Ключевые: `TYPE_LOCATION_REQUEST`/`TYPE_LOCATION_STOP`, `TYPE_STAT_CRIT_EVENT`, `TYPE_SUSPEND_BOT`, `TYPE_WARM_CHAT_HISTORY`.
