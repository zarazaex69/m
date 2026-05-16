---
tags: [onelog, telemetry, surveillance, install-referrer, fcm, search]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/d6.java
  - work/jadx_base/sources/one/me/sdk/vendor/push/FcmMessagingService.java
  - work/jadx_base/sources/defpackage/vm3.java
  - work/jadx_base/sources/one/me/settings/SettingsListScreen.java
  - work/jadx_base/sources/defpackage/cg9.java
related:
  - "[[31-onelog-event-categories]]"
  - "[[27-hardcoded-keys-audit]]"
---

# ACTION, CLICK, SHOW OneLog — разные категории

## ACTION операции

| Операция | Что |
|---|---|
| `GET_INSTALL_REFERRER` | **источник установки приложения** (is_update_version, value=referrer string) |
| `FCM_ON_DELETED_MESSAGES` | FCM сообщил об удалённых сообщениях |
| `ACTION_CACHE_CLEARED` | очищен кэш приложения |

### GET_INSTALL_REFERRER — особо важно

`d6.java:82` — при первом запуске после установки клиент отправляет в OneLog `GET_INSTALL_REFERRER` с:
- `is_update_version` — обновление или новая установка
- `value` — **строка install referrer** (откуда пришёл пользователь: Google Play, прямая ссылка, реферальная программа)

Это означает: сервер знает, откуда пришёл каждый новый пользователь (из какой рекламной кампании, по какой ссылке). Событие отправляется только один раз при первой установке (проверяется `installationInfoVersion != "26.15.3"`).

## CLICK операции

| Операция | Что |
|---|---|
| `profile_button_click` | нажатие на профиль (с `source_meta`) |
| `video_speed_change` | смена скорости видео (с `source_meta`) |
| `open_broadcast_button_click` | нажатие кнопки трансляции (с `source_meta`) |

## SHOW операции

| Операция | Что |
|---|---|
| `SEARCH_RESPONSE` | показан результат поиска (с атрибутами) |

## Сводка

`GET_INSTALL_REFERRER` — сервер знает источник установки каждого пользователя. `FCM_ON_DELETED_MESSAGES` — сервер знает, когда FCM удалял сообщения. `CLICK` — детальная аналитика кликов с `source_meta`.
