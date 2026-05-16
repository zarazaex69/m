---
tags: [calls, webrtc, server-control, ev1, config]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ev1.java
related:
  - "[[45-calls-sdk-pmskey-cluster]]"
  - "[[134-webrtc-field-trials]]"
---

# ev1 — полная конфигурация WebRTC сессии

`defpackage/ev1.java` — toString() конфигурации WebRTC сессии. Содержит все параметры, которые применяются к PeerConnection.

## Ключевые поля (из toString)

| Поле | Что |
|---|---|
| `isWebTransportEnabled` | WebTransport включён |
| `wtToWsFallbackParams` | параметры fallback WS→WT |
| `useP2PRelayEnabled` | P2P relay включён |
| `considerP2PRelayCapabilityEnabled` | учитывать capability relay |
| `aiOpusBweConfig` | конфиг AI Opus BWE |
| `isH265Prioritized` | H265 приоритизирован |
| `isLinearBweEnabled` | Linear BWE включён |
| `isAdaptiveOpusComplexityEnabled` | адаптивная сложность Opus |
| `isAudioRecordEnabledOnStart` | аудио включено на старте |
| `isAudioPipelineDisabled` | аудио пайплайн отключён |
| `isAudioCaptureLoggingEnabled` | логирование захвата аудио |
| `simulcastState` | DISABLED/ONLY_SW_VP8/ALL_SUPPORTED_CODEC |
| `emulatedSignalingError` | NONE/SERVICE_UNAVAILABLE/PARTICIPANT_LIMIT_REACHED |
| `emulatedIceCandidateError` | NONE/REMOTE/LOCAL |
| `isSignalingByIpEnabled` | сигналинг по IP |
| `isReplaceParametersInEndpointEnabled` | замена параметров endpoint |
| `isUseGeneratedPeerIdEnabled` | клиентская генерация peer ID |
| `isDirectICERestartEnabled` | прямой ICE restart |
| `bitrateDumpGatheringState` | состояние сбора bitrate dump |
| `isEarlyApplyRemoteOfferEnabled` | ранняя обработка оффера |
| `isSummaryStatsEnabled` | сводная статистика |
| `isSignalingLogThrottlingEnabled` | throttling логов сигналинга |
| `isDtxDenoiseEnabled` | DTX denoise |
| `isCallEndedReasonFixEnabled` | фикс причины завершения звонка |

## Что важно

1. **`emulatedSignalingError`** — сервер может эмулировать ошибку сигналинга: `SERVICE_UNAVAILABLE` или `PARTICIPANT_LIMIT_REACHED`. Это означает, что сервер может принудительно завершить звонок с определённой ошибкой.

2. **`emulatedIceCandidateError`** — эмуляция ошибки ICE candidates. Сервер может эмулировать проблемы с ICE.

3. **`isAudioCaptureLoggingEnabled`** — логирование захвата аудио. Это дополнение к `calls-sdk-log-audio` из [[45-calls-sdk-pmskey-cluster]].

4. **`simulcastState`** — три режима simulcast: DISABLED, ONLY_SW_VP8, ALL_SUPPORTED_CODEC.

## Сводка

`ev1` — полная конфигурация WebRTC сессии с 25+ параметрами. Ключевые: `emulatedSignalingError` (сервер может принудительно завершить звонок), `emulatedIceCandidateError`, `isAudioCaptureLoggingEnabled`, `simulcastState`.
