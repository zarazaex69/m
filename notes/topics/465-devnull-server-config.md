---
tags: [devnull-server-config, server-control, surveillance, telemetry, opcode-stat, memory-stat, battery-stat]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/mm5.java
related:
  - "[[463-qp6-rtd-pmskey-full]]"
  - "[[428-battery-cpu-collector]]"
  - "[[03-pms-server-flags]]"
---

# mm5 — DevNullServerConfig (серверный конфиг событий)

`mm5` — `DevNullServerConfig` — конфигурация событий от сервера. Управляет, какие события собираются.

## Флаги событий

| Флаг | Что |
|---|---|
| `isAbEventEnabled` | **A/B тестирование** |
| `isOpcodeStatEnabled` | **статистика опкодов** |
| `isChatHistoryStatEnabled` | **статистика истории чатов** |
| `isUploadHangCheckEnabled` | **проверка зависания загрузки** |
| `isUploadErrorEventEnabled` | **события ошибок загрузки** |
| `isMemoryStatEnabled` | **статистика памяти** |
| `isBatteryStatEnabled` | **статистика батареи** |
| `isVideoTranscodeSizeRegressionEnabled` | регрессия размера транскодирования видео |

## Что важно

1. **`isOpcodeStatEnabled`** — сервер может включить сбор статистики по всем опкодам WS-протокола.

2. **`isMemoryStatEnabled`** — сервер может включить сбор статистики памяти.

3. **`isBatteryStatEnabled`** — сервер может включить сбор статистики батареи.

4. **`isChatHistoryStatEnabled`** — сервер может включить статистику истории чатов.

5. **`isUploadHangCheckEnabled`** — проверка зависания загрузки файлов.

## Сводка

`DevNullServerConfig`: 8 флагов событий. Ключевые: `isOpcodeStatEnabled`/`isMemoryStatEnabled`/`isBatteryStatEnabled`/`isChatHistoryStatEnabled`.
