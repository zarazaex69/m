---
tags: [protocol, ws, contacts, presence, phone, email, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/nm4.java
related:
  - "[[06-contacts]]"
  - "[[260-protos-contact-schema]]"
  - "[[36-presence-server-controlled]]"
---

# nm4 — парсер контактов и presence данных

`nm4` — парсер данных контактов и presence, используемый в WS-ответах.

## Поля контакта

| Поле | Что |
|---|---|
| `phonebook_id` | **ID в адресной книге** |
| `contact_id` | ID контакта |
| `phone` / `phone_key` / `server_phone` | **телефоны** |
| `email` | **email** |
| `first_name` / `last_name` | имя/фамилия |
| `avatar_path` | путь к аватару |
| `type` | тип контакта |

## Поля presence

| Поле | Что |
|---|---|
| `seen` | **время последнего визита** |
| `status` | **статус** |

## Поля уведомлений (FCM)

| Поле | Что |
|---|---|
| `message_id` | ID сообщения |
| `sender_user_name` | имя отправителя |
| `large_image_url` | URL большого изображения |
| `last_modified` | время изменения |

## Что важно

1. **`phone`/`phone_key`/`server_phone`** — три варианта телефона. Сервер хранит телефон в нескольких форматах.

2. **`seen`** — время последнего визита пользователя. Это presence данные.

3. **`phonebook_id`** — ID в адресной книге устройства. Сервер знает соответствие между серверными контактами и локальной адресной книгой.

## Сводка

`nm4`: phonebook_id/contact_id/phone/phone_key/server_phone/email/first_name/last_name/avatar_path + presence(seen/status).
