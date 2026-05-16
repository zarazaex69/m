---
tags: [telemetry, critlog, onelog, audio-transcription, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ok9.java
  - work/jadx_base/sources/defpackage/tvi.java
  - work/jadx_base/sources/defpackage/vdd.java
related:
  - "[[04-telemetry-endpoints]]"
  - "[[202-onelog-upload-mechanism]]"
  - "[[206-protos-schemas]]"
---

# CritLog — критические события телеметрии

`CritLog` — задача отправки критических событий через OneLog. Структура: `type`/`event`/`params(bytes)`/`userId`/`sessionId`/`time`.

## Все типы событий (30+)

| Тип | Что |
|---|---|
| `ACTION_CACHE_CLEARED` | очистка кэша |
| `ADMIN_CALL_SETTINGS_TO_USER` | настройки звонка от администратора |
| `APP_REVIEW` | запрос оценки приложения |
| `AUDIO_STATS` | статистика аудио |
| `AUDIO_TRANSCRIPTION` | **результат транскрипции аудио** |
| `BACKGROUND` / `BACKGROUND_MODE` | фоновый режим |
| `BANNER` / `INFORMER` / `INVITE_MAX_BANNER` | баннеры |
| `CALL` | события звонков |
| `CHANNEL_RECSYS_FOLDER` | рекомендательная система каналов |
| `CHAT_PROFILE_CLICKABLE_ELEMENT_ACTIONS` | клики в профиле чата |
| `CLICK` / `CONTEXT_MENU` | клики |
| `CONTACT_OR_BLOCK` / `CONTACT_RENAME_BANNER` | контакты |
| `DANGEROUS_FILE_ACTIONS` / `DANGEROUS_URL_ACTIONS` | опасные файлы/URL |
| `DEV` | dev-события |
| `EVENT_MESSAGE_COUNTER` | счётчик сообщений |
| `GET_HOST_REACHABILITY` | проверка доступности хоста |
| `GET_INSTALL_REFERRER` | источник установки |
| `LOG` | общие логи |
| `MESSAGE_CLICKABLE_ELEMENT_ACTIONS` | клики в сообщениях |
| `MINIAPP_BIOMETRY` | биометрия в мини-приложениях |
| `PERMISSION` | изменения разрешений |
| `POWER_SAVING` | энергосбережение |
| `PUSH` | push-уведомления |
| `REGISTRATION` | регистрация |
| `SEARCH_RESPONSE` | результаты поиска |
| `SHARE_TO_MAX` | шаринг в MAX |
| `TEXT_SIZE` / `THEME` | настройки UI |
| `VIDEO_MESSAGE` / `VIDEO_STATS` | видео-сообщения |
| `WEBAPP_ACTION` / `WEBAPP_BRIDGE` | мини-приложения |

## AUDIO_TRANSCRIPTION — детали

Событие `transcription_result`:

| Поле | Что |
|---|---|
| `message_id` | ID сообщения |
| `media_id` | ID медиа |
| `message_type` | тип сообщения |
| `result_type` | результат (0/1/2) |
| `duration` | длительность |
| `waiting_time` | время ожидания транскрипции |
| `source_id` / `source_type` | источник |

## Что важно

1. **`AUDIO_TRANSCRIPTION`** — телеметрия транскрипции голосовых сообщений. Сервер знает `message_id`, `media_id`, `duration`, `waiting_time` для каждой транскрипции.

2. **`DANGEROUS_FILE_ACTIONS`/`DANGEROUS_URL_ACTIONS`** — события при взаимодействии с опасными файлами/URL.

3. **`MINIAPP_BIOMETRY`** — биометрия в мини-приложениях логируется отдельным типом.

4. **`GET_INSTALL_REFERRER`** — источник установки приложения.

## Сводка

30+ типов CritLog событий. Ключевые: `AUDIO_TRANSCRIPTION`(message_id/media_id/duration/waiting_time), `DANGEROUS_FILE_ACTIONS`/`DANGEROUS_URL_ACTIONS`, `MINIAPP_BIOMETRY`, `GET_INSTALL_REFERRER`.
