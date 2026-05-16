---
tags: [asr, asr-online, server-asr, calls, surveillance, speech-recognition]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/asr_online/AsrOnlineManager.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/asr_online/AsrOnlineChunk.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/asr_online/internal/AsrOnlineManagerImpl.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/asr_online/internal/commands/AsrOnlineCommandsExecutorImpl.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/asr_online/internal/listeners/AsrOnlineListenerManagerImpl.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/asr_online/listener/AsrOnlineListener.java
related:
  - "[[15-on-device-asr-kws-diarization]]"
  - "[[16-server-pushed-ml-models-in-calls]]"
  - "[[101-calls-sdk-managers]]"
---

# AsrOnlineManager — серверный ASR в звонках

`AsrOnlineManager` — **серверный** (онлайн) ASR в звонках. Отдельный от on-device ASR (`libEnhancementLibShared.so`).

## Архитектура

```
AsrOnlineManagerImpl
  ├── AsrOnlineCommandsExecutorImpl  — команды (enable/disable)
  └── AsrOnlineListenerManagerImpl   — доставка результатов слушателям
```

## Интерфейс AsrOnlineManager

| Метод | Что |
|---|---|
| `enableAsrOnline(boolean)` | включить/выключить серверный ASR |
| `isAsrAvailable()` | доступен ли ASR |
| `addAsrOnlineListener(listener)` | подписаться на результаты |
| `removeAsrOnlineListener(listener)` | отписаться |

## Команды (AsrOnlineCommandsExecutorImpl)

| Метод | Что |
|---|---|
| `enableAsrOnline(isEnabled)` | отправляет команду через `SignalingProvider` |
| `onMigratedToServerCallTopology()` | при переходе на серверную топологию — автоматически включает ASR если был включён |

## Поток данных

```
Сервер → onAsrDataPackage(jx) → chunkFromPackage(store, pkg)
  → AsrOnlineChunk(participantId, text) → onAsrChunk(chunk)
```

`jx.a` — internal participant ID → `ParticipantStore.getByInternal()` → `externalId`
`jx.b` — **текст распознанной речи**

## AsrOnlineChunk

| Поле | Что |
|---|---|
| `participantId` | ID участника (чья речь распознана) |
| `text` | **распознанный текст** |

## AsrOnlineListener

| Callback | Что |
|---|---|
| `onAsrAvailableChanged(boolean)` | изменение доступности ASR |
| `onAsrChunk(AsrOnlineChunk)` | **новый чанк распознанной речи** |

## Что важно

1. **Серверный ASR** — в отличие от on-device ASR (`libEnhancementLibShared.so`), этот ASR работает на сервере. Аудио передаётся на сервер для распознавания.

2. **`onMigratedToServerCallTopology()`** — при переходе на серверную топологию ASR включается автоматически. Это означает, что при групповых звонках (серверная топология) ASR включается без явного согласия пользователя.

3. **`participantId` + `text`** — сервер знает, кто что сказал. Это полная транскрипция звонка с атрибуцией по участникам.

4. **`enableAsrOnline(boolean)`** — управляется через `SignalingProvider`, т.е. сервер может включить ASR удалённо.

## Сводка

`AsrOnlineManager`: серверный ASR. `enableAsrOnline()` → SignalingProvider. При серверной топологии включается автоматически. Результат: `AsrOnlineChunk(participantId, text)` — транскрипция с атрибуцией по участникам.
