---
tags: [api-servers, dev-tools, server-switch, surveillance, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/r58.java
related:
  - "[[05-dev-menu-in-prod]]"
  - "[[356-api-oneme-ru-servers]]"
  - "[[433-devmenu-details]]"
---

# r58 — Server Host Selector (выбор API-сервера)

`r58` — ViewModel для `ServerHostBottomSheet`. Управляет выбором API-сервера.

## Список серверов

```java
List.of(
  "api.oneme.ru",       // production
  "api-test.oneme.ru",  // test
  "api-tg.oneme.ru",    // test (tg)
  "api-test2.oneme.ru"  // test 2
)
```

Плюс кастомный сервер из SharedPreferences `"dev_tools"` → ключ `"Custom"`.

## Что важно

1. **4 сервера** — production + 3 тестовых.

2. **Кастомный сервер** — можно ввести произвольный адрес.

3. **`dev_tools` SharedPreferences** — настройки dev-меню хранятся в `dev_tools`.

4. Текущий сервер подсвечивается в списке.

## Сводка

`r58`: серверы api.oneme.ru/api-test.oneme.ru/api-tg.oneme.ru/api-test2.oneme.ru + Custom. SharedPreferences `dev_tools`.
