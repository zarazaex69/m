---
tags: [calls, server-control, pms, noise-suppression, fast-join]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/kd1.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[45-calls-sdk-pmskey-cluster]]"
---

# Calls fast-join, NS, new-pms PmsKey

| Ключ | # | Default | Описание |
|---|---|---|---|
| `calls-android-fast-join` | 120 | false | «Быстрое присоединение через клиентский бекенд» |
| `calls-android-ns` | 121 | false | «Включить шумодав» |
| `calls-android-ac` | 122 | — | Acoustic Cancellation |
| `calls-android-new-pms` | 123 | false | «Новый механизм получения звонковых настроек» |
| `calls-android-ice-cps` | 115 | — | ICE candidate pool size |
| `calls-android-wtp` | 126 | — | WebTransport параметры |

## Что важно

1. **`calls-android-ns`** — «Включить шумодав» — server-gated Noise Suppression. Это отдельно от `libEnhancementLibShared.so` NS (см. [[15-on-device-asr-kws-diarization]]). Это NS в звонках через WebRTC.

2. **`calls-android-fast-join`** — «Быстрое присоединение через клиентский бекенд» — оптимизация задержки при присоединении к звонку.

3. **`calls-android-new-pms`** — «Новый механизм получения звонковых настроек» — server-gated переключение на новый механизм получения PmsKey для звонков.

## Сводка

`calls-android-ns` — server-gated шумодав в звонках. `calls-android-fast-join` — быстрое присоединение. `calls-android-new-pms` — новый механизм получения настроек.
