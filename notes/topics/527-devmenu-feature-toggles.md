---
tags: [devmenu, feature-toggles, server-control, debug]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/devmenu/DevMenuFeatureTogglesPageScreen.java
  - work/jadx_base/sources/one/me/devmenu/DevMenuScreen.java
  - work/jadx_base/sources/one/me/devmenu/DevMenuGeneralPageScreen.java
related:
  - "[[05-dev-menu-in-prod]]"
  - "[[433-dev-menu]]"
  - "[[526-json-bottom-sheet]]"
---

# DevMenu — полная структура

`DevMenuScreen` — главный экран DevMenu с вкладками.

## Вкладки DevMenu

1. **General** (`DevMenuGeneralPageScreen`) — общие настройки
2. **Feature Toggles** (`DevMenuFeatureTogglesPageScreen`) — переключатели фич

## DevMenuFeatureTogglesPageScreen

Экран переключателей фич. Поиск по фичам (`"Поиск"`).

### Редакторы значений

| Тип | Bottom Sheet |
|---|---|
| `Boolean` | прямое переключение |
| `Long` | `LongValueBottomSheet` |
| `Integer` | `IntValueBottomSheet` |
| `String` (JSON) | `JsonBottomSheet` |
| `String` | `StringValueBottomSheet` |
| PmsKey | `StringValueBottomSheet` |

### Источник значений

`FeatureValueInfoBottomSheet` — показывает информацию о значении фичи:
- `arg:title` — название
- Источник: `"SERVER"` — значение с сервера

## Что важно

1. **Все типы значений редактируемы** — Boolean/Long/Integer/String/JSON/PmsKey.

2. **Источник `"SERVER"`** — значения фич могут приходить с сервера.

3. **`JsonBottomSheet`** — JSON-значения редактируются через специальный редактор.

4. **`PmsKey`** — PmsKey-значения редактируются через `StringValueBottomSheet`.

5. **DevMenu в production** — все эти экраны доступны в release-сборке.

## Сводка

`DevMenuFeatureTogglesPageScreen`: редакторы Boolean/Long/Integer/String/JSON/PmsKey. Источник `"SERVER"`. DevMenu в production.
