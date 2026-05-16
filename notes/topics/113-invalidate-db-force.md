---
tags: [database, server-control, pms, destructive]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/jr8.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[03-pms-server-flags]]"
  - "[[12-force-update-killswitch]]"
---

# invalidate-db-force и invalidate-db-msg-exception — серверная инвалидация БД

## invalidate-db-force (#141)

`PmsKey.f141invalidatedbforce` — конфиг принудительной инвалидации базы данных. Структура `m94`:
- `a` — bool: инвалидировать ли
- `b` — int: версия для инвалидации

Логика в `jr8.java`: если `invalidateDbByException=true` (была ошибка БД) ИЛИ `invalidate-db-force` задаёт новую версию > текущей — **принудительно инвалидировать БД**.

При инвалидации: «WARNING! Invalidate db start. Cause was force invalidate: true/false, curVer: X, configVer: Y».

## invalidate-db-msg-exception (#142)

`PmsKey.f142invalidatedbmsgexception` — bool. При `true` — инвалидировать БД при исключении в обработке сообщений.

## Что важно

1. **`invalidate-db-force`** — сервер может принудительно инвалидировать локальную базу данных клиента. Это означает, что **сервер может удалить всю локальную историю сообщений** пользователя.

2. **Версионирование** — инвалидация происходит только если `configVer > curVer`. Сервер увеличивает версию для триггера инвалидации.

3. **`invalidateDbByException`** в `yag.java` — флаг, что БД была инвалидирована из-за исключения. Сервер знает о проблемах с БД клиента.

## Сводка

`invalidate-db-force` — сервер может принудительно инвалидировать (удалить) локальную БД клиента через версионирование. `invalidate-db-msg-exception` — инвалидация при ошибке обработки сообщений.
