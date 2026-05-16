---
tags: [calls, audio, server-control, pms, pipeline, simulcast]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/kd1.java
  - work/jadx_base/sources/defpackage/nd1.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[45-calls-sdk-pmskey-cluster]]"
  - "[[54-ws-session-config-fingerprint]]"
---

# Calls audio pipeline и simulcast PmsKey

| Ключ | # | Default | Описание |
|---|---|---|---|
| `calls-sdk-disable-pipeline` | 130 | false | «Отключить аудио пайплайн» |
| `calls-sdk-dnt-disable-audio` | 134 | false | «Не блокировать звук на старте» |
| `calls-android-simulcast-sw-vp8` | 133 | false | «Включить sw VP8 simulcast» |
| `calls-android-signaling-ip` | 119 | — | «Подключаться к сигналингу по IP» |

## Что важно

1. **`calls-sdk-disable-pipeline`** — «Отключить аудио пайплайн» — отключает весь APM (AEC, NS, AGC). Уже упомянуто в [[45-calls-sdk-pmskey-cluster]], теперь с точным описанием.

2. **`calls-sdk-dnt-disable-audio`** — «Не блокировать звук на старте» — при включении аудио не блокируется в начале звонка. Это UX-оптимизация.

3. **`calls-android-simulcast-sw-vp8`** — «Включить sw VP8 simulcast» — software VP8 simulcast. Simulcast = отправка нескольких потоков разного качества одновременно.

4. **`calls-android-signaling-ip`** — «Подключаться к сигналингу по IP» — использовать IP вместо hostname для сигналинга. Уже упомянуто в [[45-calls-sdk-pmskey-cluster]].

5. Оба `calls-sdk-disable-pipeline` и `calls-sdk-dnt-disable-audio` отправляются в WS session config (см. [[54-ws-session-config-fingerprint]]).

## Сводка

`calls-sdk-disable-pipeline` — «Отключить аудио пайплайн». `calls-sdk-dnt-disable-audio` — «Не блокировать звук на старте». `calls-android-simulcast-sw-vp8` — VP8 simulcast.
