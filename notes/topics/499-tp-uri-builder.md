---
tags: [api-uri, ok-api, surveillance, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/tp.java
related:
  - "[[496-api-methods-full]]"
  - "[[356-api-oneme-ru-servers]]"
---

# tp — URI Builder для API-запросов

`tp` — построитель URI для API-запросов.

## Методы

| Метод | Что |
|---|---|
| `tp.a()` | `https://api.ok.ru` — базовый URL |
| `tp.b(method)` | `ok://api/api/{method}` — URI для метода |
| `tp.c(uri)` | обратное преобразование URI → имя метода |

## Формат URI

```
ok://api/api/vchat.startConversation
→ ok://api/api/vchat/startConversation
```

Точки заменяются на слэши.

## Что важно

1. **`https://api.ok.ru`** — базовый URL API. Это OK.ru API, не MAX-специфичный.

2. **`ok://api/api/`** — внутренняя схема URI для маршрутизации запросов.

3. Все API-запросы используют `ok://api/api/{method}` схему.

## Сводка

`tp.b(method)` → `ok://api/api/{method}`. Базовый URL: `https://api.ok.ru`.
