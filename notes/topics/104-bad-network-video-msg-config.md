---
tags: [calls, network, server-control, pms, video-messages]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ak1.java
  - work/jadx_base/sources/defpackage/usj.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[03-pms-server-flags]]"
  - "[[45-calls-sdk-pmskey-cluster]]"
---

# bad-networ-indicator-config и video-msg-config

## bad-networ-indicator-config (#14)

`PmsKey.f14badnetworindicatorconfig` — JSON-конфиг индикатора плохой сети во время звонков. Default:

```json
{
  "rtt": {
    "step": 0.055, "baseline": 0.4,
    "stepWeight": 0.12, "weightUp": 0.3, "weightDown": 0.8
  },
  "loss": {
    "step": 1.5, "baseline": 0.0,
    "stepWeight": 0.17, "weightUp": 0.3, "weightDown": 0.6
  }
}
```

Это алгоритм вычисления «плохой сети» на основе RTT и packet loss. Сервер контролирует пороги и веса. При изменении параметров — индикатор плохой сети будет срабатывать чаще или реже.

## video-msg-config (#297)

`PmsKey.f297videomsgconfig` — JSON-конфиг видеосообщений. Используется в `usj.java` для настройки параметров записи/воспроизведения видеосообщений.

## Сводка

`bad-networ-indicator-config` — server-pushed алгоритм детекции плохой сети (RTT/loss пороги и веса). `video-msg-config` — server-pushed конфиг видеосообщений.
