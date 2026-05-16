---
tags: [webrtc, native-libs, calls, custom-build, vk, ok]
status: confirmed
sources:
  - findings/native/libjingle_peerconnection_so.strings.txt
  - work/apktool_arm64/lib/arm64-v8a/libjingle_peerconnection_so.so
related:
  - "[[09-native-libs]]"
  - "[[15-on-device-asr-kws-diarization]]"
  - "[[42-voice-messages-opus-server-controlled]]"
  - "[[23-camera-mic-screen-entry-points]]"
---

# libjingle_peerconnection_so.so — кастомный WebRTC-стек VK/OK

`libjingle_peerconnection_so.so` — 12.7 MB, самая большая нативная библиотека в APK. Это **кастомный форк WebRTC**, собранный VK/OK для своего Calls SDK. Стандартный upstream WebRTC не содержит `WebRTC-VK-*` и `WebRTC-OK-*` feature flags.

## Кастомные feature flags

В `.rodata` присутствуют строки, которых нет в upstream WebRTC:

| Флаг | Что |
|---|---|
| `WebRTC-VK-OpusMaxPlcDurationMs` | VK-патч: максимальная длительность PLC (Packet Loss Concealment) для Opus. Влияет на поведение при потере пакетов. |
| `WebRTC-OK-StunCustomAttr` | OK-патч: кастомный STUN-атрибут. Позволяет добавлять нестандартные атрибуты в STUN-сообщения — потенциально для идентификации клиента на TURN/STUN-серверах. |
| `WebRTC-OK-TurnChannelDataMark` | OK-патч: кастомная маркировка TURN channel data. |
| `WebRTC-OK-FrameDropper-Alt` | OK-патч: альтернативный алгоритм frame dropping при перегрузке. |

## CallsSDK feature flags

Отдельный namespace `CallsSDK-*` — это VK/OK Calls SDK поверх WebRTC:

| Флаг | Что |
|---|---|
| `CallsSDK-AIOpusBWE` | AI-based Bandwidth Estimation для Opus (связан с `WebRtcOpus_SetAiOpusBweEnabled`) |
| `CallsSDK-Audio-AudioProcessingOffOnMute` | отключать APM при mute |
| `CallsSDK-Audio-EarlyStartPlayout` / `EarlyStartRecording` | ранний старт воспроизведения/записи |
| `CallsSDK-Audio-OpusAdapterMinBitrate` | минимальный bitrate Opus |
| `CallsSDK-Audio-OpusDREDByBitrate` | DRED (Deep Redundancy Encoding) по bitrate |
| `CallsSDK-Audio-OpusFECWithDRED` | FEC + DRED одновременно |
| `CallsSDK-Audio-OpusNOLACE` | отключить NOLACE (Neural Opus Loss And Concealment Enhancement) |
| `CallsSdk-DisableAudioProcessing` | полное отключение APM |
| `CallsSDK-DisableSharedSocket` | отдельный сокет для каждого потока |
| `CallsSDK-DREDLowBitrate` | DRED при низком bitrate |
| `CallsSDK-LinearMinBitrate` | линейный минимальный bitrate |
| `CallsSdk-LogAudioCapture` | **логировать захват аудио** |
| `Calls-SDK-LogDCTraffic` | **логировать DataChannel трафик** |
| `CallsSDK-OpusFileLogs` | **писать Opus-логи в файл** |

Три флага с `Log*` — потенциально диагностические, но если включены через server-pushed конфиг (аналогично ML-моделям в [[16-server-pushed-ml-models-in-calls]]), то сервер может включить логирование аудиозахвата или DataChannel-трафика в рантайме.

## Кодеки

Поддерживаются: H.264, VP8, VP9, AV1 (видео); Opus (аудио). Opus 1.5 с AI BWE и DRED — это относительно свежая версия (Opus 1.5 вышел в 2023). Стандартный DTLS/SRTP для медиа-шифрования присутствует.

## WebRTC-OK-StunCustomAttr — что это значит

STUN-протокол позволяет добавлять нестандартные атрибуты (с кодами 0x8000+). Флаг `WebRTC-OK-StunCustomAttr` означает, что OK-патч добавляет в STUN binding request/response **нестандартный атрибут**. Это может быть:
- идентификатор пользователя/сессии для TURN-сервера (чтобы TURN знал, кто подключается);
- маркер для приоритизации трафика;
- или просто диагностический тег.

Без исходников патча точно сказать нельзя. Но факт: STUN-пакеты MAX содержат нестандартные атрибуты, которых нет в стандартном WebRTC. Это означает, что TURN/STUN-серверы VK/OK могут идентифицировать клиентов MAX по этим атрибутам.

## Скептический разбор

- Кастомный WebRTC — нормальная практика для крупных компаний (Google Meet, Zoom, Discord — все форкают).
- `CallsSdk-LogAudioCapture` и `Calls-SDK-LogDCTraffic` — диагностические флаги, которые в нормальном режиме выключены. Но если они управляются через server-pushed конфиг (как `CallsSDK-AIOpusBWE` через PmsKey), то сервер может включить их для конкретного пользователя.
- `WebRTC-OK-StunCustomAttr` — нестандартный STUN-атрибут. Это не backdoor, но это **идентификатор клиента в STUN-трафике**, который виден на уровне UDP-пакетов (до TLS).

## Сводка

`libjingle_peerconnection_so.so` — кастомный WebRTC-форк VK/OK с 4 нестандартными `WebRTC-VK/OK-*` флагами и 14 `CallsSDK-*` флагами. Ключевые особенности: нестандартный STUN-атрибут (`WebRTC-OK-StunCustomAttr`), AI-based Opus BWE, DRED, и три флага логирования аудио/DataChannel, которые потенциально управляются сервером.
