---
tags: [calls, recording, server-control, stream, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/record/RecordManager.java
related:
  - "[[154-conversation-features-roles]]"
  - "[[25-resources-audit]]"
  - "[[179-conversation-interface]]"
---

# RecordManager — запись и стриминг звонков

`RecordManager` — управление записью звонков.

## Методы

| Метод | Что |
|---|---|
| `startRecord(StartParams)` | начать запись |
| `stopRecord(StopParams)` | остановить запись |
| `getRecordDescription()` | текущее описание записи |
| `getRecordDescriptionHistory()` | история записей |
| `addRecordListener(listener)` | слушатель событий записи |

## StartParams

| Поле | Что |
|---|---|
| `isStream` | **стриминг** (не просто запись) |
| `movieId` | ID видео |
| `albumId` | ID альбома |
| `sessionRoomId` | ID комнаты сессии |
| `name` | название |
| `description` | описание |
| `groupId` | ID группы |
| `privacy` | приватность |

## Что важно

1. **`isStream=true`** — запись может быть стримингом. Это означает, что звонок может транслироваться в реальном времени.

2. **`movieId`** — запись привязывается к конкретному видео-объекту. Это интеграция с видео-платформой VK/OK.

3. **`privacy`** — приватность записи. Сервер контролирует, кто может видеть запись.

4. **`RECORD` фича** (см. [[154-conversation-features-roles]]) — запись доступна только для определённых ролей.

5. **`record-stop` сигналинг** (см. [[153-signaling-commands]]) — остановка записи через сигналинг.

## Сводка

`RecordManager` — запись и стриминг звонков. `StartParams` с `isStream`/`movieId`/`albumId`/`privacy`. Интеграция с видео-платформой VK/OK.
