---
tags: [api, endpoints, oneme, test-servers, dev-menu, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/r58.java
  - work/jadx_base/sources/defpackage/f58.java
  - work/jadx_base/sources/defpackage/vsl.java
related:
  - "[[05-dev-menu-in-prod]]"
  - "[[04-telemetry-endpoints]]"
---

# api.oneme.ru — основной API и тестовые серверы

## Основной API

`api.oneme.ru:443` (TLS) — основной API сервер MAX.

## Тестовые серверы (r58.java)

```java
List.of("api.oneme.ru", "api-test.oneme.ru", "api-tg.oneme.ru", "api-test2.oneme.ru")
```

| Сервер | Что |
|---|---|
| `api.oneme.ru` | **production** |
| `api-test.oneme.ru` | тестовый |
| `api-tg.oneme.ru` | TG-тестовый |
| `api-test2.oneme.ru` | тестовый 2 |

## Что важно

1. **4 сервера** — production + 3 тестовых. Это подтверждение [[05-dev-menu-in-prod]] — dev-меню позволяет переключаться между серверами.

2. **`api-tg.oneme.ru`** — отдельный тестовый сервер с суффиксом `tg`. Возможно, для тестирования Telegram-совместимости.

3. **`vsl.java`** — `new ue4("api.oneme.ru", "443", true)` — явное указание порта 443 и TLS.

## Сводка

API серверы: `api.oneme.ru`(prod) + `api-test.oneme.ru` + `api-tg.oneme.ru` + `api-test2.oneme.ru`.
