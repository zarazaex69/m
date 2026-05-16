---
tags: [database, comments, complain-reasons, schema, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/w24.java
  - work/jadx_base/sources/defpackage/t54.java
related:
  - "[[215-messages-db-schema]]"
  - "[[216-local-db-tables]]"
---

# comments + complain_reasons DB схемы

## comments — комментарии к постам

| Поле | Что |
|---|---|
| `id` / `server_id` | ID |
| `time` / `update_time` / `time_local` | времена |
| `sender` | **ID отправителя** |
| `cid` | ID разговора |
| `text` | **текст** |
| `delivery_status` / `status` / `status_in_process` | статусы |
| `error` / `localized_error` | ошибки |
| `attaches` | вложения |
| `media_type` | тип медиа |
| `detect_share` | обнаружение шаринга |
| `msg_link_type` / `msg_link_id` | ссылка на сообщение |
| `inserted_from_msg_link` | вставлено по ссылке |
| `msg_link_out_chat_id` / `msg_link_out_post_id` / `msg_link_out_msg_id` | исходящая ссылка |
| `options` | опции |
| `elements` | элементы |
| `reactions` / `reactions_update_time` | реакции |
| `parent_chat_server_id` / `parent_message_server_id` | **родительский пост** |

## complain_reasons

| Поле | Что |
|---|---|
| `id` | ID |
| `type_id` | **тип жалобы** |
| `complain_reasons` | **причины жалоб** (BLOB) |

## Что важно

1. **`comments.parent_chat_server_id`/`parent_message_server_id`** — комментарий привязан к конкретному посту в конкретном чате.

2. **`comments`** — структура аналогична `messages`, но с дополнительными полями для комментариев.

3. **`complain_reasons.type_id`** — типы жалоб управляются сервером через `COMPLAIN_REASONS_GET`.

## Сводка

`comments`: 28 полей (аналог messages + parent_chat_server_id/parent_message_server_id). `complain_reasons`: id/type_id/complain_reasons(BLOB).
