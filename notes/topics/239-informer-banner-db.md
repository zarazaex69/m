---
tags: [database, informer-banner, schema, server-control, tracking]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/sk.java
related:
  - "[[216-local-db-tables]]"
  - "[[48-informer-banners]]"
---

# informer_banner DB схема

`informer_banner` — таблица информационных баннеров (см. [[48-informer-banners]]).

## Поля

| Поле | Что |
|---|---|
| `id` | ID баннера |
| `title` | заголовок |
| `description` | описание |
| `settings` | настройки (JSON/BLOB) |
| `priority` | приоритет |
| `repeat` | повторять |
| `rerun` | перезапустить |
| `animoji_id` | ID анимодзи |
| `url` | URL действия |
| `type` | тип баннера |
| `click_time` | **время клика** |
| `show_time` | **время показа** |
| `close_time` | **время закрытия** |
| `show_count` | **счётчик показов** |

## Что важно

1. **`click_time`/`show_time`/`close_time`** — полный lifecycle взаимодействия с баннером хранится локально. Это данные для аналитики.

2. **`show_count`** — счётчик показов. Сервер может ограничить количество показов.

3. **`animoji_id`** — баннер может содержать анимодзи.

## Сводка

`informer_banner`: 14 полей. Ключевые: click_time/show_time/close_time/show_count — полный lifecycle взаимодействия.
