---
tags: [proc-files, cpu-stats, battery, surveillance, telemetry]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/k6e.java
  - work/jadx_base/sources/defpackage/ui7.java
related:
  - "[[428-battery-cpu-collector]]"
  - "[[427-db-additional-tables]]"
---

# /proc файлы — чтение системной статистики

MAX читает системные файлы `/proc` для сбора статистики.

## /proc/self/stat (k6e.java)

Читает `/proc/self/stat` — статистика процесса.

Парсит поля:
- PID
- comm (имя процесса)
- state (состояние)
- utime (user CPU time)
- stime (system CPU time)
- и другие поля (минимум 50)

## /proc/self + /data/data/ru.oneme.app (ui7.java)

```java
public static final String[] i = {"/proc/self", "/data/data/ru.oneme.app"};
```

Пути для мониторинга.

## Что важно

1. **`/proc/self/stat`** — CPU time процесса. Используется для `battery` таблицы (utime/stime).

2. **`/data/data/ru.oneme.app`** — путь к данным приложения. Используется для мониторинга.

3. Данные из `/proc` используются для `battery-slice-interval` сбора.

## Сводка

`k6e`: читает `/proc/self/stat` → PID/comm/state/utime/stime. `ui7`: пути `/proc/self` + `/data/data/ru.oneme.app`.
