---
tags: [user-settings, server-control, ws, surveillance, privacy]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/sgj.java
  - work/jadx_base/sources/defpackage/ma4.java
related:
  - "[[32-presence-server-controlled]]"
  - "[[11-state-bots-and-content-policy]]"
  - "[[22-gost-digitalid-family]]"
  - "[[03-pms-server-flags]]"
---

# UserSettings — полная карта серверно-управляемых настроек аккаунта

`defpackage/sgj.java` — модель `UserSettings`, которая приходит с сервера при каждом подключении. Полный список полей из `toString()`:

## Поля UserSettings

| Поле | Тип | Что |
|---|---|---|
| `pushNewContacts` | Boolean | push-уведомления о новых контактах |
| `dontDisturbUntil` | Long | timestamp «не беспокоить до» |
| `dialogsPushNotification` | String | настройка push для диалогов |
| `chatsPushNotification` | String | настройка push для чатов |
| `pushSound` | String | звук push-уведомлений |
| `dialogsPushSound` | String | звук push для диалогов |
| `chatsPushSound` | String | звук push для чатов |
| `hiddenOnline` | Boolean | **скрыть статус онлайн** (см. [[32-presence-server-controlled]]) |
| `led` | Integer | LED-уведомления |
| `dialogsLed` | Integer | LED для диалогов |
| `chatsLed` | Integer | LED для чатов |
| `vibration` | Boolean | вибрация |
| `dialogsVibration` | Boolean | вибрация для диалогов |
| `chatsVibration` | Boolean | вибрация для чатов |
| `chatsInvite` | int | политика приглашений в чаты |
| `incomingCall` | int | настройка входящих звонков |
| `inactiveTtl` | rgj | TTL неактивности |
| `groupChatCallNotificationStatus` | int | уведомления о звонках в группах (ON/OFF/null) |
| `suggestStickersStatus` | int | статус предложения стикеров (ON/OFF/null) |
| `audioTranscriptionEnabled` | Boolean | **включена ли транскрипция аудио** |
| `safeMode` | Boolean | **безопасный режим** |
| `safeModeNoPin` | Boolean | безопасный режим без PIN |
| `searchByPhone` | int | **поиск по номеру телефона** |
| `unsafeFiles` | Boolean | разрешить небезопасные файлы |
| `contentLevelAccess` | Boolean | **уровень доступа к контенту** (см. [[11-state-bots-and-content-policy]]) |
| `familyProtection` | qgj | **семейная защита** (см. [[22-gost-digitalid-family]]) |
| `phoneNumberPrivacy` | int | **приватность номера телефона** |
| `A` | Boolean | неизвестное поле |
| `B` | String | неизвестное поле |

## Что важно

1. **`safeMode` и `safeModeNoPin`** — сервер может включить «безопасный режим» для аккаунта. Что именно ограничивает безопасный режим — неизвестно без дополнительного анализа, но это server-pushed ограничение функциональности.

2. **`searchByPhone`** — сервер контролирует, можно ли найти пользователя по номеру телефона. Это privacy-настройка, но она server-pushed, а не только user-controlled.

3. **`audioTranscriptionEnabled`** — сервер может включить/выключить транскрипцию аудио для конкретного пользователя через UserSettings (в дополнение к PmsKey `enable-audio-messages-transcription`). Два независимых канала управления.

4. **`inactiveTtl`** — TTL неактивности аккаунта. Сервер задаёт, через какое время неактивный аккаунт считается «устаревшим».

5. **`contentLevelAccess`** и **`familyProtection`** — уже описаны в [[11-state-bots-and-content-policy]] и [[22-gost-digitalid-family]]. Подтверждение: эти поля приходят в UserSettings при каждом подключении.

## Сводка

UserSettings — 26+ полей, которые сервер отправляет клиенту при каждом подключении. Включают privacy-настройки (`hiddenOnline`, `searchByPhone`, `phoneNumberPrivacy`), ограничения функциональности (`safeMode`, `contentLevelAccess`), и настройки уведомлений. Все server-pushed, не только user-controlled.
