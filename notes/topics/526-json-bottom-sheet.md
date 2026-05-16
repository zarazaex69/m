---
tags: [json-editor, devmenu, debug, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/devmenu/utils/JsonBottomSheet.java
related:
  - "[[05-dev-menu-in-prod]]"
  - "[[433-dev-menu]]"
  - "[[525-integrity-logs-viewer]]"
---

# JsonBottomSheet — редактор JSON в DevMenu

`JsonBottomSheet` — bottom sheet для редактирования JSON в DevMenu.

## Параметры

| Параметр | Тип | Описание |
|---|---|---|
| `arg:button_id` | `Long` | ID кнопки |
| `arg:initial_json` | `String` | начальный JSON |
| `arg:descriptions` | `String[]` | описания полей |

## Интерфейс

- Заголовок: `"Редактирование JSON"`
- Кнопка: `"+ Добавить свойство"`
- Кнопка: `"Установить"`
- Поля редактируются через `tz8` (key-value пары)

## Что важно

1. **`JsonBottomSheet`** — редактор JSON в DevMenu (production). Позволяет редактировать произвольный JSON.

2. **`arg:initial_json`** — начальный JSON передаётся как параметр.

3. **`arg:button_id`** — ID кнопки для идентификации действия.

## Сводка

`JsonBottomSheet` — редактор JSON в DevMenu (production). Параметры: `button_id`/`initial_json`/`descriptions`.
