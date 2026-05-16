---
tags: [voice-messages, opus, server-control, audio]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
  - work/jadx_base/sources/defpackage/qp6.java
  - work/jadx_base/sources/defpackage/l6k.java
  - work/jadx_base/sources/defpackage/kd0.java
related:
  - "[[03-pms-server-flags]]"
  - "[[23-camera-mic-screen-entry-points]]"
  - "[[09-native-libs]]"
---

# Voice messages — server-controlled Opus encoder параметры

Голосовые сообщения («голосовухи») в MAX используют **серверно-управляемый OPUS encoder** в качестве альтернативы стандартному `MediaRecorder`/AAC.

## PmsKey

| Ключ | # | Тип | Что |
|---|---|---|---|
| `opus-recorder` | 308 | bool | глобальный switch использования OPUS-рекордера вместо `MediaRecorder` |
| `opus-recorder-bitrate` | 309 | int | bitrate (bps) для OPUS-кодирования |
| `opus-recorder-sample-rate` | 310 | int | sample rate (Hz) — 8000/16000/24000/48000 |
| `audio-msg-transcribe-min-duration` | (косвенно) | — | мин. длина для серверной транскрипции |
| `audio-transcription-locales` | 12 | (список) | разрешённые локали для транскрипции |

`qp6.t2 = ko6(...)`, `qp6.u2 = no6(...)`, `qp6.v2 = oo6(...)` — accessors для трёх полей. `t2`-вариант (`ko6`) возможно даёт boolean, `no6` — long/int, `oo6` — int.

## Что значит

1. **Server-controlled bitrate и sample-rate** — сервер по PmsKey задаёт качество записи голосовых. Меньше bitrate = быстрее, дешевле трафик, но хуже качество. Сервер может в любой момент **тихо снизить качество записываемого голоса** (например, для оптимизации серверного транскрипционного pipeline на стороне сервера).

2. **OPUS-recorder vs MediaRecorder** — две альтернативные реализации захвата голоса. Стандартный `MediaRecorder` (см. [[23-camera-mic-screen-entry-points]] §3, `defpackage/kd0.java`) использует системный path Android/AAC. Альтернатива через нативный OPUS-encoder (вероятно через `libffmpg.so` либо отдельный путь) — даёт больший контроль клиента над форматом, но обходит системные сервисы вроде шумоподавления.

3. **Серверное переключение между двумя путями** — `opus-recorder=false` → `MediaRecorder`, `opus-recorder=true` → нативный OPUS. Решение принимается сервером.

4. **`audio-transcription-locales`** — список локалей (например, `["ru", "en"]`), для которых сервер разрешает on-device или серверную транскрипцию голосовых. Если запись на языке не из списка, транскрипция не делается. Это политическое решение «какие языки транскрибировать».

## Связь с ASR/transcription

С учётом on-device ASR в `libEnhancementLibShared.so` (см. [[15-on-device-asr-kws-diarization]]) и серверного опкода `TRANSCRIBE_MEDIA(202)` (см. [[20-ws-protocol-opcodes]]), полный путь голосового сообщения:

1. Запись через MediaRecorder ИЛИ OPUS-recorder (server choice).
2. Параметры записи (bitrate/sample rate) — server-pushed.
3. Транскрипция: либо on-device через embedded ASR, либо серверная через `TRANSCRIBE_MEDIA`.
4. Транскрипт идёт обратно по WS-сессии.

PmsKey `enable-audio-messages-transcription` — общий kill-switch для фичи транскрипции голосовухи.

## Сводка

Голосовые сообщения в MAX — server-tunable: сервер задаёт encoder (OPUS или AAC), bitrate, sample rate, и whitelist локалей для транскрипции. Это не плохая фича — это нормальная server-tunability — но в комбинации с server-pushed transcription (`TRANSCRIBE_MEDIA(202)`) и on-device ASR в нативной либе складывается в полный server-controlled voice-pipeline.
