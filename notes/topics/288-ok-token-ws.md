---
tags: [protocol, ws, ok-token, token-refresh, sessions, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/seh.java
related:
  - "[[29-external-callback-ok-token]]"
  - "[[286-ws-opcodes-additional]]"
---

# OK_TOKEN WS опкод — обновление токена авторизации

`OK_TOKEN` (опкод 98) — сервер обновляет OK-токен авторизации через WS.

## Поля ответа

| Поле | Что |
|---|---|
| `token_refresh_ts` | **время обновления токена** |
| `token_lifetime_ts` | **время жизни токена** |

## Что важно

1. **Сервер может обновить токен авторизации через WS** без явного запроса клиента.

2. **`token_lifetime_ts`** — время жизни токена. Сервер контролирует, как долго действует токен.

3. **`token_refresh_ts`** — время последнего обновления. Клиент сохраняет это в `okTokenRefreshTs` SharedPreferences (см. [[224-yag-shared-prefs-full]]).

## Сводка

`OK_TOKEN`: token_refresh_ts/token_lifetime_ts. Сервер обновляет OK-токен через WS.
