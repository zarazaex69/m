---
tags: [database, tasks, recent, schema]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/crc.java
  - work/jadx_base/sources/defpackage/sk.java
related:
  - "[[205-tasks-proto]]"
  - "[[216-local-db-tables]]"
---

# tasks + recent DB схемы

## tasks

| Поле | Что |
|---|---|
| `id` | ID задачи |
| `type` | тип задачи |
| `status` | статус |
| `fails_count` | счётчик неудач |
| `depends_request_id` | зависимость от запроса |
| `dependency_type` | тип зависимости |
| `data` | **BLOB — данные задачи** (protobuf) |
| `created_time` | время создания |

`INSERT OR IGNORE` — задача не перезаписывается если уже существует.

## recent

| Поле | Что |
|---|---|
| `id` | PK |
| `recent_type` | тип (стикер/emoji/GIF) |
| `recent_time` | время использования |
| `server_id` | серверный ID |
| `sticker_id` | ID стикера |
| `emoji` | emoji |
| `gif` | **BLOB — GIF данные** |
| `gif_id` | ID GIF |

## Что важно

1. **`tasks.data`** — BLOB с protobuf-данными задачи. Это сериализованные `Tasks.*` (см. [[205-tasks-proto]]).

2. **`tasks.fails_count`** — счётчик неудач. Задачи повторяются при неудаче.

3. **`recent.gif`** — GIF хранится как BLOB локально. Это кэш недавно использованных GIF.

## Сводка

`tasks`: id/type/status/fails_count/data(BLOB protobuf)/created_time. `recent`: recent_type/sticker_id/emoji/gif(BLOB)/gif_id.
