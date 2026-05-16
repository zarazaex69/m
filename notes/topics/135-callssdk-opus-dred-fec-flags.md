---
tags: [calls, webrtc, opus, dred, fec, server-control]
status: confirmed
sources:
  - findings/native/libjingle_peerconnection_so.strings.txt
  - work/jadx_base/sources/defpackage/rj5.java
related:
  - "[[43-libjingle-webrtc-custom-build]]"
  - "[[134-webrtc-field-trials]]"
---

# CallsSDK Opus DRED/FEC feature flags — нативные флаги

В `libjingle_peerconnection_so.so` присутствуют дополнительные `CallsSDK-*` флаги, не упомянутые в [[43-libjingle-webrtc-custom-build]]:

| Флаг | Что |
|---|---|
| `CallsSDK-Audio-OpusFECWithDRED` | FEC (Forward Error Correction) совместно с DRED |
| `CallsSDK-Audio-OpusDREDByBitrate` | DRED включается по bitrate |
| `CallsSDK-DREDLowBitrate` | DRED при низком bitrate (hardcoded в field trials, см. [[134-webrtc-field-trials]]) |
| `CallsSDK-LinearMinBitrate` | минимальный bitrate для линейного BWE |
| `CallsSDK-OpusFileLogs` | запись Opus-логов в файл |
| `Calls-SDK-LogDCTraffic` | логирование DataChannel трафика |

## Что важно

1. **`CallsSDK-OpusFileLogs`** — запись Opus-логов в файл. Если включён через server-pushed конфиг, клиент записывает детальные логи Opus-кодека в файл. Эти файлы могут быть загружены через `vchat.getLogUploadUrl` (см. [[96-vchat-api-methods]]).

2. **`Calls-SDK-LogDCTraffic`** — логирование DataChannel трафика. DataChannel используется для передачи данных в WebRTC (не медиа). Если включён — все данные DataChannel логируются.

3. **`CallsSDK-Audio-OpusFECWithDRED`** — FEC + DRED одновременно. Это максимальная защита от потери пакетов, но с overhead.

4. Все эти флаги управляются через `bonusFieldTrials` (см. [[134-webrtc-field-trials]]) — сервер может включить любой из них.

## Сводка

6 дополнительных `CallsSDK-*` флагов в нативной либе: `OpusFECWithDRED`, `OpusDREDByBitrate`, `DREDLowBitrate`, `LinearMinBitrate`, `OpusFileLogs` (запись в файл), `LogDCTraffic` (логирование DataChannel). Управляются через `bonusFieldTrials`.
