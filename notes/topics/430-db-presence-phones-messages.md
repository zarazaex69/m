---
tags: [database, presence, profile, phones, contacts, messages, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/crc.java
related:
  - "[[06-contacts]]"
  - "[[427-db-additional-tables]]"
  - "[[429-fcm-notifications-db]]"
---

# DB — presence, profile, phones, contacts, messages схемы

## presence

```sql
CREATE TABLE `presence` (
  `contactServerId` INTEGER NOT NULL PRIMARY KEY,
  `seen` INTEGER NOT NULL,
  `status` INTEGER NOT NULL
)
```

Онлайн-статус контактов: contactServerId, seen (timestamp), status.

## profile

```sql
CREATE TABLE `profile` (
  `id` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  `server_id` INTEGER NOT NULL,
  `profile` BLOB NOT NULL
)
```

Профили пользователей в BLOB.

## phones

```sql
CREATE TABLE `phones` (
  `id` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  `phonebook_id` INTEGER NOT NULL,
  `contact_id` INTEGER NOT NULL,
  `phone` TEXT NOT NULL,
  `phone_key` TEXT NOT NULL,
  `server_phone` INTEGER NOT NULL,
  `email` TEXT,
  `first_name` TEXT NOT NULL,
  `last_name` TEXT,
  `avatar_path` TEXT,
  `type` INTEGER NOT NULL
)
```

Телефонная книга: phone, phone_key, server_phone, email, first_name, last_name, avatar_path.

## contacts

```sql
CREATE TABLE `contacts` (
  `id` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  `server_id` INTEGER NOT NULL,
  `data` BLOB NOT NULL
)
```

## messages (частичная схема)

| Поле | Что |
|---|---|
| `server_id` | ID сообщения на сервере |
| `time` | время |
| `update_time` | время обновления |
| `sender` | отправитель |
| `cid` | ID чата |
| `text` | **текст сообщения** |
| `delivery_status` | статус доставки |
| `status` | статус |
| `attaches` | вложения (BLOB) |
| `media_type` | тип медиа |
| `msg_link_type`/`msg_link_id` | ссылка на сообщение |
| `msg_link_chat_id`/`msg_link_chat_name` | чат ссылки |
| `error`/`localized_error` | ошибки |

## Что важно

1. **`phones.phone_key`** — хэш телефона для синхронизации с сервером.

2. **`phones.server_phone`** — флаг, что телефон есть на сервере.

3. **`presence.seen`** — timestamp последнего онлайна контакта.

4. **`messages.text`** — текст сообщений хранится в открытом виде в SQLite.

## Сводка

`presence(contactServerId, seen, status)`. `phones(phone, phone_key, server_phone, email, first_name, last_name)`. `messages(server_id, time, sender, cid, text, delivery_status, attaches)`.
