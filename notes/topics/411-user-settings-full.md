---
tags: [user-settings, server-control, surveillance, safe-mode, family-protection, content-level, phone-privacy]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/sgj.java
related:
  - "[[410-user-settings-map]]"
  - "[[11-state-bots-and-content-policy]]"
  - "[[22-gost-digitalid-family]]"
---

# UserSettings (sgj) — полный список полей

`sgj` — полный объект UserSettings. Синхронизируется с сервером.

## Все поля

| Поле | Тип | Что |
|---|---|---|
| `pushNewContacts` (a) | Boolean | push для новых контактов |
| `dontDustirbUntil` (b) | Long | не беспокоить до (timestamp) |
| `dialogsPushNotification` (c) | String | push для диалогов |
| `chatsPushNotification` (d) | String | push для чатов |
| `pushSound` (e) | String | звук push |
| `dialogsPushSound` (f) | String | звук push для диалогов |
| `chatsPushSound` (g) | String | звук push для чатов |
| `hiddenOnline` (h) | Boolean | **скрытый онлайн** |
| `led` (i) | Integer | LED |
| `dialogsLed` (j) | Integer | LED для диалогов |
| `chatsLed` (k) | Integer | LED для чатов |
| `vibration` (l) | Boolean | вибрация |
| `dialogsVibration` (m) | Boolean | вибрация для диалогов |
| `chatsVibration` (n) | Boolean | вибрация для чатов |
| `chatsInvite` (o) | int | приглашения в чаты |
| `incomingCall` (p) | int | входящий звонок |
| `phoneNumberPrivacy` (q) | int | **приватность номера телефона** |
| `inactiveTtl` (r) | rgj | TTL неактивности |
| `groupChatCallNotificationStatus` (s) | int | ON/OFF/null |
| `suggestStickersStatus` (t) | int | ON/OFF/null |
| `audioTranscriptionEnabled` (u) | Boolean | **транскрипция аудио** |
| `safeMode` (v) | Boolean | **безопасный режим** |
| `safeModeNoPin` (w) | Boolean | **безопасный режим без PIN** |
| `searchByPhone` (x) | int | поиск по телефону |
| `unsafeFiles` (y) | Boolean | небезопасные файлы |
| `contentLevelAccess` (z) | Boolean | **уровень доступа к контенту** |
| `familyProtection` (C) | qgj | **семейная защита** |

## Что важно

1. **`hiddenOnline`** — сервер знает реальный онлайн-статус даже при скрытом.

2. **`phoneNumberPrivacy`** — настройка приватности номера телефона. Сервер контролирует.

3. **`safeMode`/`safeModeNoPin`** — безопасный режим. Возможно, ограничивает функциональность.

4. **`contentLevelAccess`** — уровень доступа к контенту (связано с [[11-state-bots-and-content-policy]]).

5. **`familyProtection`** — семейная защита (связано с [[22-gost-digitalid-family]]).

6. **`audioTranscriptionEnabled`** — транскрипция аудио управляется через UserSettings.

## Сводка

`UserSettings` (sgj): 27 полей. Ключевые: hiddenOnline/phoneNumberPrivacy/safeMode/safeModeNoPin/audioTranscriptionEnabled/contentLevelAccess/familyProtection.
