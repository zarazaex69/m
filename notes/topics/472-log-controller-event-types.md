---
tags: [log-controller, event-types, telemetry, surveillance, audio-transcription, push, permission, background]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ok9.java
  - work/jadx_base/sources/defpackage/jr9.java
related:
  - "[[471-log-controller]]"
  - "[[467-telemetry-event-types]]"
  - "[[409-audio-video-transcription]]"
---

# ok9.h — Все типы событий LogController

Полный список типов событий, отправляемых через `ok9.h()`.

## Типы событий (19 типов)

| Тип | События |
|---|---|
| `ACTION` | ACTION_CACHE_CLEARED / FCM_ON_DELETED_MESSAGES / **GET_INSTALL_REFERRER** |
| `APP_REVIEW` | app_review |
| **`AUDIO_TRANSCRIPTION`** | **transcription_result** |
| `AUTH_QR` | LOG |
| `BACKGROUND_MODE` | carpet_mode_on / snack_click_on / snack_hidden / snack_shown / **system_curtain_hidden** / **system_curtain_shown** / work_in_background_permission |
| `CALL` | **ADMIN_CALL_SETTINGS** / ADMIN_CALL_SETTINGS_TO_USER |
| `CHANNEL_RECSYS_FOLDER` | channel_folder_delete / channel_folder_open |
| `CLICK` | open_broadcast_button_click / profile_button_click / video_speed_change |
| `CONTACT_OR_BLOCK` | clicked / showed |
| `CONTACT_RENAME_BANNER` | save / show |
| `CONTEXT_MENU` | folder_context_menu_readall |
| `HOST_REACHABILITY` | **GET_HOST_REACHABILITY** |
| `PERMISSION` | **permission_changed_state** / **permission_status** |
| `POWER_SAVING` | click_shade_button / close_shade / show_shade |
| `PRESENCE` | **EVENT_MESSAGE_COUNTER** |
| `PUSH` | **Action** / **InboundCall** |
| `SETTINGS` | BACKGROUND / **MINIAPP_BIOMETRY** / TEXT_SIZE / THEME |
| `SHOW` | SEARCH_RESPONSE |
| `sticker` | send_sticker |

## Что важно

1. **`AUDIO_TRANSCRIPTION.transcription_result`** — результат транскрипции отправляется на сервер.

2. **`PERMISSION.permission_changed_state`/`permission_status`** — изменения разрешений логируются.

3. **`HOST_REACHABILITY.GET_HOST_REACHABILITY`** — проверка доступности хостов логируется.

4. **`PUSH.InboundCall`** — входящие звонки через push логируются.

5. **`SETTINGS.MINIAPP_BIOMETRY`** — изменения биометрии мини-приложений логируются.

6. **`PRESENCE.EVENT_MESSAGE_COUNTER`** — счётчик сообщений presence логируется.

## Сводка

19 типов событий. Критические: AUDIO_TRANSCRIPTION/PERMISSION/HOST_REACHABILITY/PUSH/SETTINGS.MINIAPP_BIOMETRY/PRESENCE.
