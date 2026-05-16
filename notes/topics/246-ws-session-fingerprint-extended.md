---
tags: [protocol, ws, session-fingerprint, server-control, calls, transcription]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ubi.java
related:
  - "[[57-ws-session-config-fingerprint]]"
  - "[[235-qp6-additional-pmskeys]]"
---

# WS Session Fingerprint — дополнительные поля (ubi.java)

Дополнение к [[57-ws-session-config-fingerprint]]. Новые поля WS-сессии из `ubi.java`.

## Дополнительные поля

| Поле | Что |
|---|---|
| `net-session-suppress-bad-disconnected-state` | подавить плохое состояние отключения |
| `net-ssl-session-validate` | валидация SSL-сессии |
| `one-video-player` | конфигурация видеоплеера |
| `one-video-uploader-config` | конфигурация загрузчика видео |
| `video-content-cache-ttl` | TTL кэша видео |
| `ab-status` | A/B статус |
| `set-audio-device` | установить аудио-устройство |
| `ringtone-content-type` | тип контента рингтона |
| `calls-sdk-disable-pipeline` | отключить pipeline звонков |
| `calls-sdk-log-audio` | **логировать аудио звонков** |
| `calls-sdk-dnt-disable-audio` | не отключать аудио |
| `chat-history-warm-opts` | опции прогрева истории |
| `chat-history-warm-fail-interval` | интервал при неудаче прогрева |
| `chat-history-notif-msg-strategy` | стратегия уведомлений истории |
| `db-tr-ex-count` | количество потоков транзакций БД |
| `db-query-ex-count` | количество потоков запросов БД |
| `chat-history-persist` | персистентность истории |
| `calls-sdk-opus-adapt` | адаптивный Opus |
| `presence-view-port` | viewport presence |
| `enable-audio-messages-transcription` | **включить транскрипцию аудио-сообщений** |
| `enable-video-messages-transcription` | **включить транскрипцию видео-сообщений** |

## Что важно

1. **`enable-audio-messages-transcription`/`enable-video-messages-transcription`** — сервер управляет транскрипцией аудио и видео сообщений через WS session.

2. **`calls-sdk-log-audio`** — сервер может включить логирование аудио звонков через WS session.

3. Итого WS session fingerprint содержит 21+ полей (топик 57) + 21 новых = **40+ полей**.

## Сводка

Дополнительные WS session поля: `enable-audio-messages-transcription`/`enable-video-messages-transcription`/`calls-sdk-log-audio`/`ab-status`/`one-video-player`/`one-video-uploader-config` и др.
