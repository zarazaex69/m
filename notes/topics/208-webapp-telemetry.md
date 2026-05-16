---
tags: [webapp, telemetry, critlog, bridge, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/x9k.java
  - work/jadx_base/sources/defpackage/j6k.java
related:
  - "[[10-webapp-jsbridge]]"
  - "[[37-webapp-privilege-configuration]]"
  - "[[207-critlog-events]]"
---

# WebApp телеметрия — WEBAPP_ACTION и WEBAPP_BRIDGE

Каждое взаимодействие с мини-приложением логируется через CritLog.

## WEBAPP_ACTION — действия с мини-приложением

| Событие | Что |
|---|---|
| `OPEN` | открытие мини-приложения |
| `CLOSE` | закрытие |
| `REFRESH` | обновление |
| `MINIAPP_TAKE_PHOTO` | **фото через мини-приложение** |
| `MINIAPP_TAKE_PHOTO_FROM_DOWNLOAD_MENU` | фото из меню загрузки |

Параметры: `botId`, `webAppName`, `entryPoint`, `sourceType`, `sourceId`.

## WEBAPP_BRIDGE — вызовы JS-bridge

Параметры каждого вызова:

| Поле | Что |
|---|---|
| `sessionId` | ID сессии |
| `botId` | ID бота/мини-приложения |
| `webAppName` | имя мини-приложения |
| `success` | успешность вызова |
| `type` | тип вызова |
| `method` | метод |
| `code` | код ответа |

## Что важно

1. **Каждый вызов JS-bridge логируется** с `botId`, `webAppName`, `success`, `method`, `code`. Сервер знает, какие методы вызывает каждое мини-приложение.

2. **`MINIAPP_TAKE_PHOTO`** — отдельное событие для фото через мини-приложение. Логируется с `botId` и `entryPoint`.

3. **`sessionId`** в WEBAPP_BRIDGE — привязка к сессии пользователя.

## Сводка

`WEBAPP_ACTION`: OPEN/CLOSE/REFRESH/MINIAPP_TAKE_PHOTO + botId/webAppName/entryPoint/sourceType. `WEBAPP_BRIDGE`: каждый JS-bridge вызов + sessionId/botId/webAppName/success/method/code.
