---
tags: [protocol, tasks, external-video, congrats, holiday, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Tasks.java
related:
  - "[[205-tasks-proto]]"
  - "[[283-tasks-location-reaction-photo]]"
---

# ExternalVideoSend + CongratsStatus Tasks

## ExternalVideoSend — отправка внешнего видео

| Поле | Что |
|---|---|
| `chatId` | ID чата |
| `messageId` | ID сообщения |
| `externalUrl` | **внешний URL видео** |
| `stickerId` | ID стикера |
| `attachLocalId` | локальный ID вложения |
| `requestId` | ID запроса |

## CongratsStatus — поздравительный статус

| Поле | Что |
|---|---|
| `userId` | **ID пользователя** |
| `holidayId` | **ID праздника** |
| `status` | статус |
| `requestId` | ID запроса |

## Что важно

1. **`ExternalVideoSend.externalUrl`** — URL внешнего видео (YouTube, VK Video и т.д.). Сервер знает, какие внешние видео отправляет пользователь.

2. **`CongratsStatus.holidayId`** — ID праздника. Это система поздравлений с праздниками (день рождения, Новый год и т.д.).

3. **`CongratsStatus.userId`** — кому отправляется поздравление.

## Сводка

`ExternalVideoSend`: chatId/messageId/externalUrl/stickerId. `CongratsStatus`: userId/holidayId/status.
