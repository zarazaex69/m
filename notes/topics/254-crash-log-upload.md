---
tags: [crash-report, logs, upload, gzip, thread-dump, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/yxb.java
related:
  - "[[17-apptracer-uplink]]"
  - "[[228-libtracernative-exports]]"
---

# Crash/Log Upload — структура загрузки логов

`yxb.java` — загрузка crash-репортов и логов на сервер.

## Структура multipart-запроса

| Часть | Что |
|---|---|
| `stackTrace` / `file` | стек трейс или файл |
| `uploadBean` | **JSON метаданные** |
| `threadDump` (`threads.gzip`) | **дамп потоков** (gzip) |
| `logs` (`logs.gzip`) | **логи приложения** (gzip) |
| `drops` (`drops.json`) | **отброшенные события** (JSON) |

## Поля uploadBean

| Поле | Что |
|---|---|
| `type` | тип репорта |
| `format` | формат |
| `severity` | **серьёзность** |
| `tags` | теги |

## Что важно

1. **`logs.gzip`** — полные логи приложения загружаются на сервер при краше. Это может включать чувствительные данные если `log-sensitive=true`.

2. **`threadDump`** — дамп всех потоков приложения. Это детальная информация о состоянии приложения.

3. **`drops.json`** — отброшенные события. Сервер знает, какие события не были доставлены.

4. **`severity`** — серьёзность репорта. Сервер классифицирует репорты.

## Сводка

Crash/log upload: stackTrace/uploadBean(type/format/severity/tags)/threadDump(gzip)/logs(gzip)/drops(json). Полные логи и дамп потоков при краше.
