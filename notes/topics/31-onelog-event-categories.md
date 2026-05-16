---
tags: [telemetry, onelog, events, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ (60+ files calling ok9.h(...))
related:
  - "[[04-telemetry-endpoints]]"
  - "[[14-stat-prefs-metrics]]"
  - "[[19-fcm-push-payload]]"
  - "[[24-host-reachability-probe]]"
---

# OneLog event-категории — что улетает в аналитику

`ru.ok.android.onelog` — общий аналитический канал OK.ru, заворачивает события в `OneLogItem` и отправляет через `Uploader` через `BIND_JOB_SERVICE`-сервис `ru.ok.android.onelog.UploadService`. Конкретный URL не зашит в этом дереве (берётся из `ho` API-клиента, инициализируется выше). Здесь — карта event-категорий, найденных через grep на `ok9.h(...)` (60+ файлов).

API события: `ok9.h(logger, category, operation, attributes_map?, flags)`.

## Категории событий и operation-ы

| Категория | Operation-ы (примеры) | Где |
|---|---|---|
| `PUSH` | `Action`, `InboundCall` | `NotificationTamService`, `a6c`, `bt1` |
| `ACTION` | `FCM_ON_DELETED_MESSAGES`, `GET_INSTALL_REFERRER` | `FcmMessagingService`, `d6` |
| `CALL` | `ADMIN_CALL_SETTINGS`, `ADMIN_CALL_SETTINGS_TO_USER`, ... | `ob2`, `CallAdminSettingsScreen` |
| `AUDIO_STATS` | (динамическое имя оп) | `wb0` |
| `VIDEO_MESSAGE` | (динамическое имя оп) | `etj` |
| `SETTINGS` | `BACKGROUND`, `THEME`, `TEXT_SIZE` | `uu`, `AppearanceSettingsMultiThemeScreen` |
| `BACKGROUND_MODE` | `snack_shown`, `snack_hidden`, `snack_click_on`, `system_curtain_shown`, `system_curtain_hidden`, `carpet_mode_on` | `BackgroundListenService`, `fh9`, `jr9`, `on0` |
| `POWER_SAVING` | `click_shade_button`, `close_shade` | `n86` |
| `PERMISSION` | `permission_status`, `permission_changed_state` | `DailyAnalyticsWorker`, `vdd` |
| `CONTACT_OR_BLOCK` | `clicked`, `showed` | `xi4` |
| `CONTEXT_MENU` | `folder_context_menu_readall` (+ countChats, countMessages) | `c87` |
| `CHAT_PROFILE_CLICKABLE_ELEMENT_ACTIONS` | (динамическое) | `zle` |
| `CHANNEL_RECSYS_FOLDER` | `channel_folder_open`, ... | `db0`, `bp3` |
| `INVITE_MAX_BANNER` | (динамическое имя оп) | `kt8` |
| `SHARE_TO_MAX` | (динамическое имя оп) | `y5h` |
| `sticker` | `send_sticker` (с meta `screen`: `first_message`, `showcase_webapp`) | `n8b`, `abd`, `cf3` |
| `CLICK` | `profile_button_click` (с `source_meta`) | `SettingsListScreen` |
| `HOST_REACHABILITY` | `GET_HOST_REACHABILITY` | (см. [[24-host-reachability-probe]]) |
| `DEV` | (произвольное оп) | `km5` |

## Ключевые наблюдения

1. **`PUSH/Action`** содержит `trid` (transaction id), `eKey` (encryption/session key), и `p_op` — операция реакции пользователя на push: `n_q_rep` (quick reply), `n_q_rep_empty`, `n_canceled` (нотификация отклонена), `n_canceled_ch` (свёрнуто), `open_chat`, `open_url`, `open_chats`. То есть **реакция пользователя на каждый push фиксируется в аналитике**, включая такие действия как «свайп влево, чтобы скрыть».
2. **`PERMISSION/permission_status`** — `DailyAnalyticsWorker` периодически (раз в сутки) репортит **полное состояние всех runtime-разрешений** (CAMERA, RECORD_AUDIO, ACCESS_FINE_LOCATION, READ_CONTACTS и т. д.). Сервер MAX знает, какие пермы у конкретного пользователя выданы.
3. **`PERMISSION/permission_changed_state`** — фиксируется любое изменение разрешения. То есть «пользователь только что отозвал разрешение на микрофон» — отдельное событие.
4. **`BACKGROUND_MODE`** — детально ловит UX вокруг foreground-сервиса «работа в фоне» (см. [[08-background-wake]]): показ снэк-бара, клики по нему, отображение системной шторки уведомления, активация «carpet mode».
5. **`HOST_REACHABILITY`** — то самое событие отчёта по probe-у списка хостов с операторской инфой/IP/VPN (см. [[24-host-reachability-probe]]).
6. **`AUDIO_STATS`** — отдельная категория для аудио-статистики (вероятно бит-рейт, потери пакетов, задержка). Event имя — динамическая строка (зависит от под-метрики).
7. **`sticker/send_sticker`** включает meta `screen` со значениями `first_message` (отправка стикера в первое сообщение в новом чате) и `showcase_webapp` — то есть фиксируется **контекст**, откуда пользователь нажал стикер.
8. **`CONTACT_OR_BLOCK`** — UX вокруг блокировки контактов; `showed` (показано окно) и `clicked` (клик).

## Что значит в сумме

OneLog у MAX — это **широкий поведенческий канал телеметрии**: каждый клик в настройках, каждая реакция на push, каждое изменение разрешения, состояние foreground-сервиса, статистика звонка, все клики по стикерам с контекстом — попадают в `ru.ok.android.onelog.UploadService` и оттуда уходят на инфраструктуру OK.ru.

С точки зрения «что сервер знает о пользователе» — поведенческий профиль строится в очень мелкой гранулярности: какой темой пользуется, какие пермы даёт, какие пуши свайпает, какие стикеры отправляет в каких чатах.

PmsKey `analytics-enabled` (см. [[03-pms-server-flags]]) — серверный kill-switch для всего этого канала, плюс `mytracker-enabled` для отдельного MyTracker-канала.

Подытоживая: **MAX — это полностью инструментированный продукт в OK-стандарте**, и OneLog у него — основной канал поведенческой аналитики.
