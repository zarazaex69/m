---
tags: [calls, network-stat, stats, rtt, loss, candidate-type]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/net/internal/monitor/NetworkStat.java
related:
  - "[[176-rate-hints]]"
  - "[[137-rating-limits-p2prelay-config]]"
  - "[[95-calls-stats-40-metrics]]"
---

# NetworkStat — структура сетевой статистики звонка

`NetworkStat` — структура данных для мониторинга сети во время звонка.

## Поля

| Поле | Тип | Что |
|---|---|---|
| `rttMs` | Integer | RTT в миллисекундах |
| `audioLoss` | Float | потери аудио-пакетов |
| `videoLoss` | Float | потери видео-пакетов |
| `activeCandidateType` | String | **тип активного ICE candidate** |

## Что важно

1. **`activeCandidateType`** — тип активного ICE candidate: `host` (прямое P2P), `srflx` (STUN), `relay` (TURN). Это показывает, как маршрутизируется медиа-трафик.

2. `NetworkStat` используется в `RateManagerImpl` для мониторинга и генерации `RateHint` (см. [[176-rate-hints]]).

3. `NetworkStat` также используется в `report-network-stat` сигналинг-команде (см. [[153-signaling-commands]]) — но там только `timestamp` и `bitrate`.

## Сводка

`NetworkStat` — `rttMs`/`audioLoss`/`videoLoss`/`activeCandidateType`. Используется для автоматической генерации `RateHint` при превышении порогов.
