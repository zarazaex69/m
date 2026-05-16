---
tags: [calls, p2p, relay, server-control, pms, webrtc]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/kd1.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[45-calls-sdk-pmskey-cluster]]"
  - "[[43-libjingle-webrtc-custom-build]]"
  - "[[69-remote-settings-calls-sdk]]"
---

# P2P relay и WebTransport PmsKey — маршрутизация звонков

| Ключ | # | Default | Описание |
|---|---|---|---|
| `calls-use-p2p-relay` | 61 | false | «Использовать p2p relay» |
| `calls-use-p2p-relay-caps` | 62 | false | «Учитывать p2p relay capability» |
| `calls-sdk-wt-enabled` | 102 | false | «Включить WebTransport» |
| `calls-sdk-am-speaker-fix` | 101 | false | «Последовательное переключение аудио устройств» |
| `calls-sdk-webrtc-logs` | 103 | false | WebRTC логи |

## Что важно

1. **`calls-use-p2p-relay`** — server-gated использование P2P relay. При включении звонки идут через relay-сервер вместо прямого P2P. Это означает, что сервер может перенаправить весь медиа-трафик звонков через свои серверы.

2. **`calls-use-p2p-relay-caps`** — учитывать capability устройства при выборе relay. Если устройство не поддерживает прямой P2P — использовать relay.

3. **`calls-sdk-wt-enabled`** — WebTransport вместо WebRTC DataChannel. Уже упомянуто в [[45-calls-sdk-pmskey-cluster]].

4. **`android.p2prelay.config`** в RemoteSettings (см. [[69-remote-settings-calls-sdk]]) — дополнительный конфиг P2P relay.

## Сводка

`calls-use-p2p-relay` — server-gated перенаправление медиа-трафика звонков через relay-серверы. При включении сервер получает полный контроль над маршрутизацией медиа.
