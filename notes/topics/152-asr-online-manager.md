---
tags: [calls, asr, online-transcription, surveillance, webrtc]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/asr_online/AsrOnlineManager.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/asr_online/AsrOnlineChunk.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/ConversationImpl.java
related:
  - "[[15-on-device-asr-kws-diarization]]"
  - "[[70-keyword-spotter-in-calls]]"
  - "[[84-transcribe-media-ws-opcode]]"
---

# AsrOnlineManager — онлайн-транскрипция во время звонков

`AsrOnlineManager` — интерфейс для онлайн-транскрипции речи во время звонков.

## Интерфейс

| Метод | Что |
|---|---|
| `isAsrAvailable()` | доступна ли ASR |
| `enableAsrOnline(boolean)` | включить/выключить онлайн-ASR |
| `addAsrOnlineListener(listener)` | добавить слушатель транскрипции |
| `removeAsrOnlineListener(listener)` | удалить слушатель |

## AsrOnlineChunk

`AsrOnlineChunk` — чанк транскрипции:
- `participantId` — ID участника, чья речь транскрибируется
- `text` — **текст транскрипции**

`chunkFromPackage(store, asrRecvDataPackage)` — создаёт чанк из пакета данных ASR.

## AsrOnlineCommandsExecutorImpl — детали

`enableAsrOnline(boolean)` — при включении:
1. Если звонок в server topology — отправляет сигналинг-сообщение `"request-asr"` через `signalingProvider.getSignaling().i(dp7VarB)`.
2. Иначе — устанавливает флаг в call объекте.

`onMigratedToServerCallTopology()` — при миграции на серверную топологию — применяет текущий ASR-статус.

**`"request-asr"`** — это сигналинг-сообщение, которое клиент отправляет серверу для запроса онлайн-транскрипции. Это означает, что онлайн-ASR работает через серверную инфраструктуру, а не только on-device.

## Что важно

1. **Онлайн-транскрипция во время звонков** — это отдельный механизм от `TRANSCRIBE_MEDIA(202)` (см. [[84-transcribe-media-ws-opcode]]). `AsrOnlineManager` — real-time транскрипция речи участников звонка.

2. **`AsrOnlineChunk.text`** — текст транскрипции с `participantId`. Это означает, что система знает, кто именно что сказал.

3. **`enableAsrOnline(boolean)`** — можно включить/выключить онлайн-ASR. Это server-controlled через `pa1.X` флаг.

4. В сочетании с [[15-on-device-asr-kws-diarization]] (on-device ASR) и [[70-keyword-spotter-in-calls]] (KWS) — три уровня аудио-анализа во время звонков: KWS (ключевые слова), on-device ASR, и онлайн-ASR.

## Сводка

`AsrOnlineManager` — real-time транскрипция речи участников звонков. `AsrOnlineChunk` содержит `participantId` и `text`. Три уровня аудио-анализа в звонках: KWS + on-device ASR + online ASR.
