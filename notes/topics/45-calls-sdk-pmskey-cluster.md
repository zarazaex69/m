---
tags: [calls, webrtc, server-control, pms, logging, audio]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
  - work/jadx_base/sources/defpackage/qp6.java
related:
  - "[[43-libjingle-webrtc-custom-build]]"
  - "[[03-pms-server-flags]]"
  - "[[23-camera-mic-screen-entry-points]]"
  - "[[42-voice-messages-opus-server-controlled]]"
---

# Calls SDK — 30+ server-controlled PmsKey для звонков

Звонковый стек MAX управляется через 30+ PmsKey семейств `calls-sdk-*` и `calls-android-*`. Это самый большой кластер PmsKey в приложении.

## calls-sdk-* (SDK-уровень)

| PmsKey | # | Что |
|---|---|---|
| `calls-sdk-webrtc-logs` | 103 | включить WebRTC-логи (в logcat/файл) |
| `calls-sdk-traffic-markup` | 104 | маркировка трафика звонков (DSCP/QoS) |
| `calls-sdk-ai-opus-bwe` | 105 | AI-based Bandwidth Estimation для Opus (см. [[43-libjingle-webrtc-custom-build]]) |
| `calls-sdk-linear-opus-bwe` | 106 | линейный BWE для Opus |
| `calls-sdk-incall-stat` | 109 | статистика во время звонка |
| `calls-sdk-disable-pipeline` | 130 | отключить audio processing pipeline |
| `calls-sdk-log-audio` | 131 | **логировать аудио-захват** |
| `calls-sdk-dnt-disable-audio` | 134 | отключить аудио (Do Not Transmit) |
| `calls-sdk-opus-adapt` | 135 | адаптивный Opus |
| `calls-sdk-h265-prioritized` | (125) | приоритизировать H.265 |
| `calls-sdk-am-speaker-fix` | 101 | фикс AudioManager speaker |
| `calls-sdk-wt-enabled` | 102 | WebTransport вместо WebRTC DataChannel |

## calls-android-* (Android-специфика)

| PmsKey | # | Что |
|---|---|---|
| `calls-android-direct-ice-restart` | 110 | прямой ICE restart |
| `calls-android-no-ice-restart` | 111 | запретить ICE restart |
| `calls-android-vtv2` | 112 | Video Track v2 |
| `calls-android-lla` | 113 | Low Latency Audio |
| `calls-android-nidm` | 114 | Network Interface Detection Mode |
| `calls-android-ice-cps` | 115 | ICE candidate pair selection |
| `calls-android-h265-s` | 116 | H.265 software encoder |
| `calls-android-ssttl` | 117 | SDP session TTL |
| `calls-android-early-create-pc` | 118 | создать PeerConnection заранее |
| `calls-android-signaling-ip` | 119 | **IP-адрес сигналинг-сервера** (server-pushed) |
| `calls-android-fast-join` | 120 | быстрое подключение к звонку |
| `calls-android-ns` | 121 | Noise Suppression |
| `calls-android-ac` | 122 | Acoustic Cancellation |
| `calls-android-new-pms` | 123 | новый формат PMS-конфига для звонков |
| `calls-android-signaling-to` | 124 | **timeout сигналинга** |
| `calls-android-wtp` | 126 | WebTransport параметры |
| `calls-android-update-endpoint-params` | 132 | обновить параметры endpoint |
| `calls-android-simulcast-sw-vp8` | 133 | simulcast VP8 software |
| `calls-android-early-set-offer` | 136 | ранняя установка SDP offer |
| `calls-android-gen-peerid` | 137 | генерация peer ID |

## Что важно

1. **`calls-sdk-log-audio` (#131)** — server-gated включение логирования аудио-захвата. Соответствует `CallsSdk-LogAudioCapture` feature flag в `libjingle_peerconnection_so.so` (см. [[43-libjingle-webrtc-custom-build]]). Если сервер включает этот PmsKey, клиент начинает логировать захваченный аудио-поток. Куда идут логи — в logcat или в файл (через `CallsSDK-OpusFileLogs`) — зависит от дополнительных флагов.

2. **`calls-android-signaling-ip` (#119)** — IP-адрес сигналинг-сервера задаётся сервером через PmsKey. Это означает, что сервер может перенаправить звонковый сигналинг на произвольный IP без обновления клиента.

3. **`calls-sdk-wt-enabled` (#102)** — WebTransport вместо WebRTC DataChannel. WebTransport — более новый протокол поверх HTTP/3 (QUIC). Если включён, звонки идут через QUIC, а не через DTLS/SRTP. Это меняет модель шифрования и трассировки трафика.

4. **`calls-android-no-ice-restart` (#111)** vs **`calls-android-direct-ice-restart` (#110)** — взаимоисключающие флаги управления ICE restart. Сервер может запретить ICE restart (что ухудшает восстановление соединения при смене сети) или форсировать прямой restart.

5. **`calls-sdk-disable-pipeline` (#130)** — отключить весь audio processing pipeline (AEC, NS, AGC). Если включён, аудио идёт без обработки — что может быть нужно для диагностики, но также означает, что сервер может отключить шумоподавление для конкретного пользователя.

## Сводка

30+ PmsKey управляют каждым аспектом звонкового стека: кодеки, ICE, сигналинг-IP, audio processing, логирование. Ключевые риски: `calls-sdk-log-audio` (server-gated аудио-логирование), `calls-android-signaling-ip` (server-pushed сигналинг-сервер), `calls-sdk-wt-enabled` (переключение на WebTransport/QUIC).
