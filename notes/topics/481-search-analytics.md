---
tags: [search, telemetry, surveillance, contacts, search-analytics]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/vm3.java
related:
  - "[[472-log-controller-event-types]]"
  - "[[06-contacts]]"
---

# SHOW.SEARCH_RESPONSE — аналитика поиска

Событие `SHOW.SEARCH_RESPONSE` отправляется при показе результатов поиска.

## Структура события

| Поле | Что |
|---|---|
| `counters.RECENTS` | **количество недавних** результатов |
| `counters.ALL_CONTACTS` | **количество контактов** в результатах |
| `counters.LOCAL_SEARCH` | **количество локальных** результатов |
| `inputQuery` | **поисковый запрос** |

## Что важно

1. **`inputQuery`** — поисковый запрос пользователя отправляется на сервер.

2. **`ALL_CONTACTS`** — количество контактов в результатах поиска.

3. **`RECENTS`** — количество недавних результатов.

4. Событие отправляется при каждом показе результатов поиска.

## Сводка

`SHOW.SEARCH_RESPONSE {counters: {RECENTS, ALL_CONTACTS, LOCAL_SEARCH}, inputQuery}`. Поисковый запрос отправляется на сервер.
