---
tags: [watchdog, server-control, surveillance, crash-detection, stack-trace]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/jkc.java
  - work/jadx_base/sources/defpackage/jn0.java
related:
  - "[[463-qp6-rtd-pmskey-full]]"
  - "[[03-pms-server-flags]]"
  - "[[442-background-listen-service]]"
---

# WatchdogConfig + keepBackgroundSocket

## WatchdogConfig (jkc)

Конфигурация watchdog — мониторинг зависаний приложения.

| Поле | Что |
|---|---|
| `isEnabled` | watchdog включён |
| `stuckThreshold` | **порог "застревания"** (мс) |
| `hangThreshold` | **порог "зависания"** (мс) |
| `saveStacktrace` | **сохранять стектрейс** |
| `useShortMeta` | использовать короткие метаданные |

Конфиг приходит с сервера через PmsKey `watchdogconfig`.

## keepBackgroundSocket (jn0)

`PMS keepBackgroundSocket changed` — PmsKey управляет фоновым сокетом.

При `PMS disabled` → `force-disabling feature` — принудительное отключение.

## Что важно

1. **`saveStacktrace`** — watchdog может сохранять стектрейс при зависании. Это означает, что стектрейс отправляется на сервер.

2. **`stuckThreshold`/`hangThreshold`** — пороги задаются сервером.

3. **`keepBackgroundSocket`** — сервер управляет фоновым сокетом.

## Сводка

`WatchdogConfig(isEnabled, stuckThreshold, hangThreshold, saveStacktrace, useShortMeta)`. `keepBackgroundSocket` — PmsKey для фонового сокета.
