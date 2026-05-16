---
tags: [calls, webrtc, field-trials, server-control, audio, bwe]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/rj5.java
  - work/jadx_base/sources/defpackage/ead.java
related:
  - "[[43-libjingle-webrtc-custom-build]]"
  - "[[45-calls-sdk-pmskey-cluster]]"
---

# WebRTC Field Trials — hardcoded и server-controlled флаги

`rj5.java` — инициализация `PeerConnectionFactory` с field trials. Это строка флагов, передаваемых в WebRTC engine.

## Hardcoded field trials (всегда включены)

| Флаг | Что |
|---|---|
| `WebRTC-Bwe-LossBasedBweV2/Enabled:true,...` | Loss-based BWE v2 с детальными параметрами |
| `CallsSDK-Audio-OpusNOLACE/Enabled/` | отключить NOLACE (Neural Opus Loss And Concealment Enhancement) |
| `WebRTC-AdjustOpusBandwidth/Enabled/` | адаптивная полоса пропускания Opus |
| `CallsSDK-DREDLowBitrate/Enabled/` | DRED при низком bitrate |
| `WebRTC-Audio-StableTargetAdaptation/Enabled/` | стабильная адаптация целевого bitrate |
| `CallsSDK-Audio-OpusAdapterMinBitrate/Enabled:16000/` | минимальный bitrate Opus 16kbps |
| `WebRTC-Audio-AdaptivePtime/enabled:true,...` | адаптивный packet time |
| `CallsSDK-DisableSharedSocket/Enabled/` | отключить shared socket |

## Условные field trials (server-controlled)

| Условие | Флаг |
|---|---|
| `padVar.d` | `CallsSDK-Audio-EarlyStartPlayout/Enabled/` |
| `padVar.e` | `CallsSDK-Audio-EarlyStartRecording/Enabled/` |
| `padVar.f` | `CallsSDK-Audio-AudioProcessingOffOnMute/Enabled/` |
| `str2 != null` | дополнительные флаги из server config |
| `strN != null` | ещё дополнительные флаги |

## DRED в ead.java

`ead.java:66` — добавляет `dred=100` и `red` в SDP для аудио. DRED (Deep Redundancy Encoding) — механизм восстановления аудио при потере пакетов.

## Что важно

1. **Hardcoded field trials** — 8 флагов всегда включены. Это означает, что VK/OK сделали специфические оптимизации WebRTC, которые нельзя отключить через PmsKey.

2. **Условные флаги** — `padVar.d/e/f` — вероятно, управляются через PmsKey или RemoteSettings. Сервер может включить/выключить EarlyStartPlayout, EarlyStartRecording, AudioProcessingOffOnMute.

3. **`str2` и `strN`** — дополнительные field trials из server config. Сервер может передать произвольные WebRTC field trials.

## Сводка

8 hardcoded WebRTC field trials + условные (server-controlled). Сервер может передать произвольные WebRTC field trials через `str2`/`strN`. DRED hardcoded в SDP.
