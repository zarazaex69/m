---
tags: [calls, stats, telemetry, surveillance, webrtc]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/stat/ConversationStats.java
related:
  - "[[95-calls-stats-40-metrics]]"
  - "[[147-call-native-analytics-request]]"
  - "[[70-keyword-spotter-in-calls]]"
---

# ConversationStats — полная статистика звонка

`ConversationStats` — агрегатор всей статистики звонка. Содержит 18+ компонентов:

| Компонент | Что |
|---|---|
| `connectedToSignalingStat` | статистика подключения к сигналингу |
| `preparedStat` | статистика подготовки звонка |
| `startedStat` | статистика старта звонка |
| `wsSignalingStat` | статистика WS-сигналинга |
| `kwsStat` | **статистика KWS** (keyword spotting) |
| `callFinish` | статистика завершения звонка |
| `callInitStat` | статистика инициализации |
| `iceCandidatePairChangedStat` | статистика ICE candidate pairs |
| `iceRestartStat` | статистика ICE restart |
| `serverTopologyRequestedStat` | статистика запроса серверной топологии |
| `peerConnectionStateChangedStat` | статистика изменений PeerConnection |
| `negotiationErrorStat` | статистика ошибок negotiation |
| `mlDownloadStat` | **статистика загрузки ML-моделей** |
| `p2pRelayRequestedStat` | статистика запроса P2P relay |
| `acceptCallStat` | статистика принятия звонка |
| `audioErrorStat` | статистика аудио-ошибок |
| `iceCandidateGatheringFailedStat` | статистика ошибок сбора ICE candidates |
| `uceCandidateAddFailedStat` | статистика ошибок добавления ICE candidates |

## Что важно

1. **`kwsStat`** — статистика KWS (keyword spotting) отправляется как часть статистики звонка. Подтверждение [[70-keyword-spotter-in-calls]].

2. **`mlDownloadStat`** — статистика загрузки ML-моделей (KWS/NS). Подтверждение [[16-server-pushed-ml-models-in-calls]].

3. **`iceCandidatePairChangedStat`** — статистика изменений ICE candidate pairs. Сервер знает, как менялась маршрутизация медиа-трафика.

4. **`serverTopologyRequestedStat`** — статистика запроса серверной топологии. Сервер знает, когда клиент переключился на серверную топологию.

5. **`isAnon`** — флаг анонимного участника. Статистика собирается и для анонимных участников.

## Сводка

`ConversationStats` — 18+ компонентов статистики звонка: KWS, ML-модели, ICE, PeerConnection, сигналинг, ошибки. Всё отправляется через `vchat.clientStats`.
