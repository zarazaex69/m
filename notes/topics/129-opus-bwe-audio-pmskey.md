---
tags: [calls, opus, bwe, server-control, pms, audio]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/nd1.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[43-libjingle-webrtc-custom-build]]"
  - "[[45-calls-sdk-pmskey-cluster]]"
---

# Opus BWE и calls audio PmsKey — дополнительные описания

| Ключ | # | Default | Описание |
|---|---|---|---|
| `calls-sdk-ai-opus-bwe` | 105 | — | AI-based Bandwidth Estimation для Opus |
| `calls-sdk-linear-opus-bwe` | 106 | false | «Включить Linear Opus BWE» |
| `calls-sdk-log-audio` | 131 | false | «Логгировать локальное аудио» |
| `calls-android-update-endpoint-params` | 132 | false | обновить параметры endpoint |

## Что важно

1. **`calls-sdk-ai-opus-bwe`** — AI-based BWE. Это `CallsSDK-AIOpusBWE` feature flag из [[43-libjingle-webrtc-custom-build]]. Сервер контролирует, использовать ли AI для оценки пропускной способности.

2. **`calls-sdk-linear-opus-bwe`** — «Включить Linear Opus BWE» — альтернативный алгоритм BWE. Взаимоисключающий с AI BWE.

3. **`calls-sdk-log-audio`** — «Логгировать локальное аудио» — подтверждение из [[45-calls-sdk-pmskey-cluster]].

## Сводка

Два алгоритма BWE для Opus: AI-based и Linear. Сервер выбирает алгоритм. `calls-sdk-log-audio` — логирование локального аудио.
