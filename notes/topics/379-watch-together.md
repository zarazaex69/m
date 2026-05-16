---
tags: [watch-together, calls, video, movie, surveillance, wt-signaling]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/watch_together/WatchTogetherPlayer.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/watch_together/listener/WatchTogetherListener.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/watch_together/listener/states/MovieState.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/watch_together/listener/states/MovieStartedData.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/watch_together/listener/states/MovieStoppedData.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/wt/WTSignaling.java
related:
  - "[[101-calls-sdk-managers]]"
  - "[[378-p2prelay-sessionroom]]"
---

# WatchTogether — совместный просмотр видео в звонках

`WatchTogetherPlayer` — управление совместным просмотром видео во время звонка.

## Команды WatchTogetherPlayer

| Команда | Параметры |
|---|---|
| `play(movieId, volume, meta, moveToAdminOnHangup)` | воспроизведение |
| `pause(movieId)` | пауза |
| `resume(movieId)` | продолжить |
| `stop(movieId)` | остановить |
| `setPosition(movieId, position, unit)` | перемотка |
| `setMuted(movieId, isMuted)` | заглушить |
| `setVolume(movieId, volume, isMuted)` | громкость |

## MovieState

| Поле | Что |
|---|---|
| `participantId` | **кто управляет** воспроизведением |
| `position` | текущая позиция |
| `isPlaying` | воспроизводится ли |
| `volume` | громкость |
| `isMuted` | заглушено ли |
| `movie` | объект фильма |

## Callbacks (WatchTogetherListener)

| Callback | Что |
|---|---|
| `onVideoStarted(MovieStartedData)` | видео запущено (participant, roomId, movie) |
| `onVideoStatesChanged(MovieStates)` | состояние изменилось |
| `onVideoStopped(MovieStoppedData)` | видео остановлено (participant, roomId, movieId, sourceType) |

## WTSignaling

Транспорт для WatchTogether — WebTransport (`TRANSPORT_TAG = "WebTransportNetworking"`).

| Параметр | Что |
|---|---|
| `isDataCompressionEnabled` | сжатие данных (default: **true**) |
| `isWtTrafficLoggingEnabled` | логирование трафика |
| `isFallbackSupported()` | поддержка fallback |

Использует `lpk` (WebSocket/WebTransport) с SSL-верификацией через `X509Certificate`.

## Что важно

1. **`participantId` в MovieState** — сервер знает, кто управляет воспроизведением.

2. **`moveToAdminOnHangup`** — при завершении звонка управление переходит к администратору.

3. **`sourceType` в MovieStoppedData** — тип источника остановки (кто/что остановило).

4. **WebTransport** — WatchTogether использует WebTransport (не WebSocket). Это более новый протокол.

5. **`isDataCompressionEnabled = true`** — данные сжимаются по умолчанию.

## Сводка

`WatchTogetherPlayer`: play/pause/resume/stop/setPosition/setMuted/setVolume. `MovieState(participantId, position, isPlaying, volume, isMuted)`. Транспорт: WebTransport с сжатием.
