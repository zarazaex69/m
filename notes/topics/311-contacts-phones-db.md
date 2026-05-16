---
tags: [database, contacts, phones, schema, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/sk.java
related:
  - "[[06-contacts]]"
  - "[[308-nm4-contact-presence-parser]]"
  - "[[216-local-db-tables]]"
---

# contacts + phones DB схемы

## contacts

| Поле | Что |
|---|---|
| `id` | локальный ID |
| `server_id` | серверный ID |
| `data` | **BLOB — данные контакта** (protobuf) |

## phones

| Поле | Что |
|---|---|
| `id` | локальный ID |
| `phonebook_id` | **ID в адресной книге** |
| `contact_id` | ID контакта |
| `phone` | **номер телефона** |
| `phone_key` | **ключ телефона** |
| `server_phone` | **серверный телефон** |
| `email` | **email** |
| `first_name` / `last_name` | имя/фамилия |
| `avatar_path` | путь к аватару |
| `type` | тип |

## Что важно

1. **`phones.phone`/`phone_key`/`server_phone`** — три варианта телефона. Это нормализованный, ключевой и серверный форматы.

2. **`phones.phonebook_id`** — ID в адресной книге устройства. Связывает серверный контакт с локальной адресной книгой.

3. **`phones.email`** — email хранится в таблице телефонов. Это данные из адресной книги.

4. **`contacts.data`** — BLOB с protobuf-данными контакта (см. [[260-protos-contact-schema]]).

## Сводка

`contacts`: id/server_id/data(BLOB protobuf). `phones`: phonebook_id/contact_id/phone/phone_key/server_phone/email/first_name/last_name/avatar_path.
