---
tags: [rtd, pmskey, server-control, surveillance, debug, telemetry]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/rtd.java
related:
  - "[[03-pms-server-flags]]"
  - "[[343-rtd-server-flags]]"
  - "[[463-qp6-rtd-pmskey-full]]"
---

# rtd — RTD (Remote Toggles Database) — дополнительные поля

`rtd` — дополнительные поля RTD, не вошедшие в предыдущие топики.

## Флаги (boolean)

| Поле | Что |
|---|---|
| `isWakelockOnPushEnabled` | WakeLock при push |
| `isDraftsSyncEnabled` | синхронизация черновиков |
| `isReplaceFirebaseExecutorsEnabled` | замена Firebase executors |
| `isFullLogEnabled` | **полное логирование** |
| `isDebugProfileInfoEnabled` | **отладочная информация профиля** |
| `isFakeChatsEnabled` | **фейковые чаты** |
| `isChannelsEnabled` | каналы включены |
| `isFakeInAppReviewEnabled` | **фейковый In-App Review** |
| `isSendLocationEnabled` | отправка геолокации |
| `isAccountNicknameEnabled` | никнейм аккаунта |
| `isReconnectCallSoundEnabled` | звук переподключения звонка |
| `isMytrackerEnabled` | **MyTracker включён** |
| `isBotComplaintEnabled` | жалобы на ботов |
| `isBotStartParamEnabled` | параметры запуска бота |
| `isNetClientDnsEnabled` | **DNS клиент** |
| `isOfficialOrgEnabled` | официальные организации |
| `isInvalidateDbByMsgException` | инвалидация БД по исключению |
| `vpnChatBottomsheetEnabled` | VPN bottomsheet в чате |
| `vpnCallBottomsheetEnabled` | VPN bottomsheet в звонке |

## Строки/числа

| Поле | Что |
|---|---|
| `callServers` | **серверы звонков** |
| `supportAccount` | аккаунт поддержки |
| `supportEmail` | email поддержки |
| `userLogReportChatId` | ID чата для отчётов |
| `inviteLink`/`inviteShort`/`inviteLong`/`inviteHeader` | ссылки приглашения |
| `moneyBotId` | ID бота для платежей |
| `nonContactSyncTimeInSec` | время синхронизации не-контактов |
| `nonContactsCollectionInterval` | интервал сбора не-контактов |
| `callRateParams` | параметры оценки звонка |
| `statSessionBackgroundThreshold` | порог фоновой сессии |
| `playerControlParams` | параметры плеера |
| `channelStatsBotId` | ID бота статистики каналов |
| `uploadHangBarrierMs` | барьер зависания загрузки |
| `memorySliceIntervalMs` | интервал среза памяти |
| `publicSearchResultsLimit` | лимит публичного поиска |
| `retryTranscriptionAttempt`/`retryTranscribeTimeout` | повтор транскрипции |
| `mediaNotReadyDelay` | задержка медиа |

## Что важно

1. **`isFakeChatsEnabled`** — фейковые чаты управляются сервером.

2. **`isFullLogEnabled`** — полное логирование управляется сервером.

3. **`callServers`** — список серверов звонков с сервера.

4. **`moneyBotId`** — ID бота для платежей.

5. **`vpnChatBottomsheetEnabled`/`vpnCallBottomsheetEnabled`** — VPN-предупреждения управляются сервером.

## Сводка

`rtd`: 19 boolean флагов + 18 строк/чисел. Ключевые: `isFakeChatsEnabled`/`isFullLogEnabled`/`callServers`/`moneyBotId`/`vpnChatBottomsheetEnabled`.
