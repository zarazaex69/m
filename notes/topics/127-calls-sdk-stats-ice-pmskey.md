---
tags: [calls, server-control, pms, stats, ice, webrtc]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/kd1.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[45-calls-sdk-pmskey-cluster]]"
---

# Calls SDK stats и ICE PmsKey — дополнительные описания

| Ключ | # | Default | Описание |
|---|---|---|---|
| `calls-sdk-webrtc-logs` | 103 | false | «Логгирование WebRtc в звонках» |
| `calls-sdk-incall-stat` | 109 | false | «Отправлять статистику во время звонка» |
| `calls-sdk-opus-adapt` | 135 | false | «Адаптивная complexity опус» |
| `calls-android-direct-ice-restart` | 110 | false | «Явный ICE рестарт при реконнекте» |
| `calls-android-no-ice-restart` | 111 | false | «Не делать ICE рестарт при реконнекте» |

## Что важно

1. **`calls-sdk-incall-stat`** — «Отправлять статистику во время звонка». При включении клиент отправляет статистику в реальном времени во время звонка (не только после). Это дополнение к 40 метрикам из [[95-calls-stats-40-metrics]].

2. **`calls-android-direct-ice-restart`** vs **`calls-android-no-ice-restart`** — взаимоисключающие флаги. Сервер может запретить ICE restart при реконнекте, что ухудшает восстановление соединения.

3. **`calls-sdk-opus-adapt`** — «Адаптивная complexity опус» — адаптивная сложность кодирования Opus. Оптимизация CPU.

## Сводка

Дополнительные описания calls SDK PmsKey: `calls-sdk-incall-stat` (real-time stats), `calls-android-direct-ice-restart`/`no-ice-restart` (ICE restart control), `calls-sdk-opus-adapt` (adaptive Opus complexity).
