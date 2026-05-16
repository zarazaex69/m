---
tags: [calls, media-connection, stats, first-connection]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/connection/MediaConnectionManager.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/connection/MediaConnectionListener.java
related:
  - "[[179-conversation-interface]]"
  - "[[171-ice-restart-peer-connection-stat]]"
---

# MediaConnectionManager — события медиа-соединения

`MediaConnectionManager` — слушатель событий медиа-соединения.

## MediaConnectionListener

| Callback | Что |
|---|---|
| `onMediaConnected(ConnectedInfo)` | медиа-соединение установлено |
| `onMediaDisconnected(DisconnectedInfo)` | медиа-соединение разорвано |

## ConnectedInfo

- `isFirstConnection` — **первое ли это подключение** в данном звонке

## Что важно

1. **`isFirstConnection`** — сервер знает, было ли это первое медиа-подключение или переподключение. Это важно для диагностики: если `isFirstConnection=false` — произошёл ICE restart или переподключение.

2. `onMediaConnected`/`onMediaDisconnected` — события жизненного цикла медиа-соединения. Используются для обновления UI и статистики.

## Сводка

`MediaConnectionManager` — `onMediaConnected(isFirstConnection)` / `onMediaDisconnected`. `isFirstConnection` — первое ли подключение.
