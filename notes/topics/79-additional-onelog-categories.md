---
tags: [onelog, telemetry, surveillance, transcription, qr-auth, channels]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/tvi.java
  - work/jadx_base/sources/defpackage/zf0.java
  - work/jadx_base/sources/defpackage/db0.java
  - work/jadx_base/sources/defpackage/bp3.java
  - work/jadx_base/sources/defpackage/ki4.java
  - work/jadx_base/sources/defpackage/r3e.java
related:
  - "[[31-onelog-event-categories]]"
  - "[[55-2fa-stories-misc-pmskey]]"
---

# Дополнительные OneLog категории — AUDIO_TRANSCRIPTION, AUTH_QR, CHANNEL_RECSYS_FOLDER, PRESENCE, CONTACT_RENAME_BANNER

## AUDIO_TRANSCRIPTION

`tvi.java` — `transcription_result` с атрибутами:
- `message_id`, `media_id`, `message_type`
- `result_type` (0=success, 1=?, 2=error)
- `duration` — длительность аудио
- `waiting_time` — время ожидания транскрипции
- `source_id`, `source_type` — chat_id и тип чата

Сервер знает: для каких сообщений была запрошена транскрипция, как долго ждали, и успешно ли. `source_id` — chat_id, то есть сервер знает, в каком чате пользователь транскрибировал аудио.

## AUTH_QR

`zf0.java` — `LOG` с атрибутами `action`:
- `qr_login_button_click` — нажата кнопка QR-входа
- `permission_prompt_shown` — показан запрос разрешения камеры
- `permission_decision` — решение по разрешению камеры (с `status`)
- `qr_scan_failed` — сканирование QR не удалось (с `fail_reason_code`)
- `qr_scan_succeeded` — сканирование QR успешно
- `qr_not_auth_ui_shown` — показан UI «не авторизован»

Сервер знает каждый шаг QR-аутентификации, включая ошибки.

## CHANNEL_RECSYS_FOLDER

`db0.java`, `bp3.java`, `m57.java` — аналитика рекомендательной системы каналов:
- `channel_folder_open` — открыта папка каналов
- `channel_folder_delete` — удалена папка каналов

## PRESENCE

`r3e.java` — `EVENT_MESSAGE_COUNTER` — счётчик сообщений для presence-системы.

## CONTACT_RENAME_BANNER

`ki4.java`, `ii4.java` — `show` и `save` — показ и сохранение баннера переименования контакта.

## Сводка

`AUDIO_TRANSCRIPTION` — сервер знает каждый запрос транскрипции с chat_id и временем ожидания. `AUTH_QR` — полный лог QR-аутентификации. `CHANNEL_RECSYS_FOLDER` — взаимодействие с рекомендательными папками каналов. `CONTACT_RENAME_BANNER` — переименование контактов.
