---
tags: [webapp, phone-share, ws, hash, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/jlb.java
related:
  - "[[236-webapp-phone-hash]]"
  - "[[287-web-app-init-data]]"
---

# PHONE_WEBAPP_SHARE — шаринг телефона в WebApp через WS

`PHONE_WEBAPP_SHARE` (опкод 97) — ответ сервера на запрос телефона от мини-приложения.

## Поля ответа

| Поле | Что |
|---|---|
| `phone` | **номер телефона** |
| `hash` | **HMAC-подпись** |
| `contact` | данные контакта |

## Что важно

1. **`phone`** — номер телефона пользователя передаётся мини-приложению через WS.

2. **`hash`** — HMAC-подпись для верификации. Аналог [[236-webapp-phone-hash]].

3. Это WS-аналог JS-bridge `WebAppRequestPhone` — телефон может быть получен как через JS-bridge, так и через WS.

## Сводка

`PHONE_WEBAPP_SHARE`: phone/hash/contact. Номер телефона передаётся мини-приложению через WS.
