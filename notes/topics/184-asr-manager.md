---
tags: [calls, asr, recording, surveillance, on-device]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/asr/AsrManager.java
related:
  - "[[15-on-device-asr-kws-diarization]]"
  - "[[152-asr-online-manager]]"
  - "[[154-conversation-features-roles]]"
  - "[[179-conversation-interface]]"
---

# AsrManager — on-device ASR запись в звонках

`AsrManager` — управление on-device ASR записью во время звонков.

## Методы

| Метод | Что |
|---|---|
| `startRecord(fileName, sessionRoomId)` | **начать ASR-запись** в файл |
| `stopRecord(sessionRoomId)` | остановить ASR-запись |
| `addAsrRecordListener(listener)` | слушатель событий ASR |
| `removeAsrRecordListener(listener)` | удалить слушатель |

## Что важно

1. **`startRecord(fileName, sessionRoomId)`** — ASR-запись сохраняется в файл с именем `fileName`. Это означает, что аудио звонка записывается на устройство для последующей обработки ASR.

2. **`sessionRoomId`** — запись привязана к конкретной комнате сессии. В групповых звонках с несколькими комнатами — запись для каждой комнаты отдельно.

3. **`ASR_RECORD` фича** (см. [[154-conversation-features-roles]]) — запись для ASR доступна только для определённых ролей.

4. Это отдельно от `AsrOnlineManager` (см. [[152-asr-online-manager]]) — `AsrManager` записывает в файл для on-device обработки, `AsrOnlineManager` — real-time через сигналинг.

## Сводка

`AsrManager` — on-device ASR запись в файл (`fileName`). Отдельно от online-транскрипции. `ASR_RECORD` фича с role-based access.
