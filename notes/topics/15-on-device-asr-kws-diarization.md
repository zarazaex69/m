---
tags: [surveillance, ml, calls, asr, kws, native-libs, partially-removed-in-26.16.0]
status: partially-removed-in-26.16.0
sources:
  - findings/native/libEnhancementLibShared.exports.txt
  - findings/native/libEnhancementLibShared.strings.txt
  - work/jadx_base/sources/defpackage/sgj.java
  - work/jadx_base/sources/defpackage/ma4.java
related:
  - "[[09-native-libs]]"
  - "[[16-server-pushed-ml-models-in-calls]]"
  - "[[03-pms-server-flags]]"
  - "[[11-state-bots-and-content-policy]]"
  - "[[530-version-26.16.0-diff]]"
---

> ## ⚠️ В MAX 26.16.0 — KWS-часть удалена из libEnhancementLibShared
>
> ❌ `vk::enh::BCResNetKWS` (класс + конструктор + computeProbs/doScoreData/extractFeatures/reset + 3 перегрузки `score`) — **удалён**  
> ❌ `vk::enh::BCResNetExternalStateKWS` (тот же набор методов) — **удалён**  
> ❌ `vk::enh::FeatureExtractor` (extract, extract_, fft, spectrogram, buildHammingWindow + конструктор) — **удалён**  
> ❌ `vk::enh::buildMelBasis` — удалён  
> ✅ `vk::enh::AsrService` (Conformer-CTC ASR) — **на месте**  
> ✅ `vk::enh::SpeakerRecognitionEngineFactory` / `SpeakerRecognitionVerifierFactory` — **на месте**  
> ✅ Diarization — на месте  
> ✅ Wav2Lip (animoji) — на месте  
> ✅ Denoise — на месте
>
> Размер `libEnhancementLibShared.so`: 5.72 MB → 5.66 MB (−62 KB на KWS-стек). Экспортов `vk::enh`: 306 → 281 (−25).
>
> Ниже — описание полного на 26.15.3 стека. См. [[530-version-26.16.0-diff]].
>
> ---


# На-устройство ASR / Keyword Spotting / Диаризация в `libEnhancementLibShared.so`

В предыдущем разборе (`09-native-libs.md`) `libEnhancementLibShared.so` (5.7 MB) описывалась как «VK-овский DSP для аудио в звонках». При вытаскивании символов через `nm -D` оказалось, что это куда больше, чем DSP.

Файлы исходного дерева, видные в строках:

```
/home/good/mainframer/webrtc4/src/modules/audio_processing/one-ann-audio-processing/...
  contrib/kaldi/src/feat/feature-mfcc.cc
  contrib/kaldi/src/feat/feature-window.cc
  contrib/kaldi/src/feat/mel-computations.cc
  contrib/kaldi/src/matrix/kaldi-vector.cc
  contrib/kaldi/src/matrix/srfft.cc
  contrib/kaldi/src/transform/cmvn.cc
  enh/modules/automatic_speech_recognition/asr_service.cpp
```

То есть это форк Google WebRTC (`webrtc4/`), в который вкомпилен модуль `one-ann-audio-processing` («ann» = artificial neural network) с подмодулями `automatic_speech_recognition`, `kaldi` и нейросетевой обработкой через TensorFlow Lite (с XNNPACK CPU-делегатом и поддержкой Android NNAPI).

## 1. ASR (Automatic Speech Recognition) на устройстве

Класс `vk::enh::AsrService`. Конструктор принимает путь:

```
vk::enh::AsrService::AsrService(std::string const&)
```

Внутри пайплайна (по строкам):

```
ASR pipeline version:
ASR process started
ASR service has not started
Failed to init ASR process:
ASR_METRIC:
asr_service
/asr/conformer_ctc_bpe_128
/asr/pipeline
```

`/asr/conformer_ctc_bpe_128` — имя модели: **Conformer + CTC + BPE-128**. Это ASR-архитектура, не KWS, не VAD. Distill-вариант, размер словаря — 128 BPE-токенов. Готова к рантайму на CPU через TFLite/XNNPACK; поддержка NNAPI присутствует (есть символы `ANeuralNetworksModel_*`).

API выставляется через C-обёртки:

```c
c_enh_asr_create
c_enh_asr_destroy
c_enh_asr_predict
c_enh_asr_predict_many
c_enh_asr_result_destroy
c_enh_asr_result_arr_destroy
```

VAD вокруг ASR: `_ZNK2vk3enh10AsrService12getVadEventsERKNS0_11AudioBufferE` — `AsrService::getVadEvents(AudioBuffer const&)`. Внутри ссылается на `/vad/vad_webrtc`.

Конструктор `AsrService(std::string)` принимает каталог моделей. Очевидное использование — расшифровка голосовых сообщений (PmsKey `audioTranscriptionEnabled` в `UserSettings`, см. `topics/11-state-bots-and-content-policy.md`).

## 2. KWS — Keyword Spotting (BCResNet)

`vk::enh::BCResNetKWS` и `vk::enh::BCResNetExternalStateKWS` — это **детектор ключевых слов** на нейросети BCResNet (broadcasted residual learning, Google 2021). `score(short const*)`, `extractFeatures()`, `computeProbs()`, `doScoreData()`, `reset()`.

`ExternalState`-вариант — это streaming-режим: состояние сети живёт между батчами, что нужно для непрерывной (real-time) детекции ключевых слов в потоке аудио без задержки на батч.

Регистрация через factory: `vk::enh::KWSFactory::createAlgorithm(Config)`. Конфигурация — из `vk::enh::Config` (см. ниже про DLL и формат конфига).

Что это значит: в `libEnhancementLibShared.so` встроен runtime, способный непрерывно детектировать произнесение конкретных слов, без отправки аудио на сервер. Для чего это используется в MAX:

- очевидное назначение — voice activation в звонках (поднять руку, mute/unmute и т. п.). Соответствующих UI-флагов в jadx-выгрузке я не нашёл.
- другое назначение — wake-word.
- технически тот же runtime годится и для триггерной детекции произвольных слов (модель — это тензорный файл, который можно подменить).

## 3. Диаризация (DiarizationEngine)

`vk::enh::DiarizationEngineFactory::createAlgorithm(Config)` плюс реестр алгоритмов:

```
vk::enh::Registry<std::string,
  std::shared_ptr<vk::enh::DiarizationEngine>(const vk::enh::Config &)>
```

Это разделение записи/потока на голоса (кто когда говорит). В мессенджере применимо к расшифровке групповых звонков и к голосовым сообщениям с несколькими говорящими.

## 4. AudioClassifier

```
c_enh_audio_classifier_create
c_enh_audio_classifier_predict
c_enh_audio_classifier_number_of_events_per_timestamp
```

Классификатор аудио событий. То есть вторая нейросеть, которая помечает таймстампы тегами событий («музыка», «голос», «шум»…). Для чего применяется — в коде явно не написано; типичные применения — pre-filter для ASR, либо отметка типа сообщения для UI.

## 5. Punctuation

`vk::enh::PunctFactory`, `c_enh_punct_predict`. Модель: `/punct/puc`. Восстановление пунктуации в транскрипте после ASR.

## 6. AsrService::collapseCompoundWords / fillPunct / calcConfidence

```
vk::enh::AsrService::calcConfidence(...)
vk::enh::AsrService::collapseCompoundWords(AlignResult const&)
vk::enh::AsrService::fillPunct(AlignResult const&, std::string const&)
```

То есть полный пост-процессинг: confidence-score, склеивание составных слов, заполнение пунктуации. Готовый продукт.

## 7. Aligner

```
c_enh_aligner_create
c_enh_aligner_align_normalized
c_enh_aligner_align_pcm
c_enh_align_result_destroy
```

Forced alignment — выравнивание текста на аудио-таймлайне (нужно для подсветки слов в транскрипте, для редактирования голосовухи).

## 8. Resampler / AudioBuffer / Pipeline

```
c_enh_resampler_create / c_enh_resampler_resample
c_enh_pipeline_create / c_enh_pipeline_run / c_enh_pipeline_accept_pcm
c_enh_audio_batch_*
```

`vk::enh::AudioBuffer` умеет читать из WAV-файлов, текстовых дампов floats/PCM, и от голого `float*`/`short*` буфера. То есть пайплайн умеет принимать сырой PCM, ресемплить, прогонять через DSP/ASR/KWS и отдавать наружу.

## 9. Динамическая подгрузка библиотек: `vk::enh::dll`

```
vk::enh::dll::Dll::Dll(char const*)
vk::enh::dll::Dll::open(char const*)
vk::enh::dll::Dll::findFunction(char const*)
vk::enh::dll::DllHandle::freeLibrary()
vk::enh::dll::openDll(char const*)
vk::enh::dll::findSymbol(void*, char const*)
vk::enh::dll::closeDll(void*)
vk::enh::dll::getLastError()
```

Это собственная обвязка над `dlopen`/`dlsym`/`dlclose` со своим именованием.

Скептический разбор:

- В самой `libEnhancementLibShared.so` я **не нашёл xref-ов** на `vk::enh::dll::Dll::open` через `rizin` (`afl ~+vk::enh::dll` показывает только определения функций, без вызовов изнутри). То есть эти функции экспортированы, но не вызываются из самой `.so`.
- В Java-стороне строки `EnhancementLibShared` или `vk::enh::dll` я тоже не нашёл (`grep -R EnhancementLibShared` пусто).
- Возможно, эти функции — наследие шаблонного скелета (windows-portable код, в котором `dll::*` на posix маппится в `dlopen`). Возможно, вызываются через C-обёртки в неэкспортированных функциях.
- Утверждать, что это «механизм подгрузки .so из сети» — нельзя без отдельного подтверждения. Текущий статус: «функции расшифровки и dlopen-обвязки в коде есть, но место их вызова в библиотеке мне не видно». Это не нулевой риск, но и не доказательство.

В сочетании с `vk::enh::decrypt(std::vector<unsigned char>)` (расшифровка байтового массива) — теоретически возможен сценарий «качаем зашифрованный артефакт, расшифровываем и грузим», но прямых доказательств такого сценария **в коде не нашёл**.

## 10. Encrypted models

```
vk::enh::decrypt(std::vector<unsigned char>)
```

Принимает вектор байт, возвращает что-то расшифрованное. Используется как минимум для хранения моделей: TFLite-модели и Kaldi-tables в APK либо лежат в зашифрованном виде, либо часть из них так пакуется. Файлов-кандидатов в `assets/` под `/asr/`, `/punct/`, `/vad/`, `/kws/` я в jadx-выгрузке не вижу — то есть модели либо подгружаются (через `dll`-обвязку?), либо хранятся в зашифрованных blob-ах в составе самого `.so`.

Чтобы их найти, нужно разобрать секции `.rodata` `libEnhancementLibShared.so` и поискать характерные TFLite-magic (`TFL3`) — это отдельная задача.

## 11. Где это используется в Java-стороне

Реально присутствующие в `findings/raw/pms_keys.txt` ключи, относящиеся к этому стеку:

```
audio-transcription-locales      — список локалей, для которых сервер разрешает транскрипцию
retry-transcribe-attempt         — параметры повторной попытки расшифровки
retry-transcribe-timeout
calls-android-ns                 — noise suppression в звонке (включается сервером)
calls-sdk-disable-pipeline       — выключить аудио-пайплайн SDK
calls-sdk-dnt-disable-audio      — «do-not-track» режим для аудио
calls-sdk-ai-opus-bwe            — AI-модель для bandwidth estimation в Opus
calls-sdk-log-audio              — серверно-включаемая запись аудио SDK (WIRETAP, см. FINDINGS)
calls-sdk-incall-stat
```

Поведение существенной части аудио-пайплайна — серверно-управляемое.

UserSettings-флаг `audioTranscriptionEnabled` (см. `topics/11-state-bots-and-content-policy.md`) приходит с сервера в одном объекте с `familyProtection`, `contentLevelAccess`, `safeMode` и т. д. Виден в `defpackage/sgj.java:123`, парсится из map в `defpackage/ma4.java:106-107`. Внутри настроек профиля его можно переключить, но факт наличия модели на устройстве и серверной возможности её включить — от тумблера не зависит.

Локализационные строки UI:
- `messages_settings_audio_transcription` (l5f.java)
- `setting_message_audio_transcription` (n0f.java)

## 12. Кратко

`libEnhancementLibShared.so` — **не «DSP для звонков»**, а отдельный embedded-runtime для нейросетевой обработки звука с конкретно перечисленным набором задач:

- ASR (Conformer-CTC-128) — распознавание речи на устройстве.
- VAD (`vad_webrtc`) — детекция голоса.
- KWS (BCResNet, streaming) — детекция ключевых слов в реальном времени.
- Diarization — разделение по говорящим.
- Audio classifier — теги аудио-событий.
- Punctuation predictor.
- Forced aligner для подсветки слов в транскрипте.
- TFLite + XNNPACK + опциональный NNAPI делегат.
- Свой `dlopen`-обёрточный слой и функция `decrypt(...)` для зашифрованных артефактов.

Из «странностей»:

- В мессенджере встроен KWS-runtime, способный к streaming-детекции слов на сыром аудио. Какая модель туда заряжается из коробки — нужно отдельно вытаскивать из `.rodata`.
- `vk::enh::dll::Dll::open(char const*)` + `vk::enh::decrypt(...)` — вместе они образуют достаточный набор для подгрузки внешнего .so (или модели) в рантайме, после расшифровки.
- Все ключевые поведенческие тумблеры (`calls-enh-asr`, `audio-transcription-enabled`, `calls-vad-enabled`, `calls-enh-aligner`) — серверно-управляемые PmsKey.

Файлы:

- `findings/native/libEnhancementLibShared.exports.txt`
- `findings/native/libEnhancementLibShared.strings.txt`
