---
tags: [webapp, biometry, database, token, surveillance, jsbridge]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/crc.java
  - work/jadx_base/sources/defpackage/z3c.java
  - work/jadx_base/sources/defpackage/vm6.java
  - work/jadx_base/sources/defpackage/t54.java
  - work/jadx_base/sources/defpackage/fj3.java
  - work/jadx_base/sources/defpackage/pxa.java
related:
  - "[[420-webapp-storage-biometry]]"
  - "[[419-webapp-js-events]]"
---

# webapp_biometry — таблица биометрии мини-приложений в БД

Таблица `webapp_biometry` в SQLite базе данных MAX.

## Схема таблицы

```sql
CREATE TABLE IF NOT EXISTS `webapp_biometry` (
  `id` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  `user_id` INTEGER NOT NULL,
  `bot_id` INTEGER NOT NULL,
  `token` TEXT,
  `access_requested` INTEGER NOT NULL,
  `access_granted` INTEGER NOT NULL
)
```

## Запросы

| Запрос | Что |
|---|---|
| `SELECT * FROM webapp_biometry WHERE user_id = ?` | получить биометрию пользователя |
| `INSERT OR REPLACE INTO webapp_biometry (id,user_id,bot_id,token,access_requested,access_granted)` | сохранить |
| `UPDATE webapp_biometry SET access_requested = ?, access_granted = ? WHERE user_id = ? AND bot_id = ?` | обновить статус доступа |
| `UPDATE webapp_biometry SET token = ? WHERE user_id = ? AND bot_id = ?` | обновить токен |

## Что важно

1. **`bot_id`** — биометрия привязана к конкретному боту/мини-приложению.

2. **`token`** — биометрический токен хранится в БД. Это токен, который мини-приложение получает после успешной биометрической аутентификации.

3. **`access_requested`/`access_granted`** — статус запроса и предоставления доступа к биометрии.

4. **`user_id`** — биометрия привязана к пользователю.

5. Таблица хранится в локальной SQLite БД MAX.

## Сводка

`webapp_biometry(id, user_id, bot_id, token, access_requested, access_granted)`. Биометрический токен хранится локально, привязан к user_id + bot_id.
