---
tags: [ws, protocol, tamtam, opcodes, surveillance, command-channel]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/u0d.java
  - findings/raw/ws_opcodes.txt
related:
  - "[[19-fcm-push-payload]]"
  - "[[03-pms-server-flags]]"
  - "[[06-contacts]]"
---

# WS-протокол MAX/TamTam — 159 опкодов

WS-сессия с `api.oneme.ru` использует бинарный протокол TamTam. Все запросы и серверные нотификации идентифицируются `short`-опкодом. Полный список — `findings/raw/ws_opcodes.txt`.

Ниже — группировка по семантике с акцентом на «что это значит для приватности и серверного контроля».

## Группы опкодов

### Сессия (1, 2, 3, 5, 6, 21, 22, 23, 158)

`PING(1)`, `DEBUG(2)`, `RECONNECT(3)`, `LOG(5)`, `SESSION_INIT(6)`, `SYNC(21)`, `CONFIG(22)`, `AUTH_CONFIRM(23)`, `OK_TOKEN(158)`.

`OK_TOKEN` — обмен Single-Sign-On токеном с OK.ru/VK ID. После авторизации в MAX тот же стек получает токен для других сервисов экосистемы.

### Аутентификация и регистрация (16-20, 98-117)

`PROFILE(16)`, `AUTH_REQUEST(17)`, `AUTH(18)`, `LOGIN(19)`, `LOGOUT(20)`, `PHONE_BIND_REQUEST(98)`, `PHONE_BIND_CONFIRM(99)`, `AUTH_LOGIN_RESTORE_PASSWORD(101)`, `AUTH_2FA_DETAILS(104)`, `EXTERNAL_CALLBACK(105)`, `PHONE_WEBAPP_SHARE(106)`, `AUTH_VALIDATE_PASSWORD(107)`, `AUTH_VALIDATE_HINT(108)`, `AUTH_VERIFY_EMAIL(109)`, `AUTH_CHECK_EMAIL(110)`, `AUTH_SET_2FA(111)`, `AUTH_CREATE_TRACK(112)`, `AUTH_CHECK_PASSWORD(113)`, `AUTH_LOGIN_CHECK_PASSWORD(115)`, `AUTH_LOGIN_PROFILE_DELETE(116)`, `AUTH_QR_APPROVE(290)`.

Из необычного:

- **`EXTERNAL_CALLBACK(105)`** — пара к [[13-deeplinks-idp|deeplink :auth?externalCallback=1]]. Это серверная подпись OAuth-callback-а внешнему приложению. То есть MAX в роли Identity Provider оформляется как отдельный опкод протокола.
- **`PHONE_WEBAPP_SHARE(106)`** — поделиться номером телефона с мини-приложением (см. [[10-webapp-jsbridge]]). Серверная команда подтверждения, что мини-апке отдадим MSISDN.
- `AUTH_QR_APPROVE(290)` — авторизация по QR (логин на десктопе через скан кода в мобильном клиенте).

### Контакты (32-43, 46)

`CONTACT_INFO(32)`, `CONTACT_ADD(33)`, `CONTACT_UPDATE(34)`, `CONTACT_PRESENCE(35)`, `CONTACT_LIST(36)`, `CONTACT_SEARCH(37)`, `CONTACT_MUTUAL(38)`, `CONTACT_PHOTOS(39)`, `CONTACT_SORT(40)`, `CONTACT_VERIFY(42)`, `REMOVE_CONTACT_PHOTO(43)`, `CONTACT_INFO_BY_PHONE(46)`.

`CONTACT_INFO_BY_PHONE(46)` — резолв номера → MAX-аккаунт без необходимости иметь номер в контактах. Используется в фоновом сборе «не-контактов» (см. [[06-contacts]] и PmsKey `non-contact-*`).

### Чаты (48-63, 86, 117, 196, 198)

`CHAT_INFO(48)`, `CHAT_HISTORY(49)`, `CHAT_MARK(50)`, `CHAT_MEDIA(51)`, `CHAT_DELETE(52)`, `CHATS_LIST(53)`, `CHAT_CLEAR(54)`, `CHAT_UPDATE(55)`, `CHAT_CHECK_LINK(56)`, `CHAT_JOIN(57)`, `CHAT_LEAVE(58)`, `CHAT_MEMBERS(59)`, `PUBLIC_SEARCH(60)`, `CHAT_PERSONAL_CONFIG(61)`, `CHAT_LIVESTREAM_INFO(62)`, `CHAT_CREATE(63)`, `CHAT_PIN_SET_VISIBILITY(86)`, `CHAT_COMPLAIN(117)`, `CHAT_HIDE(196)`, `CHAT_SEARCH_COMMON_PARTICIPANTS(198)`, `CHAT_SUGGEST(300)`.

`CHAT_SEARCH_COMMON_PARTICIPANTS(198)` — поиск общих участников в чатах. Это не безобидно: позволяет узнать «есть ли у меня и у X общий чат», что раскрывает социальный граф.

### Сообщения (64-74, 92, 118, 154, 178-181, 202, 303-306)

`MSG_SEND(64)`, `MSG_TYPING(65)`, `MSG_DELETE(66)`, `MSG_EDIT(67)`, `CHAT_SEARCH(68)`, `MSG_SHARE_PREVIEW(70)`, `MSG_GET(71)`, `MSG_SEARCH_TOUCH(72)`, `MSG_SEARCH(73)`, `MSG_GET_STAT(74)`, `MSG_DELETE_RANGE(92)`, `MSG_SEND_CALLBACK(118)`, `NOTIF_MSG_DELAYED(154)`, `MSG_REACTION(178)`, `MSG_CANCEL_REACTION(179)`, `MSG_GET_REACTIONS(180)`, `MSG_GET_DETAILED_REACTIONS(181)`, `TRANSCRIBE_MEDIA(202)`, `MSG_DELIVERY(303)`, `SEND_VOTE(304)`, `VOTERS_LIST_BY_ANSWER(305)`, `GET_POLL_UPDATES(306)`.

`TRANSCRIBE_MEDIA(202)` — серверная транскрипция. Запрос «расшифруй вот это аудио»: альтернатива on-device ASR из [[15-on-device-asr-kws-diarization]]. То есть для расшифровки голосовых сообщений у клиента **два пути**: локально (через `libEnhancementLibShared.so`) и серверно (через `TRANSCRIBE_MEDIA`). Какой путь применяется когда — определяется PmsKey `audio-transcription-locales` и UserSettings.

`MSG_GET_STAT(74)` — статистика по сообщению (просмотры/реакции/forward-ы) на стороне сервера. Сервер видит больше, чем клиент.

### Звонки (76, 78, 79, 84, 103, 137, 143, 166, 195)

`VIDEO_CHAT_START(76)`, `VIDEO_CHAT_START_ACTIVE(78)`, `VIDEO_CHAT_HISTORY(79)`, `VIDEO_CHAT_CREATE_JOIN_LINK(84)`, `GET_INBOUND_CALLS(103)`, `NOTIF_CALL_START(137)`, `NOTIF_CALLBACK_ANSWER(143)`, `VIDEO_CHAT_JOIN(166)`, `VIDEO_CHAT_MEMBERS(195)`.

### Файлы и медиа (80-83, 87-89)

`PHOTO_UPLOAD(80)`, `STICKER_UPLOAD(81)`, `VIDEO_UPLOAD(82)`, `VIDEO_PLAY(83)`, `FILE_UPLOAD(87)`, `FILE_DOWNLOAD(88)`, `LINK_INFO(89)`.

`LINK_INFO(89)` — серверный resolve превью URL. Клиент шлёт URL, сервер возвращает превью. Это означает, что **URL из переписок проходит через сервер** до того, как пользователь нажмёт.

### Геолокация (125-126, 147-148)

`LOCATION_SEND(125)`, `LOCATION_REQUEST(126)`, `NOTIF_LOCATION(147)`, `NOTIF_LOCATION_REQUEST(148)`.

`LOCATION_REQUEST(126)` (серверный → клиент) совпадает с [[19-fcm-push-payload|push-типом `LocationRequest`]]. То есть после wake-trigger через push сервер по WS отправляет опкод 126 клиенту, и клиент возвращает координаты через `LOCATION_SEND(125)`. Цепочка «silent push → WS LOCATION_REQUEST → LOCATION_SEND» документирована протоколом.

PmsKey `send-location-enabled` (см. [[03-pms-server-flags]]) — глобальный gate для этого. Если выключен — клиент при `LOCATION_REQUEST` не отвечает координатами.

### Серверные нотификации (NOTIF_*) (128-160)

`NOTIF_MESSAGE(128)`, `NOTIF_TYPING(129)`, `NOTIF_MARK(130)`, `NOTIF_CONTACT(131)`, `NOTIF_PRESENCE(132)`, `NOTIF_CONFIG(134)`, `NOTIF_CHAT(135)`, `NOTIF_ATTACH(136)`, `NOTIF_CALL_START(137)`, `NOTIF_CONTACT_SORT(139)`, `NOTIF_MSG_DELETE_RANGE(140)`, `NOTIF_MSG_DELETE(142)`, `NOTIF_LOCATION(147)`, `NOTIF_LOCATION_REQUEST(148)`, `NOTIF_ASSETS_UPDATE(150)`, `NOTIF_DRAFT(152)`, `NOTIF_DRAFT_DISCARD(153)`, `NOTIF_MSG_DELAYED(154)`, `NOTIF_MSG_REACTIONS_CHANGED(155)`, `NOTIF_MSG_YOU_REACTED(156)`, `NOTIF_PROFILE(159)`, `NOTIF_BANNERS(292)`, `NOTIF_TRANSCRIPTION(293)`, `NOTIF_FOLDERS(277)`.

`NOTIF_DRAFT(152)` / `NOTIF_DRAFT_DISCARD(153)` / `DRAFT_SAVE(176)` / `DRAFT_DISCARD(177)` — **черновики синхронизируются между устройствами через сервер**. Это значит то, что пользователь набирает, но не отправил, — попадает в `api.oneme.ru`. Без E2E-шифрования (см. [[FINDINGS]] §14) — сервер видит черновик в открытом виде.

`NOTIF_PROFILE(159)` — серверный апдейт профиля. То есть свой собственный профиль пользователя (имя, аватар, статус) пушится с сервера. Это даёт серверу право принудительно менять отображаемое имя или аватар клиента.

### Чёрные списки и жалобы (161, 162, 117)

`COMPLAIN(161)`, `COMPLAIN_REASONS_GET(162)`, `CHAT_COMPLAIN(117)`.

### Боты, команды, мини-приложения (118, 144, 145, 160)

`MSG_SEND_CALLBACK(118)`, `CHAT_BOT_COMMANDS(144)`, `BOT_INFO(145)`, `WEB_APP_INIT_DATA(160)`.

`WEB_APP_INIT_DATA(160)` — параметр инициализации мини-приложения. То же, что в Telegram Mini Apps `init_data` — серверная подпись профиля для передачи внутрь WebApp. Содержит userId, имя, флаги; используется мини-приложениями для аутентификации.

### Папки (272-277)

`FOLDERS_GET(272)`, `FOLDERS_GET_BY_ID(273)`, `FOLDERS_UPDATE(274)`, `FOLDERS_REORDER(275)`, `FOLDERS_DELETE(276)`, `NOTIF_FOLDERS(277)`.

### Профиль (199, 200)

`PROFILE_DELETE(199)`, `PROFILE_DELETE_TIME(200)`. Удаление аккаунта (с задержкой `PROFILE_DELETE_TIME`).

### Серверные ассеты (25-29, 259-261)

`PRESET_AVATARS(25)`, `ASSETS_GET(26)`, `ASSETS_UPDATE(27)`, `ASSETS_GET_BY_IDS(28)`, `ASSETS_ADD(29)`, `ASSETS_REMOVE(259)`, `ASSETS_MOVE(260)`, `ASSETS_LIST_MODIFY(261)`.

«Ассеты» — пользовательские медиа-привязки (избранные стикеры, аватары и т. п.).

### Прочее

- `BANNERS_GET(302)`, `NOTIF_BANNERS(292)` — серверные баннеры/информеры в UI клиента.
- `CHAT_SUGGEST(300)` — рекомендация чатов сервером.
- `AUDIO_PLAY(301)` — телеметрия проигрывания аудио.
- `ORG_INFO(256)` — info про организацию (B2B-аккаунты).

## Что важно

1. **Опкоды показывают полный набор серверных команд клиенту.** Это не только нотификации; есть прямые команды («запроси координаты», «получи информер», «вот тебе обновлённый профиль»).
2. **`LOCATION_REQUEST(126)` существует как отдельный опкод**, в паре с push-типом `LocationRequest`. Цепочка «silent push → WS request» документирована.
3. **Серверная транскрипция (`TRANSCRIBE_MEDIA(202)`)** — альтернатива on-device ASR. То есть сервер может затребовать копию аудио для расшифровки на своей стороне.
4. **Серверный resolve URL (`LINK_INFO(89)`)** — при отображении превью ссылки в чате клиент шлёт URL на сервер. Сервер видит, какие URL пользователь упоминает.
5. **Синхронизация черновиков** (`DRAFT_SAVE(176)`, `NOTIF_DRAFT(152)`) — то, что пользователь набирает, отправляется в `api.oneme.ru` без отправки сообщения.
6. **`NOTIF_PROFILE(159)`** — сервер push-ит апдейт профиля пользователя. Кто и что меняет — определяется серверной стороной (включая, теоретически, принудительное переименование).
7. **`PHONE_WEBAPP_SHARE(106)`** + JS-bridge `verify_mobile_id` — два уровня отдачи MSISDN мини-приложениям.
8. **`OK_TOKEN(158)`** — токен для других сервисов VK/OK экосистемы. После входа в MAX тот же логин даёт доступ дальше.

Не нашёл (значит — не реализовано в этой версии или вынесено отдельно):

- E2E-handshake опкодов (нет SET_KEY, GET_KEY, генерации сессионных ключей, PreKey-bundle и т. д.).
- Опкодов для прокси/circumvention.
- Отдельных опкодов для GOST-крипто.

Это согласуется с тем, что **сквозного шифрования в протоколе нет** — все сообщения проходят через сервер в виде, который сервер обрабатывает.
