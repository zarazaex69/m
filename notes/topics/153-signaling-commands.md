---
tags: [calls, signaling, commands, server-control, webrtc]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/qyf.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/asr_online/internal/commands/AsrOnlineCommandsExecutorImpl.java
related:
  - "[[152-asr-online-manager]]"
  - "[[136-ev1-webrtc-session-config]]"
  - "[[43-libjingle-webrtc-custom-build]]"
---

# Signaling commands — команды звонкового сигналинга

Звонковый SDK использует JSON-команды через сигналинг-канал. Полный список команд:

| Команда | Что |
|---|---|
| `accept-call` | принять звонок |
| `accept-promotion` | принять повышение роли |
| `add-movie` | добавить видео (watch together) |
| `change-media-settings` | изменить медиа-настройки |
| `change-simulcast` | изменить simulcast |
| `enable-feature-for-roles` | **включить фичу для ролей** |
| `get-hand-queue` | получить очередь поднятых рук |
| `get-participant-list-chunk` | получить чанк списка участников |
| `get-rooms` | получить список комнат |
| `hangup` | завершить звонок |
| `mute-participant` | **заглушить участника** |
| `record-stop` | остановить запись |
| `remove-movie` | удалить видео |
| `report-network-stat` | отчёт о сетевой статистике |
| `report-perf-stat` | отчёт о производительности |
| `request-asr` | **запросить онлайн-транскрипцию** |
| `request-promotion` | запросить повышение роли |
| `start-url-sharing` | начать шаринг URL |
| `stop-url-sharing` | остановить шаринг URL |
| `switch-room` | переключить комнату |
| `update-display-layout` | обновить layout отображения |
| `update-media-modifiers` | обновить медиа-модификаторы |
| `update-movie` | обновить видео |
| `update-rooms` | обновить комнаты |

## Что важно

1. **`mute-participant`** — администратор может заглушить участника через сигналинг. Это server-side команда.

2. **`request-asr`** — запрос онлайн-транскрипции (см. [[152-asr-online-manager]]). Содержит `start=true/false`.

3. **`enable-feature-for-roles`** — включить фичу для определённых ролей. Это server-controlled feature gating на уровне звонка.

4. **`report-network-stat`** — отчёт о сетевой статистике с `timestamp` и `bitrate`. Клиент периодически отправляет сетевую статистику.

5. **`report-perf-stat`** — отчёт о производительности.

## Сводка

24 сигналинг-команды. Ключевые: `mute-participant` (заглушить участника), `request-asr` (онлайн-транскрипция), `enable-feature-for-roles` (feature gating по ролям), `report-network-stat`/`report-perf-stat` (телеметрия).
