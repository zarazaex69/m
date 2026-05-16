---
tags: [calls, rate-hints, quality, stats, telemetry]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/rate/internal/RttRateHintTrigger.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/rate/internal/LossHintTrigger.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/rate/internal/RateManagerImpl.java
related:
  - "[[173-call-finish-init-stat]]"
  - "[[137-rating-limits-p2prelay-config]]"
---

# RateHint — автоматические причины плохого качества звонка

`RateHint` — автоматически определяемые причины плохого качества. Отправляются в `call_finish.rate_reasons`.

## Типы RateHint

| Причина | Формат | Что |
|---|---|---|
| RTT | `rtt_<rttMs>` | высокий RTT (задержка) |
| Audio loss | `audioloss_<maxLoss>` | потери аудио-пакетов |
| Video loss | `videoloss_<maxLoss>` | потери видео-пакетов |
| Candidate type | (из CandidateTypeHintTrigger) | тип ICE candidate |

## Механизм

`RateManagerImpl` содержит 4 триггера:
- `rttTrigger` — мониторит RTT
- `lossTrigger` — мониторит packet loss (audio + video)
- `directCandidateTypeTrigger` — мониторит тип прямого ICE candidate
- `serverCandidateTypeTrigger` — мониторит тип серверного ICE candidate

При превышении порогов (из `android.rating.limits`) — добавляет `RateHint` в коллекцию.

## Что важно

1. **`rtt_<rttMs>`** — конкретное значение RTT в момент проблемы. Сервер знает точную задержку.

2. **`audioloss_<maxLoss>` и `videoloss_<maxLoss>`** — конкретные значения потерь пакетов.

3. Пороги задаются сервером через `android.rating.limits` (см. [[137-rating-limits-p2prelay-config]]).

## Сводка

`RateHint` — автоматические причины плохого качества: `rtt_<ms>`, `audioloss_<loss>`, `videoloss_<loss>`, candidate type. Пороги server-controlled. Отправляются в `call_finish.rate_reasons`.
