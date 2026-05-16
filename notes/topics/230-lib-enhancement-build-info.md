---
tags: [native, build-info, infrastructure, tflite, kaldi, webrtc]
status: confirmed
sources:
  - findings/native/libEnhancementLibShared.strings.txt
related:
  - "[[229-lib-enhancement-exports]]"
  - "[[15-on-device-asr-kws-diarization]]"
---

# libEnhancementLibShared — build paths и внутренняя инфраструктура

Отладочные строки в `libEnhancementLibShared.so` раскрывают внутреннюю инфраструктуру VK.

## Build path

```
/home/good/mainframer/webrtc4/src/modules/audio_processing/one-ann-audio-processing/
```

- **`mainframer`** — инструмент удалённой сборки (Mainframer). Сборка происходит на удалённом сервере.
- **`webrtc4`** — 4-я версия кастомного WebRTC репозитория VK.
- **`one-ann-audio-processing`** — внутреннее название модуля аудио-обработки.

## Зависимости

| Зависимость | Что |
|---|---|
| TensorFlow Lite | ML-инференс (KWS, ASR, NS) |
| Kaldi | ASR фреймворк (`kaldi/src/feat/feature-window.cc`) |

## Что важно

1. **`one-ann-audio-processing`** — внутреннее название. `one` = ONE.me (MAX). Это собственная разработка VK/ONE.me.

2. **Kaldi** — профессиональный ASR фреймворк. Используется для извлечения признаков (`feature-window.cc`).

3. **TensorFlow Lite** — для ML-инференса KWS/NS/ASR моделей.

4. **`webrtc4`** — 4-я версия кастомного WebRTC. Это значит, что VK активно форкает и модифицирует WebRTC.

## Сводка

Build path: `/home/good/mainframer/webrtc4/src/.../one-ann-audio-processing`. Зависимости: TFLite + Kaldi. Внутреннее название модуля: `one-ann-audio-processing`.
