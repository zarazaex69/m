---
tags: [database, messages, schema, ttl, live-location, delayed-attrs]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/oeb.java
  - work/jadx_base/sources/defpackage/ieb.java
related:
  - "[[203-live-location]]"
  - "[[206-protos-schemas]]"
---

# Схема БД сообщений — таблица messages

Локальная SQLite БД. Таблица `messages` (и `temp_messages`).

## Поля таблицы

| Поле | Тип | Что |
|---|---|---|
| `id` | INTEGER PK | локальный ID |
| `server_id` | INTEGER | серверный ID |
| `time` | INTEGER | время |
| `update_time` | INTEGER | время обновления |
| `sender` | INTEGER | ID отправителя |
| `cid` | INTEGER | ID разговора |
| `text` | TEXT | текст сообщения |
| `delivery_status` | INTEGER | статус доставки |
| `status` | INTEGER | статус |
| `time_local` | INTEGER | локальное время |
| `error` / `localized_error` | TEXT | ошибки |
| `attaches` | BLOB | вложения |
| `media_type` | INTEGER | тип медиа |
| `detect_share` | INTEGER | обнаружение шаринга |
| `msg_link_type`/`msg_link_id` | INTEGER | тип/ID ссылки на сообщение |
| `inserted_from_msg_link` | INTEGER | вставлено по ссылке |
| `msg_link_chat_id`/`msg_link_chat_name`/`msg_link_chat_link` | — | данные чата ссылки |
| `msg_link_out_chat_id`/`msg_link_out_msg_id` | INTEGER | исходящая ссылка |
| `type` | INTEGER | тип сообщения |
| `chat_id` | INTEGER FK→chats | ID чата |
| `ttl` | INTEGER | **время жизни сообщения** |
| `channel_views` | INTEGER | просмотры в канале |
| `channel_forwards` | INTEGER | пересылки в канале |
| `view_time` | INTEGER | время просмотра |
| `zoom` | INTEGER | масштаб (для геолокации) |
| `options` | INTEGER | опции |
| `live_until` | INTEGER | **живая геолокация до** |
| `elements` | BLOB | элементы сообщения |
| `reactions` | BLOB | реакции |
| `delayed_attrs_time_to_fire` | INTEGER | **время отложенной отправки** |
| `delayed_attrs_notify_sender` | INTEGER | уведомить отправителя |

## Что важно

1. **`ttl`** — время жизни сообщения. Сообщения с TTL автоматически удаляются.

2. **`live_until`** — живая геолокация активна до этого времени.

3. **`delayed_attrs_time_to_fire`** — время отложенной отправки. Это подтверждение функции отложенных сообщений.

4. **`channel_views`/`channel_forwards`** — счётчики просмотров и пересылок хранятся локально.

5. **`msg_link_*`** — полная цепочка ссылок на сообщения (forward chain).

## Сводка

Таблица `messages`: 35+ полей. Ключевые: `ttl` (самоудаление), `live_until` (живая геолокация), `delayed_attrs_time_to_fire` (отложенная отправка), `channel_views`/`channel_forwards`.
