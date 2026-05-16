---
tags: [ws, protocol, telemetry, server-control, session]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ubi.java
related:
  - "[[33-ws-msgpack-framing]]"
  - "[[20-ws-protocol-opcodes]]"
  - "[[03-pms-server-flags]]"
  - "[[52-network-session-tls-flags]]"
  - "[[53-ab-status-cfs-pmskey]]"
---

# WS session config — что клиент отправляет серверу при каждом подключении

`defpackage/ubi.java` формирует WS-сессионный конфиг — набор key-value пар (`u6d` объектов), которые клиент отправляет серверу при установке WS-соединения. Это **диагностический fingerprint клиента**, который сервер получает при каждом подключении.

## Полный список полей (из ubi.java)

| Ключ | Что отправляется |
|---|---|
| `net-session-suppress-bad-disconnected-state` | bool — текущее значение PmsKey |
| `net-ssl-session-validate` | bool — включена ли TLS-валидация (см. [[52-network-session-tls-flags]]) |
| `one-video-player` | bool — включён ли новый видеоплеер |
| `one-video-uploader-config` | JSON-строка конфига загрузчика видео |
| `video-content-cache-ttl` | long — TTL кэша видео |
| `ab-status` | long — A/B-тест группа пользователя (см. [[53-ab-status-cfs-pmskey]]) |
| `set-audio-device` | bool — включён ли выбор аудиоустройства |
| `ringtone-content-type` | long — тип рингтона |
| `calls-sdk-disable-pipeline` | bool — отключён ли APM pipeline |
| `calls-sdk-log-audio` | bool — включено ли логирование аудио |
| `calls-sdk-dnt-disable-audio` | bool — отключена ли передача аудио |
| `chat-history-warm-opts` | int — опции прогрева истории чатов |
| `chat-history-warm-fail-interval` | int — интервал при ошибке прогрева |
| `chat-history-notif-msg-strategy` | int — стратегия уведомлений для истории |
| `db-tr-ex-count` | int — счётчик исключений транзакций БД |
| `db-query-ex-count` | int — счётчик исключений запросов БД |
| `chat-history-persist` | bool — персистентность истории чатов |
| `calls-sdk-opus-adapt` | bool — адаптивный Opus |
| `presence-view-port` | string — viewport для presence |
| `enable-audio-messages-transcription` | bool — включена ли транскрипция аудио |
| `enable-video-messages-transcription` | bool — включена ли транскрипция видео |

## Что важно

1. **Сервер получает полный snapshot состояния клиента при каждом подключении** — 21 поле, включая счётчики ошибок БД, состояние TLS-валидации, A/B-группу, включённость логирования аудио.

2. **`db-tr-ex-count` и `db-query-ex-count`** — счётчики исключений в базе данных. Сервер знает, сколько раз у клиента падала БД. Это диагностика, но также означает, что сервер видит нестабильность клиента.

3. **`calls-sdk-log-audio`** — сервер видит, включено ли логирование аудио у клиента. Если сервер сам включил этот флаг через PmsKey, он получит подтверждение, что флаг применился.

4. **`net-ssl-session-validate`** — сервер видит, проверяет ли клиент TLS. Если сервер отключил валидацию через PmsKey, он получит подтверждение.

5. **`ab-status`** — сервер видит, в какой A/B-группе клиент. Это позволяет серверу анализировать поведение по группам.

6. **`enable-audio-messages-transcription` и `enable-video-messages-transcription`** — сервер знает, включена ли транскрипция у клиента. Это важно: если транскрипция включена, сервер знает, что голосовые/видео сообщения будут транскрибированы (и, возможно, отправлены на сервер для транскрипции).

## Сводка

При каждом WS-подключении клиент отправляет 21-поле диагностический fingerprint: состояние TLS, A/B-группа, счётчики ошибок БД, состояние логирования аудио, включённость транскрипции. Сервер получает полный snapshot конфигурации клиента.
