---
tags: [calls-rate, quality, rtt, packet-loss, candidate-type, surveillance, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/rate/RateManager.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/rate/RateCallData.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/rate/RateHint.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/rate/RateManagerConfig.java
related:
  - "[[382-conversation-stats]]"
  - "[[383-externcalls-sdk-net]]"
  - "[[384-externcalls-sdk-config]]"
---

# RateManager — оценка качества звонка

`RateManager` — система оценки качества звонка. Автоматически собирает подсказки о причинах плохого качества.

## RateHint — причины плохого качества

`RateHint(reason: String)` — причина плохого качества. Список причин отправляется в `call_finish(rate_reasons)`.

## RateCallData — данные для опроса

| Поле | Что |
|---|---|
| `maxRateForQuestion` | максимальная оценка |
| `questions` | список вопросов (id, text) |

Приходит с сервера в `ConversationParams.rateCallData`.

## RateManagerConfig — конфиг триггеров

| Компонент | Что |
|---|---|
| `rttRateHintConfig` | порог RTT для подсказки |
| `lossHintConfig` | порог потерь пакетов |
| `directCandidateTypeHintConfig` | тип ICE candidate (direct) |
| `serverCandidateTypeHintConfig` | тип ICE candidate (server/relay) |

## Триггеры

- `RttRateHintTrigger` — RTT превысил порог
- `LossHintTrigger` — потери пакетов превысили порог
- `CandidateTypeHintTrigger` — тип ICE candidate (relay вместо direct)

## Что важно

1. **Автоматические подсказки** — система автоматически определяет причины плохого качества и включает их в `call_finish`.

2. **`questions` с сервера** — сервер задаёт вопросы для опроса после звонка.

3. **`maxRateForQuestion`** — максимальная оценка задаётся сервером.

4. **Конфиг триггеров** — пороги RTT/loss/candidateType задаются сервером через `android.rating.limits`.

## Сводка

`RateManager`: автоматические `RateHint(reason)` по RTT/loss/candidateType. `RateCallData(maxRate, questions)` с сервера. Конфиг: `android.rating.limits`.
