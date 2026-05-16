---
tags: [database, chat-folder, schema, filters, widgets]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/sk.java
related:
  - "[[216-local-db-tables]]"
---

# chat_folder DB схема — папки чатов

`chat_folder` — таблица папок чатов (фильтры, виджеты).

## Поля

| Поле | Что |
|---|---|
| `id` | ID папки |
| `title` | название |
| `order` | порядок |
| `emoji` | emoji папки |
| `filters` | **фильтры** (JSON/BLOB) |
| `isHiddenForAllFolder` | скрыта ли папка «Все» |
| `elements` | **элементы** (BLOB) |
| `filterSubjects` | **субъекты фильтрации** |
| `widgets` | **виджеты** (BLOB) |
| `options` | опции |
| `updateTime` | время обновления |
| `favorites` | избранные |
| `templateId` | ID шаблона |
| `sourceId` | ID источника |

## Что важно

1. **`widgets`** — папки могут содержать виджеты. Это расширенная функциональность папок.

2. **`templateId`/`sourceId`** — папки могут создаваться по шаблонам с сервера. Связано с `CHANNEL_RECSYS_FOLDER` (см. [[212-channel-recsys-telemetry]]).

3. **`filterSubjects`** — субъекты фильтрации (боты, каналы, группы и т.д.).

## Сводка

`chat_folder`: 14 полей. Ключевые: filters/filterSubjects/widgets/templateId/sourceId.
