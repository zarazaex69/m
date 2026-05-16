---
tags: [logging, server-control, pms, sensitive-data, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ul9.java
  - work/jadx_base/sources/defpackage/rtd.java
  - work/jadx_base/sources/defpackage/nd1.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[03-pms-server-flags]]"
  - "[[04-telemetry-endpoints]]"
  - "[[17-apptracer-uplink]]"
  - "[[31-onelog-event-categories]]"
---

# Server-controlled logging flags — log-full, log-sensitive, log-messages-meta

MAX содержит несколько PmsKey, которые управляют **уровнем детализации логирования** — включая логирование чувствительных данных.

## PmsKey

| Ключ | # | Что |
|---|---|---|
| `log-full` | (в rtd.java) | полное логирование всех событий (verbose mode) |
| `log-sensitive` | (в rtd.java) | логировать чувствительные данные (токены, контакты) |
| `log-messages-meta` | (в nd1.java) | логировать метаданные сообщений |
| `log-chat-meta` | (в nd1.java) | логировать метаданные чатов |
| `log-violations` | (в PmsKey) | логировать нарушения политик |
| `calls-sdk-webrtc-logs` | 103 | WebRTC-логи звонков |
| `calls-sdk-log-audio` | 131 | логировать аудио-захват (см. [[45-calls-sdk-pmskey-cluster]]) |

## Что делает log-sensitive

В `ul9.java` (LOGIN.Response handler) при `log-sensitive=true` токен аутентификации (`token`) **маскируется** (`ti3.K(str)` — вероятно, hash или `***`). При `log-sensitive=false` — токен логируется в открытом виде.

Это инвертированная семантика: `log-sensitive=true` означает «скрывать чувствительное», а не «логировать чувствительное». Название вводит в заблуждение, но логика правильная.

Однако: `log-full=true` включает полное логирование, включая `contactInfos` (список контактов из LOGIN.Response). При `log-full=true` и `log-sensitive=false` — в logcat попадают контакты пользователя и токен.

## Что делает log-messages-meta

`nd1.java:129` использует `PmsKey.f155logmessagesmeta`. По имени — логирование метаданных сообщений: sender_id, chat_id, timestamp, message_id. Не содержимое, но метаданные. Это то, что уходит в OneLog (см. [[31-onelog-event-categories]]).

## allowLogSensitiveData — SharedPreferences

В `ri9.java` (SharedPreferences) есть поле `allowLogSensitiveData` как `MutableStateFlow<Boolean>`. Это **локальный пользовательский флаг** (в dev-меню, см. [[05-dev-menu-in-prod]]), который разрешает логирование чувствительных данных. Отдельно от серверного PmsKey `log-sensitive`.

Итого два независимых канала включения sensitive-логирования: серверный PmsKey и локальный dev-флаг.

## Что важно

1. **Сервер может включить `log-full` для конкретного пользователя** через PmsKey-сегментацию. При этом в logcat (и потенциально в apptracer-uplink через `tracer_log`, см. [[17-apptracer-uplink]]) начнут попадать контакты, токены, метаданные сообщений.

2. **`log-messages-meta` + `log-chat-meta`** — серверно-включаемое логирование метаданных переписки. Это не содержимое сообщений, но граф общения (кто с кем, когда, в каком чате).

3. **`log-violations`** — логирование «нарушений политик». Что считается нарушением — определяется сервером. Это может быть попытка использовать VPN, root-устройство, нестандартный клиент.

4. **Связь с apptracer**: `tracer_log` в `libtracernative.so` (см. [[17-apptracer-uplink]]) принимает произвольные строки и отправляет на `sdk-api.apptracer.ru`. Если `log-full=true` включает verbose-логирование, а apptracer перехватывает logcat — чувствительные данные могут уйти на сервер.

## Скептический разбор

- `log-full` и `log-sensitive` — стандартная практика для диагностики. Все крупные приложения имеют такие флаги.
- Что специфично: флаги управляются **сервером через PmsKey**, а не только через dev-меню. Сервер может включить verbose-логирование для конкретного пользователя без его ведома.
- Нет подтверждения, что apptracer автоматически перехватывает logcat. Это нужно проверить отдельно.

## Сводка

Четыре PmsKey управляют уровнем логирования: `log-full` (verbose), `log-sensitive` (маскировка токенов), `log-messages-meta` (метаданные сообщений), `log-chat-meta` (метаданные чатов). Все server-controlled. При `log-full=true` и `log-sensitive=false` в logcat попадают контакты и токен аутентификации.
