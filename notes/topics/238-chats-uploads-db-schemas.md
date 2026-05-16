---
tags: [database, chats, schema, uploads, thumbhash]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/hj3.java
  - work/jadx_base/sources/defpackage/t54.java
related:
  - "[[215-messages-db-schema]]"
  - "[[216-local-db-tables]]"
---

# chats + uploads DB схемы

## chats

| Поле | Что |
|---|---|
| `id` | локальный ID |
| `server_id` | серверный ID |
| `data` | **BLOB — данные чата** |
| `favourite_index` | индекс избранного |
| `sort_time` | время сортировки |
| `cid` | ID разговора |

`data` — BLOB с полными данными чата (сериализованный protobuf).

## uploads

| Поле | Что |
|---|---|
| `attach_local_id` | локальный ID вложения |
| `prepared_path` | путь к подготовленному файлу |
| `file_name` | имя файла |
| `upload_url` | URL загрузки |
| `upload_progress` | прогресс |
| `total_bytes` | размер |
| `upload_status` | статус |
| `created_time` | время создания |
| `path` | путь к файлу |
| `last_modified` | время изменения |
| `upload_type` | тип загрузки |
| `photo_token` | токен фото |
| `attach_id` | ID вложения |
| `thumbhash_base64` | **ThumbHash** (Base64) |
| `desired_uploader` | желаемый загрузчик |

## Что важно

1. **`thumbhash_base64`** — ThumbHash — компактный хэш изображения для placeholder. Хранится в БД загрузок.

2. **`chats.data`** — BLOB с полными данными чата. Это означает, что все данные чата хранятся локально в сериализованном виде.

## Сводка

`chats`: id/server_id/data(BLOB)/favourite_index/sort_time/cid. `uploads`: 15 полей включая thumbhash_base64.
