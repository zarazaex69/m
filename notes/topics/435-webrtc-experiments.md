---
tags: [calls, experiments, webrtc, server-control, surveillance, bitrate-dump]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ev1.java
related:
  - "[[384-externcalls-sdk-config]]"
  - "[[401-media-dump-manager]]"
  - "[[414-conversation-factory]]"
---

# ev1 — Experiments (WebRTC эксперименты)

`ev1` — объект экспериментов WebRTC. Все параметры управляются сервером через `android.dump.bitrate` и другие remote settings.

## Ключевые флаги

| Флаг | Что |
|---|---|
| `isCamera2ApiEnabled` | Camera2 API |
| `isVideoDegradationForSwCodecEnabled` | деградация видео для SW кодека |
| `isNonOpusRemovalEnabled` | удаление не-Opus кодеков |
| `isEnqueuedCommandMergeEnabled` | слияние команд |
| `isDynamicScreenShareSizeUpdateEnabled` | динамическое обновление размера screen share |
| `isBackendRenderVmojiEnabled` | **серверный рендеринг анимоджи** |
| `isFilterCallMuteStateInitForAdmins` | фильтр mute для администраторов |
| `isInCallAnalyticsUploadEnabled` | аналитика во время звонка |
| `callAnalyticsUploadMaxLoss` | максимальные потери для аналитики |
| `callAnalyticsUploadMinBitrate` | минимальный битрейт для аналитики |
| `skipRequestReallocEnabled` | пропуск realloc |
| `isWebTransportEnabled` | WebTransport |
| `useP2PRelayEnabled` | P2P relay |
| `considerP2PRelayCapabilityEnabled` | учитывать capability P2P relay |
| `isH265Prioritized` | приоритет H.265 |
| `isLinearBweEnabled` | линейная оценка пропускной способности |
| `isAdaptiveOpusComplexityEnabled` | адаптивная сложность Opus |
| `isAudioRecordEnabledOnStart` | запись аудио при старте |
| `isAudioPipelineDisabled` | отключение аудио pipeline |
| `isAudioCaptureLoggingEnabled` | **логирование захвата аудио** |
| `isCorruptWsEndpointEnabled` | повреждённый WS endpoint |
| `bitrateDumpGatheringState` | **состояние сбора дампа битрейта** |
| `isNsEnabled` | NS (Noise Suppression) |
| `isLowLatencyAudioEnabled` | низкая задержка аудио |
| `isOnlySoftwareEncodersEnabled` | только SW кодеки |
| `isSignalingByIpEnabled` | signaling по IP |
| `isDirectICERestartEnabled` | прямой ICE restart |
| `isTokenInvalidationEnabled` | инвалидация токена |
| `isDoNothingOnIceFailureEnabled` | игнорировать ICE failure |

## Что важно

1. **`isAudioCaptureLoggingEnabled`** — логирование захвата аудио. Сервер может включить.

2. **`bitrateDumpGatheringState`** — состояние сбора дампа битрейта. Управляется через `android.dump.bitrate`.

3. **`isBackendRenderVmojiEnabled`** — серверный рендеринг анимоджи означает передачу данных на сервер.

4. **`isCorruptWsEndpointEnabled`** — тестовый флаг для повреждённого WS endpoint.

## Сводка

`ev1` (Experiments): 30+ флагов WebRTC. Ключевые: `isAudioCaptureLoggingEnabled`/`bitrateDumpGatheringState`/`isBackendRenderVmojiEnabled`/`isWebTransportEnabled`/`isH265Prioritized`.
