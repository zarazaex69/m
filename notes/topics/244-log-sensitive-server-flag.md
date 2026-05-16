---
tags: [server-control, logging, sensitive-data, surveillance, login]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ul9.java
  - work/jadx_base/sources/defpackage/rtd.java
related:
  - "[[234-ri9-shared-prefs]]"
  - "[[03-pms-server-flags]]"
---

# log-sensitive / log-full — серверное управление логированием

Сервер может включить детальное логирование через флаги в ответе на LOGIN.

## Флаги

| Флаг | Что |
|---|---|
| `log-full` | **полное логирование** (включая контакты) |
| `log-sensitive` | **логирование чувствительных данных** (токены, телефоны) |

## Механизм

В ответе на LOGIN сервер может передать `log-full=true` и/или `log-sensitive=true`.

При `log-sensitive=true`:
- Токен авторизации логируется в открытом виде (иначе маскируется через `ti3.K(str)`)
- Контакты логируются полностью

При `log-full=true`:
- Полное логирование контактов

## PmsKey

`PmsKey.f156logsensitive` — серверный флаг `log-sensitive` также управляется через PmsKey.

## Что важно

1. **Сервер может включить логирование токена авторизации** через `log-sensitive=true` в ответе на LOGIN.

2. **`log-full`** — полное логирование контактов. Сервер может получить полный список контактов через логи.

3. Это подтверждение [[234-ri9-shared-prefs]] — `allowLogSensitiveData` управляется сервером.

## Сводка

`log-sensitive`/`log-full` в ответе LOGIN — сервер включает логирование токена и контактов. PmsKey `logsensitive` — то же через PmsKey.
