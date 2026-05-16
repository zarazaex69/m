---
tags: [telemetry, search, critlog, surveillance, contacts]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/vm3.java
related:
  - "[[207-critlog-events]]"
  - "[[06-contacts]]"
---

# SEARCH_RESPONSE — телеметрия поиска

При каждом поисковом запросе отправляется CritLog событие `SHOW`/`SEARCH_RESPONSE`.

## Параметры

| Поле | Что |
|---|---|
| `inputQuery` | **поисковый запрос** (если не пустой) |
| `counters.RECENTS` | количество недавних контактов |
| `counters.ALL_CONTACTS` | количество всех контактов |
| `counters.LOCAL_SEARCH` | количество локальных результатов |

## Что важно

1. **`inputQuery`** — поисковый запрос отправляется на сервер. Сервер знает, что ищет пользователь.

2. **`counters.ALL_CONTACTS`** — количество контактов в адресной книге. Это дополнение к метрике `phonebook_size` из [[14-stat-prefs-metrics]].

3. Событие отправляется при каждом показе результатов поиска.

## Сводка

`SEARCH_RESPONSE`: `inputQuery` (поисковый запрос) + `counters(RECENTS/ALL_CONTACTS/LOCAL_SEARCH)`. Сервер знает поисковые запросы пользователя.
