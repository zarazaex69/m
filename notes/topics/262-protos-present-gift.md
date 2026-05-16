---
tags: [protocol, present, gift, protos, schema, money-transfer]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Protos.java
  - work/jadx_base/sources/ru/ok/tamtam/nano/Tasks.java
related:
  - "[[206-protos-schemas]]"
  - "[[55-organizations-money-transfer]]"
---

# Protos.Present + ConfirmPresent — подарки/переводы

## Protos.Present (вложение типа PRESENT)

Статусы: `NEW`(1), `RECEIVED`(2), `ACCEPTED`(3), `DECLINED`(4), `ACCEPTING`(5), `UNKNOWN`(0).

| Поле | Что |
|---|---|
| `presentId` | ID подарка |
| `metadataId` | ID метаданных |
| `senderId` | **ID отправителя** |
| `receiverId` | **ID получателя** |
| `status` | статус |

## ConfirmPresent Task

| Поле | Что |
|---|---|
| `presentId` / `metadataId` | ID подарка |
| `chatId` / `chatServerId` | ID чата |
| `messageId` / `messageServerId` | ID сообщения |
| `accept` | **принять/отклонить** |
| `asPrivate` | **как приватный** |
| `requestId` | ID запроса |

## Что важно

1. **`Present`** — подарки/переводы в чатах. Связано с [[55-organizations-money-transfer]].

2. **`ConfirmPresent.asPrivate`** — подарок можно принять приватно.

3. **`Present.senderId`/`receiverId`** — сервер знает отправителя и получателя каждого подарка.

## Сводка

`Present`: presentId/senderId/receiverId/status(NEW/RECEIVED/ACCEPTED/DECLINED). `ConfirmPresent`: accept/asPrivate.
