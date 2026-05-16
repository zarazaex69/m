---
tags: [webapp, phone-hash, jsbridge, phone-number, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/uik.java
  - work/jadx_base/sources/defpackage/zgk.java
  - work/jadx_base/sources/defpackage/ygk.java
related:
  - "[[10-webapp-jsbridge]]"
  - "[[235-qp6-additional-pmskeys]]"
---

# WebApp Phone Hash — передача номера телефона в мини-приложения

`useWebAppPhoneHash` (PmsKey) — серверный флаг, управляющий передачей хэша телефона в мини-приложения.

## Структуры

**`WebAppRequestPhoneSuccess`**: `phone`, `hash`, `authDate`

**`WebAppRequestPhoneResponse`**: `requestId`, `phone`, `hash`, `authDate`

**`Response`**: `phone`, `hash`, `authDate`

## Механизм

1. Мини-приложение запрашивает номер телефона через JS-bridge.
2. Сервер возвращает `phone` + `hash` + `authDate`.
3. `hash` — HMAC-подпись номера телефона (аналог Telegram Login Widget).
4. Если `phone` или `hash` null — ошибка `"Request phone error: phone and hash was null"`.

## Что важно

1. **Мини-приложения могут получить номер телефона пользователя** через JS-bridge. Это подтверждение [[10-webapp-jsbridge]] (`verify_mobile_id`).

2. **`hash`** — подпись для верификации. Мини-приложение может проверить подлинность номера.

3. **`useWebAppPhoneHash`** — серверный флаг. Сервер контролирует, передаётся ли хэш.

## Сводка

`WebAppRequestPhone`: мини-приложение получает `phone`+`hash`+`authDate`. Серверный флаг `useWebAppPhoneHash` управляет передачей.
