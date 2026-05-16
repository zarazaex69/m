---
tags: [pmskey, server-control, debug, logging, surveillance, transcription, calls]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[03-pms-server-flags]]"
  - "[[343-rtd-server-flags]]"
  - "[[344-pmskey-debug-special]]"
---

# PmsKey — дополнительные ключи (debug, logging, transcription, calls)

Дополнительные PmsKey, не вошедшие в предыдущие топики.

## Debug и логирование

| PmsKey | Что |
|---|---|
| `debug-mode` | **режим отладки** |
| `min-log-level` | минимальный уровень логирования |
| `user-debug-report` | **отчёт отладки пользователя** |
| `android-use-logcat-logger` | использовать logcat |
| `log-full` | **полное логирование** (см. [[343-rtd-server-flags]]) |
| `log-sensitive` | **логирование чувствительных данных** |
| `log-violations` | логирование нарушений |
| `log-messages-meta` | **логирование метаданных сообщений** |
| `log-chat-meta` | **логирование метаданных чатов** |
| `debug-profile-info` | отладочная информация профиля |
| `debug-broken-contact` | отладка сломанных контактов |
| `calls-sdk-webrtc-logs` | **логи WebRTC** |
| `calls-sdk-log-audio` | **логирование аудио** |
| `presence-offline-log` | логирование offline presence |

## Транскрипция (ASR)

| PmsKey | Что |
|---|---|
| `enable-audio-messages-transcription` | **включить транскрипцию аудиосообщений** |
| `enable-video-messages-transcription` | **включить транскрипцию видеосообщений** |
| `retry-transcribe-attempt` | количество попыток транскрипции |
| `retry-transcribe-timeout` | таймаут повтора транскрипции |

## Звонки

| PmsKey | Что |
|---|---|
| `calls-sdk-wt-enabled` | WebTransport для звонков |
| `calls-use-p2p-relay-caps` | использовать P2P relay capabilities |
| `calls-android-direct-ice-restart` | прямой ICE restart |
| `calls-android-no-ice-restart` | отключить ICE restart |
| `calls-sdk-disable-pipeline` | отключить pipeline |
| `calls-sdk-dnt-disable-audio` | отключить аудио |
| `calls-android-update-endpoint-params` | обновить параметры endpoint |
| `calls-fakeboss-incoming-call-enabled` | **фейковый входящий звонок** |
| `call-ping-fix` | фикс ping звонка |
| `call-rate` | рейтинг звонка |
| `call-permissions-interval` | интервал проверки разрешений |
| `group-call-part-limit` | лимит участников группового звонка |

## Сеть и производительность

| PmsKey | Что |
|---|---|
| `ping-background-interval` | **интервал ping в фоне** |
| `disconnect-timeout` | таймаут отключения |
| `conn-timeouts` | таймауты соединений |
| `net-client-dns-enabled` | DNS клиент |
| `net-session-rbc-enabled` | RBC сессии |
| `wm-workers-limit` | лимит workers |
| `wm-check-workers-count-interval-sec` | интервал проверки workers |
| `wm-backlog-worker-check-delay-sec` | задержка проверки backlog |
| `wm-backlog-worker-backoff-delay-sec` | backoff задержка |
| `wm-analytics-enabled` | аналитика workers |
| `memory-slice-interval` | интервал среза памяти |
| `battery-slice-interval` | **интервал среза батареи** |
| `error-stat-limit` | лимит статистики ошибок |

## Аналитика и трекинг

| PmsKey | Что |
|---|---|
| `analytics-enabled` | аналитика включена |
| `mytracker-enabled` | **MyTracker включён** |
| `mytracker-log-level` | уровень логирования MyTracker |
| `tracer-non-fatal-crashed-enabled` | нефатальные крэши в трейсере |
| `stat-session-background-threshold` | порог фоновой сессии |
| `non-contact-collection-interval` | **интервал сбора не-контактов** |
| `non-contact-sync-time` | время синхронизации не-контактов |
| `reactions-sync-time` | время синхронизации реакций |

## Что важно

1. **`enable-audio-messages-transcription`/`enable-video-messages-transcription`** — сервер может включить транскрипцию аудио/видео сообщений. Это означает, что все аудио/видео сообщения могут транскрибироваться.

2. **`log-sensitive`/`log-messages-meta`/`log-chat-meta`** — сервер может включить логирование чувствительных данных, метаданных сообщений и чатов.

3. **`calls-fakeboss-incoming-call-enabled`** — фейковый входящий звонок. Возможно для тестирования или обхода ограничений.

4. **`mytracker-enabled`** — MyTracker можно отключить с сервера.

5. **`battery-slice-interval`** — сервер контролирует интервал сбора данных о батарее.

6. **`non-contact-collection-interval`** — сервер контролирует интервал сбора данных о не-контактах.

## Сводка

Критические: `enable-audio-messages-transcription`/`enable-video-messages-transcription` (транскрипция сообщений), `log-sensitive`/`log-messages-meta`/`log-chat-meta` (логирование), `calls-fakeboss-incoming-call-enabled` (фейковый звонок), `mytracker-enabled`, `battery-slice-interval`.
