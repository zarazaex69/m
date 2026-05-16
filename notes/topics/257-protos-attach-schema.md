---
tags: [protocol, attach, protos, schema, sensitive-content]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Protos.java
related:
  - "[[206-protos-schemas]]"
  - "[[256-protos-chat-schema]]"
---

# Protos.Attach — схема вложений

`Protos.Attach` — структура вложения к сообщению.

## Типы вложений

`PHOTO`(2), `VIDEO`(3), `AUDIO`(4), `STICKER`(5), `SHARE`(6), `APP`(7), `CALL`(8), `MUSIC`(9), `FILE`(10), `CONTACT`(11), `PRESENT`(12), `INLINE_KEYBOARD`(13), `LOCATION`(14), `DAILY_MEDIA`(15), `WIDGET`(16), `POLL`(17).

## Поля Attach

| Поле | Что |
|---|---|
| `type` | тип вложения |
| `status` | статус |
| `progress` / `progressFloat` | прогресс загрузки |
| `totalBytes` / `bytesDownloaded` | размер |
| `isDeleted` | удалено |
| `isProcessingOnServer` | обрабатывается на сервере |
| `processingOnServerStatus` | статус обработки |
| `lastModified` | время изменения |
| `lastErrorTime` | время последней ошибки |
| `sensitive` | **чувствительный контент** |
| `sensitiveContentUnlocked` | **чувствительный контент разблокирован** |

## App вложение

| Поле | Что |
|---|---|
| `appId` | ID приложения |
| `state` | состояние |
| `timeout` | таймаут |

## Что важно

1. **`sensitive`/`sensitiveContentUnlocked`** — флаги чувствительного контента. Связано с `app.privacy.content.level.access` (см. [[247-user-settings-full]]).

2. **`isProcessingOnServer`** — вложение обрабатывается на сервере. Это означает, что медиа-файлы проходят серверную обработку.

3. **17 типов вложений** — включая `DAILY_MEDIA`, `WIDGET`, `PRESENT`.

## Сводка

`Protos.Attach`: 17 типов. Ключевые поля: sensitive/sensitiveContentUnlocked/isProcessingOnServer/processingOnServerStatus.
