---
tags: [telemetry, server-control, pms, opcodes, session-stats]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/tvc.java
  - work/apktool_base/smali/lsb.smali
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[03-pms-server-flags]]"
  - "[[14-stat-prefs-metrics]]"
  - "[[20-ws-protocol-opcodes]]"
---

# opcode-stat-config и session stats — статистика WS-опкодов

## opcode-stat-config (#216)

`PmsKey.f216opcodestatconfig` — long. Конфиг сбора статистики по WS-опкодам. Accessor: `qp6.opcodeStatConfig`.

## session.opcode_stats

`tvc.java` — SharedPreferences `session.opcode_stats` — хранит статистику опкодов за сессию. Это часть `StatPrefs` (см. [[14-stat-prefs-metrics]]).

Поля `tvc`:
- `opcodeStats` — статистика опкодов (JSON-строка)
- `fileOpenStats` — статистика открытия файлов
- `phonebookSize` — размер телефонной книги
- `anrDetected` — был ли ANR
- `caughtExceptionCount` — количество пойманных исключений
- `crashDetected` — был ли краш
- `frescoStats` — статистика Fresco (image loader)
- `appClockDump` — дамп часов приложения

## Что важно

1. **`opcodeStats`** — статистика по WS-опкодам за сессию. Сервер знает, какие опкоды использовались, как часто, и с какой задержкой.

2. **`anrDetected`** — сервер знает, было ли ANR в сессии.

3. **`caughtExceptionCount`** — количество пойманных исключений. Сервер знает о нестабильности клиента.

4. **`phonebookSize`** — размер телефонной книги (уже в [[14-stat-prefs-metrics]]).

5. **`appClockDump`** — дамп часов приложения. Это может включать информацию о времени запуска, времени работы, и т.п.

## Сводка

`opcode-stat-config` управляет сбором статистики WS-опкодов. `session.opcode_stats` хранит per-session статистику: опкоды, ANR, исключения, размер телефонной книги, Fresco stats, app clock dump.
