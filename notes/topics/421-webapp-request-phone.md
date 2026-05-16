---
tags: [webapp, phone-request, msisdn, jsbridge, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/qgk.java
  - work/jadx_base/sources/defpackage/ygk.java
related:
  - "[[419-webapp-js-events]]"
  - "[[10-webapp-jsbridge]]"
  - "[[01-mobile-id-cleartext]]"
---

# WebAppRequestPhone — запрос номера телефона мини-приложением

`WebAppRequestPhone` — JS-событие для получения номера телефона пользователя мини-приложением.

## Ответ (WebAppRequestPhoneResponse)

| Поле | Что |
|---|---|
| `requestId` | ID запроса |
| `phone` | **номер телефона** |
| `hash` | хэш (для верификации) |
| `authDate` | дата аутентификации |

## Что важно

1. **`phone`** — мини-приложение получает номер телефона пользователя.

2. **`hash`** + **`authDate`** — Telegram-подобная схема верификации. Мини-приложение может верифицировать данные на своём сервере.

3. Это дополняет `WebAppVerifyMobileId` (MSISDN через Header Enrichment) — два механизма получения номера телефона.

## Сводка

`WebAppRequestPhone` → `WebAppRequestPhoneResponse(requestId, phone, hash, authDate)`. Мини-приложение получает номер телефона с хэшем для верификации.
