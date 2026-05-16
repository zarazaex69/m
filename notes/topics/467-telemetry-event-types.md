---
tags: [telemetry, event-types, server-control, surveillance, opcode-stat, memory-stat, battery-stat]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/jm5.java
related:
  - "[[465-devnull-server-config]]"
  - "[[466-memory-stat]]"
  - "[[428-battery-cpu-collector]]"
---

# jm5 — Типы событий телеметрии

`jm5` — enum типов событий телеметрии. Управляются через `DevNullServerConfig`.

## Типы событий

| Тип | Ключ | Что |
|---|---|---|
| `STARTUP_REPORT` | `startup_report` | **отчёт о запуске** |
| `AB_EVENT` | `ab_event` | **A/B тестирование** |
| `OPCODE` | `opcode` | **статистика опкодов WS** |
| `CHAT_HISTORY_WARM` | `ch_history` | **прогрев истории чатов** |
| `CHAT_LIST` | `open_chats_to_render` | **открытие списка чатов** |
| `WEB_APP` | `web_app` | **события WebApp** |
| `UPLOAD_HANG` | `upload_hang` | **зависание загрузки** |
| `UPLOAD_ERROR` | `upload_error` | **ошибки загрузки** |
| `MEMORY` | `memory` | **статистика памяти** |
| `BATTERY` | `battery` | **статистика батареи** |
| `TRANSCODE` | `transcode` | транскодирование видео |
| `BAD_PUSHES` | `bad_pushes` | **плохие push-уведомления** |

## Что важно

1. **`OPCODE`** — сервер может включить сбор статистики по всем WS-опкодам.

2. **`STARTUP_REPORT`** — отчёт о каждом запуске приложения.

3. **`WEB_APP`** — события мини-приложений.

4. **`BAD_PUSHES`** — статистика плохих push-уведомлений.

5. **`CHAT_HISTORY_WARM`/`CHAT_LIST`** — статистика производительности UI.

## Сводка

12 типов событий: STARTUP_REPORT/AB_EVENT/OPCODE/CHAT_HISTORY_WARM/CHAT_LIST/WEB_APP/UPLOAD_HANG/UPLOAD_ERROR/MEMORY/BATTERY/TRANSCODE/BAD_PUSHES.
