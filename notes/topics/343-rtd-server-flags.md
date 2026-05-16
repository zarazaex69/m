---
tags: [pmskey, server-control, rtd, surveillance, mytracker, transcription]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/rtd.java
related:
  - "[[03-pms-server-flags]]"
  - "[[342-pmskey-additional-new]]"
---

# rtd SharedPreferences — полный список серверных флагов

`rtd.java` — ещё один SharedPreferences с серверными флагами. Дополнение к [[03-pms-server-flags]].

## Ключевые флаги

| Флаг | Что |
|---|---|
| `callServers` | серверы звонков |
| `supportAccount` / `supportEmail` | аккаунт/email поддержки |
| `isWakelockOnPushEnabled` | wakelock при push |
| `maxVideoDurationDownload` | максимальная длительность видео |
| `userLogReportChatId` | **ID чата для отчётов логов** |
| `useLogcatLogger` | использовать logcat |
| `isDraftsSyncEnabled` | синхронизация черновиков |
| `inviteLink` / `inviteShort` / `inviteLong` / `inviteHeader` | ссылки приглашения |
| `isReplaceFirebaseExecutorsEnabled` | замена Firebase executors |
| `isFullLogEnabled` | полное логирование |
| `needToLogSensitive` | логировать чувствительные данные |
| `isMytrackerEnabled` | **MyTracker включён** |
| `statSessionBackgroundThreshold` | **порог фоновой сессии** |
| `retryTranscriptionAttempt` / `retryTranscribeTimeout` | повторные попытки транскрипции |
| `mediaNotReadyDelay` | задержка при недоступности медиа |
| `vpnChatBottomsheetEnabled` / `vpnCallBottomsheetEnabled` | VPN bottomsheet |
| `uploadHangBarrierMs` | барьер зависания загрузки |
| `memorySliceIntervalMs` | интервал среза памяти |
| `publicSearchResultsLimit` | лимит публичного поиска |

## Что важно

1. **`userLogReportChatId`** — ID чата для отчётов логов. Сервер может указать чат, куда отправлять логи.

2. **`statSessionBackgroundThreshold`** — порог для определения фоновой сессии.

3. **`isMytrackerEnabled`** — сервер может отключить MyTracker.

## Сводка

`rtd` SharedPreferences: 40+ флагов. Ключевые: `userLogReportChatId`/`isMytrackerEnabled`/`statSessionBackgroundThreshold`/`retryTranscriptionAttempt`.
