---
tags: [calls, onelog, telemetry, surveillance, analytics]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ob2.java
  - work/jadx_base/sources/one/me/calls/ui/ui/settings/CallAdminSettingsScreen.java
related:
  - "[[31-onelog-event-categories]]"
  - "[[04-telemetry-endpoints]]"
  - "[[45-calls-sdk-pmskey-cluster]]"
---

# CALL OneLog события — детальная телеметрия звонков

`defpackage/ob2.java` — аналитика звонков через OneLog. Все события отправляются через `ok9.h(logger, "CALL", operation, attributes, flags)`.

## Список операций

| Операция | Что |
|---|---|
| `INCOMING_CALL_RECEIVED` | входящий звонок получен (с caller_id, call_id, error_type) |
| `INCOMING_CALL_INIT` | инициализация входящего звонка |
| `CALL_RECEIVED_ACCEPT` | пользователь принял звонок |
| `CALL_REMOTE_RINGING` | удалённая сторона звонит |
| `START_CALL` | начало звонка (с типом: ANOTHER_USER_CALL, ANOTHER_USER_TRY, и т.п.) |
| `FINISH_CALL` | завершение звонка (с duration, error, is_group, is_wave) |
| `GROUP_CALL_JOIN` | присоединение к групповому звонку |
| `GROUP_CALL_JOIN_FAILED` | ошибка присоединения к групповому звонку |
| `SHARE_CALL_LINK` | поделиться ссылкой на звонок |
| `ADMIN_CALL_SETTINGS` | изменение настроек звонка администратором |
| `ADMIN_CALL_SETTINGS_TO_USER` | применение настроек к конкретному пользователю (с user_id2) |
| `UNKNOWN_CALLER_ALERT` | показ/клик предупреждения о неизвестном звонящем |

## Атрибуты событий

Из `ob2.java` — атрибуты, которые отправляются с событиями:

| Атрибут | Что |
|---|---|
| `call_id` | идентификатор звонка |
| `user_id2` | ID второго участника |
| `error_type` | тип ошибки |
| `error_desc` | описание ошибки |
| `is_group` | групповой ли звонок |
| `con_state` | состояние соединения |
| `is_wave` | тип звонка (wave?) |
| `screen` | текущий экран |
| `camera` | включена ли камера |
| `microphone` | включён ли микрофон |
| `screenshare` | включён ли screenshare |
| `recording` | включена ли запись |
| `waiting` | режим ожидания |

## Что важно

1. **`FINISH_CALL`** — при завершении звонка сервер получает: duration, error, is_group, is_wave. Это полный лог всех звонков пользователя.

2. **`ADMIN_CALL_SETTINGS_TO_USER`** — администратор группового звонка может применить настройки к конкретному пользователю (отключить камеру, микрофон, screenshare, запись). Это логируется с `user_id2`.

3. **`UNKNOWN_CALLER_ALERT`** — показ предупреждения о неизвестном звонящем логируется. Сервер знает, когда пользователь видел предупреждение и кликнул на него.

4. **`camera`, `microphone`, `screenshare`, `recording`** в `ADMIN_CALL_SETTINGS` — сервер знает, какие медиа-потоки были включены в каждом звонке.

## Сводка

12+ OneLog-операций для звонков. Сервер получает полный лог: кто звонил, когда, сколько длился, с какими ошибками, какие медиа-потоки были включены. `ADMIN_CALL_SETTINGS_TO_USER` — логирование административных действий в групповых звонках с ID пользователя.
