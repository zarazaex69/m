---
tags: [user-settings, government, gost, family-protection, digital-id, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/sgj.java
  - work/jadx_base/sources/defpackage/ma4.java
  - findings/raw/pms_keys.txt
related:
  - "[[03-pms-server-flags]]"
  - "[[15-on-device-asr-kws-diarization]]"
  - "[[13-deeplinks-idp]]"
---

# Серверно-управляемые «системные боты» и пользовательский профиль с гос-фильтрами

## UserSettings — что приходит с сервера

`defpackage/sgj.java` (модель `UserSettings`, видимо protobuf-derived) — сюда сервер кладёт настройки аккаунта. Поля по `toString()`:

```
pushNewContacts, dontDustirbUntil,
dialogsPushNotification, chatsPushNotification, pushSound, dialogsPushSound, chatsPushSound,
hiddenOnline, led, dialogsLed, chatsLed, vibration, dialogsVibration, chatsVibration,
chatsInvite, incomingCall, inactiveTtl,
groupChatCallNotificationStatus, suggestStickersStatus, audioTranscriptionEnabled,
safeMode, safeModeNoPin,
searchByPhone, unsafeFiles, contentLevelAccess,
familyProtection, phoneNumberPrivacy
```

Особо интересны:

- **`safeMode` / `safeModeNoPin`** — «безопасный режим» (видимо фильтрация контента). `NoPin` — режим без PIN-разблокировки.
- **`contentLevelAccess`** — уровень доступа к контенту. В контексте РФ — возрастной ценз / фильтрация запрещённого контента, регулируемая со стороны сервера и не выключаемая локально.
- **`familyProtection`** — режим «семейной защиты», синхронизированный с сервером.
- **`phoneNumberPrivacy`** — режим скрытия номера телефона.
- **`searchByPhone`** — позволять ли искать аккаунт по номеру.
- **`unsafeFiles`** — серверно блокируется загрузка «небезопасных» файлов (видимо exe/apk и им подобное).
- **`audioTranscriptionEnabled`** — включена ли расшифровка аудио на сервере.

Эти поля **приходят с сервера** в едином объекте UserSettings и **синхронизированы между устройствами**. То есть если, скажем, гос-сервис включает у пользователя `familyProtection` или меняет `contentLevelAccess` — это применяется ко всем его клиентам.

## «Системные боты»

В PMS-флагах и `qp6.java` есть отдельные ID для системных ботов (Long), которые меняются с сервера:

```java
io6.e(qp6.class, "familyProtectionBotId",   "getFamilyProtectionBotId()J", 0),
io6.e(qp6.class, "digitalBotId",            "getDigitalBotId()J", 0),
io6.e(qp6.class, "stickerBotId",            "getStickerBotId()J", 0),
io6.e(qp6.class, "channelStatisticsBotId",  "getChannelStatisticsBotId()J", 0),
io6.e(qp6.class, "moneyTransferBotId",      "getMoneyTransferBotId()J", 0),
```

Соответствующие PmsKey:

- `family-protection-botid`
- `digitalid-botid`
- `stickers-botid`
- `channel-statistics-botid`
- `money-transfer-botid`

То есть в чате с пользователем могут появляться **бот «Семейная защита»** и **бот «Цифровой ID»** (DigitalID — государственный сервис цифровой идентификации). Их identityID задаётся **сервером** в рантайме, что даёт двойное преимущество: можно поменять «оператора» сервиса без апдейта приложения, и исследователю не вытащить URL/ID из APK напрямую.

Обработка отдельная — UI вокруг этих ботов прописан в коде (`defpackage/swe.java`, `wze.java` и т.п. — там используется `getFamilyProtectionBotId`).

## ГОСТ-окружение

PmsKey `gost-check-env` → `gostEnvironmentCheckFlags: Int`. Используется в коде в виде int-флагов; сами проверки (`gostLicenseCheckEnabled` и т.п.) — внутри.

Что это значит на практике: код имеет ветку, в которой при определённом серверном значении он начинает работать иначе — видимо, переключаться на ГОСТ-крипто (или, наоборот, проверять, что ГОСТ-провайдер **не активирован**). Конкретные действия в коде разнесены по нескольким классам и не вытащены чисто в один файл. Сам факт зашитого «gost env check» — характерный для российских корпоративных Android-приложений хук под пакеты типа КриптоПро/ВипНет.

## Кратко

- В аккаунте каждого пользователя у MAX сервер хранит флаги `safeMode`, `contentLevelAccess`, `familyProtection`, `phoneNumberPrivacy`, `unsafeFiles`. Они приходят в `UserSettings` единым объектом, синхронизируются между устройствами, и на стороне клиента нет публичной ручки, чтобы их выключить (нет UI-toggles по большей части).
- Системные боты — «Семейная защита», «Цифровой ID Россия» — встроены архитектурно: их chat-id серверно-управляемый, и UI вокруг них уже зашит.
- Hook под ГОСТ-окружение есть в коде в виде серверного флага. Конкретный тригерный путь — отдельно (видимо, активируется ГОСТ-провайдером в определённых сценариях, например при обращении к системам, требующим ГОСТ-крипто).

Это делает MAX **не просто мессенджером, а универсальным контейнером**, в который российская власть может вкатывать функциональность «прямо в чат» — и менять её без апдейта приложения.
