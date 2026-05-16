---
tags: [api-methods, telemetry, surveillance, vchat, auth, log-upload]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/
related:
  - "[[388-ok-api-service-internal]]"
  - "[[389-vchat-api-requests]]"
  - "[[493-log-external-api]]"
---

# tp.b() — Все API-методы (полный список)

`tp.b(method)` — построитель URI для API-запросов.

## Все методы

| Метод | Что |
|---|---|
| `auth.anonymLogin` | **анонимный вход** |
| `batch.executeV2` | **batch-запросы** |
| `log.externalLog` | **отправка логов** |
| `vchat.getAnonymTokenByLink` | **получить анонимный токен по ссылке** |
| `vchat.getLogUploadUrl` | **получить URL для загрузки логов** |

Плюс методы из `OkApiServiceInternal`:
- `vchat.startConversation`
- `vchat.joinConversation`
- `vchat.joinConversationByLink`
- `vchat.hangupConversation`
- `vchat.getConversationParams`
- `vchat.clientSupportedCodecs`
- `vchat.createJoinLink`
- `vchat.removeJoinLink`
- `settings.get`
- `batch.execute/vchat.getExternalIdsByOkIds`
- `batch.execute/vchat.getOkIdsByExternalIds`

## Что важно

1. **`vchat.getLogUploadUrl`** — сервер выдаёт URL для загрузки логов звонков.

2. **`auth.anonymLogin`** — анонимный вход для участия в звонках без аккаунта.

3. **`vchat.getAnonymTokenByLink`** — получение анонимного токена по ссылке.

## Сводка

5 прямых `tp.b()` методов + 11 из OkApiServiceInternal. Ключевые: `vchat.getLogUploadUrl`/`auth.anonymLogin`/`vchat.getAnonymTokenByLink`.
