---
tags: [calls, video, audio, server-control, pms, webrtc]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/kd1.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[45-calls-sdk-pmskey-cluster]]"
---

# Calls video/audio transform PmsKey — VTV2, LLA, NIDM

| Ключ | # | Default | Описание |
|---|---|---|---|
| `calls-android-vtv2` | 112 | false | «Использовать видеотрансформер v2» |
| `calls-android-lla` | 113 | false | «Использовать LL audio» |
| `calls-android-nidm` | 114 | false | «Разрешить маппинг только собственного ID» |

## Что важно

1. **`calls-android-vtv2`** — «Использовать видеотрансформер v2» — новая версия видеотрансформера. Server-gated переключение на новый алгоритм обработки видео.

2. **`calls-android-lla`** — «Использовать LL audio» — Low Latency Audio. Server-gated включение режима минимальной задержки аудио.

3. **`calls-android-nidm`** — «Разрешить маппинг только собственного ID» — No ID Mapping. При включении клиент использует только свой ID без маппинга на внешние ID. Это влияет на идентификацию участников звонка.

## Сводка

`calls-android-vtv2` — видеотрансформер v2. `calls-android-lla` — Low Latency Audio. `calls-android-nidm` — маппинг только собственного ID.
