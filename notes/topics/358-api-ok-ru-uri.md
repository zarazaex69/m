---
tags: [api, ok-ru, calls-sdk, uri-scheme, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/tp.java
related:
  - "[[200-vchat-api-full-list]]"
  - "[[356-api-oneme-ru-servers]]"
---

# tp.java — api.ok.ru URI builder

`tp.java` — утилита для построения URI к `api.ok.ru`.

## Методы

| Метод | Что |
|---|---|
| `a()` | `https://api.ok.ru` |
| `b(str)` | `ok://api/api/<str>` (точки → слэши) |
| `c(uri)` | обратное преобразование `ok://api/api/` → строка |

## Схема URI

```
ok://api/api/<method.name>
```

Например: `ok://api/api/vchat.startConversation` → `ok://api/api/vchat/startConversation`

## Что важно

1. **`https://api.ok.ru`** — основной API OK.ru. Calls SDK использует этот endpoint.

2. **`ok://api/api/`** — внутренняя URI-схема для API вызовов. Точки в имени метода заменяются на слэши.

3. Это объясняет, почему calls SDK использует `api.ok.ru` — это тот же OK.ru API, но для звонков.

## Сводка

`tp.java`: `https://api.ok.ru` + `ok://api/api/<method>` URI scheme. Calls SDK использует OK.ru API.
