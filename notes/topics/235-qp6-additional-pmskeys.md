---
tags: [pmskey, server-control, calls, presence, gost, live-streams]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/qp6.java
related:
  - "[[03-pms-server-flags]]"
  - "[[224-yag-shared-prefs-full]]"
---

# qp6 PmsKey — дополнительные серверные флаги

Дополнение к [[03-pms-server-flags]]. Новые PmsKey из `qp6.java`, не вошедшие в основной список.

## Новые флаги (выборка)

| Флаг | Что |
|---|---|
| `gostEnvironmentCheckFlags` | **флаги проверки среды ГОСТ** |
| `digitalBotId` | **ID бота DigitalID** |
| `liveStreamsEnabled` / `liveStreamsUrlPrefix` | **live-стримы** |
| `callsTrafficMarkers` | маркеры трафика звонков |
| `callsWtToWsFallbackParams` | параметры fallback WebTransport→WS |
| `callsSignalingTimeouts` | таймауты сигналинга |
| `callsAudioFormatConfig` | конфигурация аудио-формата |
| `callsIceCandidatePoolSize` | размер пула ICE кандидатов |
| `callStatsNoiseLevel` | уровень шума для статистики |
| `isSignalingByIPEnabled` | сигналинг по IP |
| `isWebRtcLoggingEnabled` | **логирование WebRTC** |
| `isCallsAudioLoggingEnabled` | **логирование аудио звонков** |
| `migrateUnsafeWarn` | предупреждение о небезопасной миграции |
| `shouldShowVpnSnackbar` | показывать VPN-снэкбар |
| `useWebAppPhoneHash` | хэш телефона для WebApp |
| `availableBotForPrBridge` | боты для PR-bridge |
| `batterySliceIntervalMs` | интервал среза батареи |
| `presenceTtl` / `presenceViewPortEnabled` | TTL presence |
| `requestPresenceForExternal` | запрашивать presence для внешних |
| `isPresenceStatEnabled` | статистика presence |
| `phonePrivacySettingEnabled` | настройка приватности телефона |
| `isTwoAccountMvpEnabled` | два аккаунта |
| `callFakeBossesEnabled` | FakeBoss в звонках |
| `minFreeAvailableSpaceMb` | минимум свободного места |
| `opusRecorderBitrate` / `opusRecorderSampleRate` | параметры Opus-записи |
| `isLogVisibleMessagesMetaEnabled` / `isLogChatMetaEnabled` | **логирование метаданных сообщений** |

## Что важно

1. **`isWebRtcLoggingEnabled`/`isCallsAudioLoggingEnabled`** — сервер может включить детальное логирование WebRTC и аудио.

2. **`isLogVisibleMessagesMetaEnabled`/`isLogChatMetaEnabled`** — сервер может включить логирование метаданных видимых сообщений и чатов.

3. **`gostEnvironmentCheckFlags`** — битовые флаги проверки среды ГОСТ.

4. **`liveStreamsEnabled`/`liveStreamsUrlPrefix`** — live-стримы управляются сервером.

5. **`useWebAppPhoneHash`** — хэш телефона для WebApp. Связано с [[10-webapp-jsbridge]].

## Сводка

Дополнительные PmsKey: `gostEnvironmentCheckFlags`, `isWebRtcLoggingEnabled`/`isCallsAudioLoggingEnabled`, `isLogVisibleMessagesMetaEnabled`/`isLogChatMetaEnabled`, `liveStreamsEnabled`, `callsTrafficMarkers`, `batterySliceIntervalMs`.
