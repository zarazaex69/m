---
tags: [webapp, init-data, ws, hash, query-id, user-data, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/qb5.java
related:
  - "[[10-webapp-jsbridge]]"
  - "[[286-ws-opcodes-additional]]"
  - "[[236-webapp-phone-hash]]"
---

# WEB_APP_INIT_DATA — инициализация WebApp через WS

`WEB_APP_INIT_DATA` (опкод 139) — данные инициализации мини-приложения, передаваемые через WS.

## Поля

| Поле | Что |
|---|---|
| `hash` | **HMAC-подпись данных** |
| `user` | **данные пользователя** (UserSettings) |
| `query_id` | **ID запроса** |
| `url` | URL мини-приложения |
| `trackId` | ID трекинга |
| `email` | email пользователя |
| `chatReactionsSettings` | настройки реакций чата |
| `organizations` | организации пользователя |

## Что важно

1. **`hash`** — HMAC-подпись. Аналог Telegram WebApp init data. Мини-приложение может верифицировать данные.

2. **`user`** — данные пользователя передаются в мини-приложение при инициализации. Это включает UserSettings.

3. **`email`** — email пользователя передаётся в мини-приложение.

4. **`organizations`** — список организаций пользователя передаётся в мини-приложение.

5. **`query_id`** — ID запроса для ответа боту через `answerWebAppQuery`.

## Сводка

`WEB_APP_INIT_DATA`: hash/user/query_id/url/trackId/email/chatReactionsSettings/organizations. Мини-приложение получает email, организации и UserSettings пользователя.
