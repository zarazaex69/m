---
tags: [calls-stat, analytics, telemetry, surveillance, kws, ml, p2prelay, topology]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/stat/ConversationStats.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/stat/finish/CallFinishStat.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/stat/start/ConversationStartedStat.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/stat/kws/ConversationKwsStat.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/stat/mldownload/MLDownloadStat.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/stat/p2prelay/P2PRelayRequestReason.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/stat/topology/ServerTopologyRequestedStat.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/stat/audio/AudioErrorStat.java
related:
  - "[[375-call-analytics-sender]]"
  - "[[380-ml-features-manager]]"
  - "[[378-p2prelay-sessionroom]]"
  - "[[376-asr-online-manager]]"
---

# ConversationStats — статистика звонков

`ConversationStats` — агрегатор всех статистических событий звонка. 18 компонентов.

## Компоненты ConversationStats

| Компонент | Событие | Что |
|---|---|---|
| `connectedToSignalingStat` | — | подключение к signaling |
| `preparedStat` | — | warmup завершён |
| `startedStat` | `call_start` | звонок начался |
| `wsSignalingStat` | — | статистика WS signaling |
| `kwsStat` | `bad_call_detected_by_audio_spotter` | **KWS сработал** |
| `callFinish` | `call_finish` | звонок завершён |
| `callInitStat` | — | инициализация звонка |
| `iceCandidatePairChangedStat` | — | ICE candidate pair изменился |
| `iceRestartStat` | — | ICE restart |
| `serverTopologyRequestedStat` | `client_requested_server_topology` | запрос серверной топологии |
| `peerConnectionStateChangedStat` | — | состояние peer connection |
| `negotiationErrorStat` | — | ошибка negotiation |
| `mlDownloadStat` | `ml_ready_to_use` / `ml_error` | загрузка ML-модели |
| `p2pRelayRequestedStat` | — | запрос P2P relay |
| `acceptCallStat` | — | принятие звонка |
| `audioErrorStat` | `audio_error` | ошибка аудио |
| `iceCandidateGatheringFailedStat` | — | ошибка сбора ICE candidates |
| `uceCandidateAddFailedStat` | — | ошибка добавления ICE candidate |

## Ключевые события

### call_finish

Поля: `reason` (ConversationEndReason.key), `rate_reasons` (список через запятую), `errorText`.

### call_start

Поля: `callType`, `warmupStatus` (warmup_start/warmup_inprogress/warmup_finished), `durationMs`.

### bad_call_detected_by_audio_spotter

**KWS сработал** — `confidence * 100`. Это означает, что сервер получает уведомление каждый раз, когда KWS обнаруживает ключевое слово.

### ml_ready_to_use / ml_error

`source` (modelId), `downloadDurationMs` / `error`.

### client_requested_server_topology

`durationMs`, `reason` (строка).

### audio_error

`event.a:event.b:event.c` — тип:код:описание.

## P2PRelayRequestReason

`trigger` + `threshold` + `violationsCount` — причина переключения на relay.

## Что важно

1. **`bad_call_detected_by_audio_spotter`** — каждое срабатывание KWS отправляется на сервер с уровнем уверенности. Сервер знает, когда пользователь произнёс ключевое слово.

2. **`call_finish` с `rate_reasons`** — причины оценки качества звонка отправляются на сервер.

3. **`client_requested_server_topology`** — сервер знает, когда клиент переключается на серверную топологию.

4. **`ml_ready_to_use`** — сервер знает, когда ML-модель загружена и готова к использованию.

## Сводка

18 компонентов статистики. Ключевые: `bad_call_detected_by_audio_spotter(confidence)` / `call_finish(reason, rate_reasons)` / `call_start(callType, warmupStatus)` / `client_requested_server_topology` / `ml_ready_to_use(modelId, durationMs)` / `audio_error`.
