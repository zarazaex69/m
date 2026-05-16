---
tags: [protocol, chat-settings, reactions, protos, schema, live-location]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Protos.java
related:
  - "[[256-protos-chat-schema]]"
  - "[[203-live-location]]"
---

# Protos.ChatSettings + ChatReactionsSettings

## ChatSettings (персональные настройки чата)

| Поле | Что |
|---|---|
| `dontDisturbUntil` | не беспокоить до |
| `favoriteIndex` | индекс в избранном |
| `hideLiveLocationPanel` | **скрыть панель live-геолокации** |
| `hideLiveLocationPanelBeforeTime` | скрыть панель до времени |
| `hideMyLiveLocationPanelBeforeTime` | скрыть мою live-геолокацию до |
| `lastNotifMark` | последняя отметка уведомления |
| `lastNotifMessageId` | ID последнего уведомлённого сообщения |
| `options[]` | опции |

## ChatReactionsSettings

| Поле | Что |
|---|---|
| `count` | количество реакций |
| `included` | включены ли реакции |
| `isActive` | активны ли |
| `isFull` | полный набор |
| `updateTime` | время обновления |

## Что важно

1. **`hideLiveLocationPanel`/`hideLiveLocationPanelBeforeTime`** — пользователь может скрыть панель live-геолокации. Это означает, что live-геолокация показывается в чате.

2. **`hideMyLiveLocationPanelBeforeTime`** — скрыть свою live-геолокацию до определённого времени.

3. **`ChatReactionsSettings.isFull`** — полный набор реакций (все emoji) или ограниченный.

## Сводка

`ChatSettings`: dontDisturbUntil/favoriteIndex/hideLiveLocationPanel/hideLiveLocationPanelBeforeTime/lastNotifMark. `ChatReactionsSettings`: count/included/isActive/isFull.
