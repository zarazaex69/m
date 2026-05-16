---
tags: [calls, quality-rating, server-control, rtt, loss, p2p]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/rate/RateManagerConfig.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/rate/internal/RateManagerConfigProviderImpl.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/p2prelay/P2PRelaySwitchConfigProviderImpl.java
related:
  - "[[69-remote-settings-calls-sdk]]"
  - "[[88-call-rate-quality-rating]]"
  - "[[125-p2p-relay-webtransport]]"
---

# android.rating.limits и android.p2prelay.config — детали

## android.rating.limits — RateManagerConfig

`RateManagerConfig` содержит 4 компонента:
- `rttRateHintConfig` — пороги RTT для оценки качества
- `lossHintConfig` — пороги packet loss
- `directCandidateTypeHintConfig` — конфиг для прямого ICE candidate
- `serverCandidateTypeHintConfig` — конфиг для серверного ICE candidate

Это детальный конфиг алгоритма оценки качества звонка. Сервер задаёт пороги RTT и packet loss, при которых звонок считается «плохим».

## android.p2prelay.config — P2PRelaySwitchConfig

`P2PRelaySwitchConfigProviderImpl` — провайдер конфига P2P relay switch. Управляет, когда переключаться с прямого P2P на relay.

## Что важно

1. **`rttRateHintConfig`** — пороги RTT. Сервер задаёт, при каком RTT показывать индикатор плохой сети (см. [[104-bad-network-video-msg-config]]).

2. **`lossHintConfig`** — пороги packet loss. Аналогично.

3. **`directCandidateTypeHintConfig` vs `serverCandidateTypeHintConfig`** — разные пороги для прямого P2P и relay-соединения. Сервер может задать более мягкие пороги для relay.

4. **`android.p2prelay.config`** — конфиг переключения на P2P relay. Сервер задаёт условия, при которых клиент переключается с прямого P2P на relay (что означает маршрутизацию через серверы VK/OK).

## Сводка

`android.rating.limits` — 4-компонентный конфиг оценки качества (RTT/loss/candidate type). `android.p2prelay.config` — конфиг переключения на P2P relay.
