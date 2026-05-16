---
tags: [record, calls, surveillance, video-recording, stream]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/record/RecordManager.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/record/RecordDescription.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/record/RecordDescriptionHistory.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/record/internal/RecordManagerImpl.java
related:
  - "[[101-calls-sdk-managers]]"
  - "[[376-asr-online-manager]]"
---

# RecordManager — запись звонков

`RecordManager` — управление записью звонков. Запись сохраняется как видео (movie) на сервере.

## StartParams

| Поле | Что |
|---|---|
| `isStream` | **стриминг** (прямая трансляция) |
| `movieId` | ID видео (если уже существует) |
| `albumId` | ID альбома |
| `groupId` | ID группы |
| `privacy` | приватность (default: `"PUBLIC"`) |
| `name` | имя записи (auto: `"YYYY-MM-DD HH:MM:SS"`) |
| `description` | описание |
| `sessionRoomId` | ID комнаты сессии |

## StopParams

| Поле | Что |
|---|---|
| `removeRecord` | удалить запись после остановки |
| `sessionRoomId` | ID комнаты |

## RemoveResult

`NOT_REQUESTED` / `REMOVED` / `NOT_SUPPORTED` / `NOT_REMOVED`

## RecordDescription

| Поле | Что |
|---|---|
| `initiator` | **ParticipantId** — кто начал запись |
| `movieId` | ID видео на сервере |
| `externalMovieId` | внешний ID видео |
| `externalOwnerId` | внешний ID владельца |
| `start` | timestamp начала |
| `type` | тип записи |

## Команды (JSON)

| Команда | Что |
|---|---|
| `record-start` | `{movieId, name, description, albumId, groupId, privacy, isStream}` |
| `record-stop` | `{command: "record-stop", remove: bool}` |

## Callbacks (RecordEventListener)

| Callback | Что |
|---|---|
| `onRecordStarted()` | запись началась |
| `onRecordStopped(participant)` | запись остановлена (кем) |
| `onRecordError(description)` | ошибка |

## Что важно

1. **`privacy = "PUBLIC"` по умолчанию** — записи звонков публичны по умолчанию. Пользователь должен явно изменить.

2. **`isStream`** — запись может быть прямой трансляцией. Это означает, что звонок может транслироваться публично в реальном времени.

3. **`initiator`** — сервер знает, кто начал запись. Это атрибуция.

4. **`sessionRoomToRecordInfoHistory`** — история записей по комнатам. Сервер хранит историю.

5. **`movieId`** — запись сохраняется как видео на сервере OK.ru/VK.

## Сводка

`RecordManager.startRecord(params)` → `record-start {movieId, name, privacy="PUBLIC", isStream}`. Запись сохраняется на сервере. `privacy="PUBLIC"` по умолчанию. Поддерживается стриминг.
