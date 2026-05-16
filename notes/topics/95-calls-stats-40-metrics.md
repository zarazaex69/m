---
tags: [calls, telemetry, surveillance, webrtc, stats]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/w32.java
  - work/jadx_base/sources/defpackage/ra1.java
related:
  - "[[71-call-onelog-events]]"
  - "[[45-calls-sdk-pmskey-cluster]]"
  - "[[94-battery-memory-monitoring]]"
---

# Calls stats — 40 метрик звонков отправляемых на сервер

`defpackage/w32.java` — полный список метрик, которые собираются во время звонков и отправляются на сервер.

## Полный список метрик

| # | Метрика | Ключ | Что |
|---|---|---|---|
| 0 | RTT | `rtt` | Round-Trip Time |
| 1 | ScreenShareFreezeCount | `ss_freeze_count` | количество фризов screenshare |
| 2 | ScreenShareFreezeDuration | `ss_total_freezes_duration` | суммарная длительность фризов |
| 3 | CpuUsagePercentTotal | `cpu_usage_percent_total` | **загрузка CPU** |
| 4 | CpuScoreMax | `cpu_score_max` | максимальный CPU score |
| 5 | CpuScoreAvg | `cpu_score_avg` | средний CPU score |
| 6 | CpuHardwareConcurrency | `cpu_hardware_concurrency` | количество ядер CPU |
| 7 | MemoryUsageMbMax | `memory_usage_mb_max` | **максимальное использование памяти** |
| 8 | MemoryUsageMbAvg | `memory_usage_mb_avg` | среднее использование памяти |
| 9 | BatteryLevelChange | `battery_level_change` | **изменение заряда батареи** |
| 10 | InsertedAudioSamplesForDeceleration | `inserted_audio_samples_for_deceleration` | аудио-семплы для замедления |
| 11 | RemovedAudioSamplesForAcceleration | `removed_audio_samples_for_acceleration` | аудио-семплы для ускорения |
| 12 | ConcealedAudioSamples | `concealed_audio_samples` | скрытые аудио-семплы (PLC) |
| 13 | JitterAudio | `jitter_audio` | джиттер аудио |
| 14 | ConcealedSilentAudioSamples | `concealed_silent_audio_samples` | скрытые тихие семплы |
| 15 | ConcealmentAudioAverageSize | `concealment_audio_avg_size` | средний размер concealment |
| 16 | AudioLevel | `audio_level` | **уровень аудио** |
| 17 | AudioBytesSent | `audio_bytes_sent` | байты аудио отправлено |
| 18 | TotalAudioEnergy | `total_audio_energy` | суммарная энергия аудио |
| 19 | AudioLossIn | `in_audio_loss` | потери входящего аудио |
| 20 | AudioLoss | `audio_loss` | потери аудио |
| 21 | VideoNackSent | `nack_sent` | NACK отправлено |
| 22 | VideoPliSent | `pli_sent` | PLI отправлено |
| 23 | VideoFirSent | `fir_sent` | FIR отправлено |
| 24 | VideoFramesDecoded | `frames_decoded` | декодировано кадров |
| 25 | VideoFramesDropped | `frames_dropped` | пропущено кадров |
| 26 | VideoJitter | `jitter_video` | джиттер видео |
| 27 | VideoInterframeDelayVariance | `interframe_delay_variance` | вариация задержки между кадрами |
| 28 | VideoFreezeCount | `freeze_count` | количество фризов видео |
| 29 | VideoTotalFreezesDuration | `total_freezes_duration` | суммарная длительность фризов |
| 30 | VideoLossIn | `in_video_loss` | потери входящего видео |
| 31 | VideoLoss | `video_loss` | потери видео |
| 32 | VideoNackReceived | `nack_received` | NACK получено |
| 33 | VideoPliReceived | `pli_received` | PLI получено |
| 34 | VideoFirReceived | `fir_received` | FIR получено |
| 35 | VideoAdaptationChanges | `adaptation_changes` | изменения адаптации |
| 36 | VideoFramesEncoded | `frames_encoded` | закодировано кадров |
| 37 | VideoBrEncode | `br_encode` | bitrate кодирования |
| 38 | VideoBrTransmit | `br_transmit` | bitrate передачи |
| 39 | VideoBrRetransmit | `br_retransmit` | bitrate повторной передачи |

## Что важно

1. **CPU и Memory метрики** — сервер получает загрузку CPU и использование памяти во время каждого звонка. Это детальный профиль производительности устройства.

2. **`AudioLevel(16)`** — уровень аудио во время звонка. Это не содержимое разговора, но сервер знает, когда пользователь говорит (высокий уровень) и когда молчит.

3. **`BatteryLevelChange(9)`** — изменение заряда батареи за время звонка.

4. **40 метрик** — это полный WebRTC stats snapshot, который отправляется на сервер после каждого звонка.

## Сводка

40 метрик звонков: CPU/Memory/Battery, аудио-качество (jitter, loss, concealment), видео-качество (frames, freezes, bitrate). Сервер получает детальный технический профиль каждого звонка, включая загрузку CPU и уровень аудио.
