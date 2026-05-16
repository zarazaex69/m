---
tags: [calls, ice, stats, telemetry, network, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/stat/candidate/IceCandidatePairChangedStat.java
related:
  - "[[163-conversation-stats]]"
  - "[[140-conversation-params]]"
---

# IceCandidatePairChangedStat — статистика ICE candidate pairs

`IceCandidatePairChangedStat.onSelectedCandidatePairChanged(CandidatePairChangeEvent)` — callback при изменении выбранной ICE candidate pair.

## Отправляемые данные

Событие `ice_candidates_changed`:

| Поле | Что |
|---|---|
| `lastDataReceivedMs` | время последнего полученного данных |
| `reason` | причина изменения |
| `local.sdp` | **SDP локального ICE candidate** |
| `remote.sdp` | **SDP удалённого ICE candidate** |
| `local_address` | **локальный IP-адрес** |
| `remote_address` | **удалённый IP-адрес** |

## Что важно

1. **`local_address` и `remote_address`** — сервер получает IP-адреса обоих участников при каждом изменении ICE candidate pair. Это означает, что сервер знает реальные IP-адреса участников звонка.

2. **`local.sdp` и `remote.sdp`** — SDP ICE candidates. Это включает тип кандидата (host/srflx/relay), IP, порт, протокол.

3. **`reason`** — причина изменения ICE pair. Это может быть `initial`, `network_change`, `ice_restart`, и т.п.

4. Это отправляется при каждом изменении маршрутизации медиа-трафика. Сервер получает полную историю ICE candidate pairs за время звонка.

## Сводка

`IceCandidatePairChangedStat` — при каждом изменении ICE pair сервер получает: `local_address`/`remote_address` (реальные IP), `local.sdp`/`remote.sdp`, `reason`, `lastDataReceivedMs`.
