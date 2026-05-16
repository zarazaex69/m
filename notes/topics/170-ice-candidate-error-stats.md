---
tags: [calls, ice, stats, errors, telemetry, network]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/stat/candidate/IceCandidateGatheringFailedStat.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/stat/candidate/IceCandidateAddFailedStat.java
related:
  - "[[163-conversation-stats]]"
  - "[[165-ice-candidate-pair-stat]]"
---

# IceCandidateGatheringFailedStat и IceCandidateAddFailedStat

## IceCandidateGatheringFailedStat

Событие при ошибке сбора ICE candidates:

| Поле | Что |
|---|---|
| `local_address` | **локальный IP-адрес** |
| `remote_url` | **URL удалённого STUN/TURN сервера** |
| `transport` | транспортный протокол |
| `string_value` | описание ошибки |
| value | код ошибки |

## IceCandidateAddFailedStat

Событие `ice_candidate_add_failed` при ошибке добавления ICE candidate:

| Поле | Что |
|---|---|
| `remote_url` | **URL удалённого сервера** |
| `string_value` | описание ошибки |
| value | код ошибки |

## Что важно

1. **`local_address`** — при ошибке сбора ICE сервер получает локальный IP-адрес клиента.

2. **`remote_url`** — URL STUN/TURN сервера, с которым была ошибка. Это позволяет серверу диагностировать проблемы с конкретными серверами.

3. Оба события входят в `ConversationStats` (см. [[163-conversation-stats]]).

## Сводка

`IceCandidateGatheringFailedStat` — `local_address`/`remote_url`/`transport` при ошибке сбора ICE. `IceCandidateAddFailedStat` — `ice_candidate_add_failed` с `remote_url`.
