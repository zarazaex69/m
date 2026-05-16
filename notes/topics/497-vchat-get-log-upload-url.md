---
tags: [log-upload, vchat, calls, surveillance, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/uq7.java
related:
  - "[[401-media-dump-manager]]"
  - "[[402-debug-manager]]"
  - "[[496-api-methods-full]]"
---

# vchat.getLogUploadUrl — получение URL для загрузки логов звонков

`uq7` — запрос `vchat.getLogUploadUrl` — получение URL для загрузки логов звонков.

## Параметры запроса

| Параметр | Что |
|---|---|
| `conversationId` | **ID звонка** |
| `webrtcPlatform` | SDK тип (`SDK_TYPE_STRING`) |
| `type` | **тип лога** (kej enum) |
| `anonymToken` | анонимный токен |

## Что важно

1. **`type`** — тип лога. Это означает, что существуют разные типы логов звонков.

2. **`conversationId`** — логи привязаны к конкретному звонку.

3. **`anonymToken`** — поддерживается анонимная загрузка логов.

4. Сервер выдаёт URL для загрузки → клиент загружает логи на этот URL.

## Сводка

`vchat.getLogUploadUrl {conversationId, webrtcPlatform, type, anonymToken}` → URL для загрузки логов звонка.
