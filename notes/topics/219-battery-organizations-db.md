---
tags: [database, battery, organizations, schema, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/geb.java
related:
  - "[[216-local-db-tables]]"
  - "[[126-energy-saving-pmskey]]"
---

# battery + organizations DB схемы

## battery

| Поле | Что |
|---|---|
| `id` | PK |
| `sliceTime` | **временной срез** |
| `utime` | user time (CPU) |
| `stime` | system time (CPU) |
| `batteryCapacity` | **ёмкость батареи** |
| `instantAmperage` | **мгновенный ток** |

Более новая версия: `sliceTime` + `payload` (BLOB).

## organizations

| Поле | Что |
|---|---|
| `id` | ID организации |
| `name` | название |
| `description` | описание |
| `parentId` | родительская организация |
| `folderTemplateId` | шаблон папки |
| `updateTime` | время обновления |

## Что важно

1. **`battery`** — хранит `utime`/`stime` (CPU time), `batteryCapacity`, `instantAmperage`. Это детальная статистика энергопотребления. Связано с [[126-energy-saving-pmskey]] — сервер давит на пользователя отключить энергосбережение, одновременно собирая данные о батарее.

2. **`organizations`** — иерархическая структура организаций (`parentId`). Это корпоративная функция MAX.

## Сводка

`battery`: sliceTime/utime/stime/batteryCapacity/instantAmperage — детальная статистика энергопотребления. `organizations`: иерархия с parentId/folderTemplateId.
