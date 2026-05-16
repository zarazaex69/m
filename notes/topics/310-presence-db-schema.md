---
tags: [database, presence, schema, surveillance, last-seen]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/crc.java
  - work/jadx_base/sources/defpackage/d49.java
related:
  - "[[36-presence-server-controlled]]"
  - "[[308-nm4-contact-presence-parser]]"
---

# presence DB схема — статус присутствия

`presence` — таблица статусов присутствия контактов.

## Поля

| Поле | Что |
|---|---|
| `contactServerId` | **серверный ID контакта** (PK) |
| `seen` | **время последнего визита** |
| `status` | **статус** |

## Что важно

1. **`seen`** — время последнего визита каждого контакта. Это «последний раз онлайн».

2. **`status`** — статус присутствия (онлайн/офлайн/и т.д.).

3. Данные приходят через `NOTIF_PRESENCE` (опкод 113) и `CONTACT_PRESENCE` (опкод 36).

4. Хранится локально для каждого контакта по `contactServerId`.

## Сводка

`presence`: contactServerId/seen(last online time)/status. Локальный кэш статусов присутствия.
