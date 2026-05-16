---
tags: [camera, microphone, screen-capture, mediaprojection, surveillance, attack-surface]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/calls/ui/ui/call/CallScreen.java
  - work/jadx_base/sources/one/me/calls/impl/service/a.java
  - work/jadx_base/sources/one/me/sdk/messagewrite/recordcontrols/delegates/VideoMessageRecordDelegate*.java
  - work/jadx_base/sources/defpackage/kd0.java
  - work/apktool_base/AndroidManifest.xml
related:
  - "[[18-manifest-deep-dive]]"
  - "[[09-native-libs]]"
  - "[[15-on-device-asr-kws-diarization]]"
  - "[[16-server-pushed-ml-models-in-calls]]"
---

# Точки запуска камеры, микрофона, записи экрана

Сводная карта мест в коде, откуда стартуют сенсоры захвата (камера, микрофон) и MediaProjection (запись экрана). Цель — увидеть полный attack surface на «когда вообще приложение может начать писать».

## 1. Запись экрана (MediaProjection)

`one.me.calls.ui.ui.call.CallScreen` — внутри экрана звонка получает `MediaProjectionManager` через `getSystemService` и вызывает `startActivityForResult(mediaProjectionManager.createScreenCaptureIntent(), 1)`. То есть **запись экрана инициируется только из звонка**; за пределами звонкового UI вход в MediaProjection из jadx-кода я не вижу.

После согласия пользователя (стандартный системный диалог Android) поток MediaProjection отдаётся в `ru.ok.tamtam.android.calls.MediaProjectionService` (foregroundServiceType=`mediaProjection`, см. [[18-manifest-deep-dive]]). Этот сервис реализует часть «расшарить экран в звонке».

Скептический разбор:
- Такой сценарий — нормальная UX-фича (screen sharing в Zoom/Discord/etc).
- Триггер требует runtime-permission через системный prompt; backend не может это начать без явного диалога.
- НО: `foregroundServiceType` у `CallServiceImpl` включает `mediaProjection` параллельно с `microphone` и `camera` (см. [[18-manifest-deep-dive]]). То есть инфраструктура одного foreground-сервиса может одновременно держать камеру, микрофон и экран. Один и тот же `Service`-узел содержит права на всё.

## 2. Микрофон в звонке

WebRTC. `libjingle_peerconnection_so.so` (см. [[09-native-libs]]) использует `AudioDeviceModule` Android-WebRTC — внутри он вызывает `AudioRecord(MediaRecorder.AudioSource.VOICE_COMMUNICATION, …)` через нативную часть. Java-сторона напрямую `new AudioRecord(...)` не вызывает; всё уходит в `.so`. `RECORD_AUDIO` permission — обязателен.

PmsKey, влияющие на этот пайплайн:
- `calls-android-ns` (noise suppression),
- `calls-sdk-disable-pipeline` (выключить SDK pipeline),
- `calls-sdk-dnt-disable-audio` (DNT-режим: не передавать аудио),
- `calls-sdk-log-audio` («WIRETAP» в FINDINGS — серверно-включаемая запись аудио SDK; см. [[03-pms-server-flags]]).

То есть само отдание звука в WebRTC находится за серверным флагом `calls-sdk-disable-pipeline`. Сервер технически может выключить локальную обработку аудио и оставить чистый pass-through.

## 3. Микрофон в голосовых сообщениях

`defpackage/kd0.java` — реализует `MediaRecorder.OnInfoListener` и держит `MediaRecorder` как поле. Это запись голосовых сообщений (так называемые «голосовухи» в чатах). Стандартный путь Android `MediaRecorder` с AAC/Opus.

Альтернативный путь — `OpusRecorder` (PmsKey `opus-recorder`, `opus-recorder-bitrate`, `opus-recorder-sample-rate`). Серверно-управляемое включение нативного Opus-енкодера вместо системного `MediaRecorder`.

Триггер записи — UI-жест «зажать кнопку микрофона» в чате. Без этого `kd0` не запускается. То есть подделать запуск этого без явного действия пользователя — нельзя без дополнительной серверной/UI-уязвимости.

## 4. Камера в видеосообщениях («кружки»)

`one/me/sdk/messagewrite/recordcontrols/delegates/VideoMessageRecordDelegate.java` — делегат записи видеосообщения. Исключения, перечисленные в jadx (`NoAvailableCameraException`, `PreviewRenderException`), показывают, что используется CameraX (`androidx.camera.*`) с превью на TextureView/SurfaceView и кодеком через `MediaCodec`.

Триггер — UI-жест зажать кнопку микрофона + переключить на видео в окне записи (типичный паттерн «кружков»).

## 5. Камера в звонке

`one/me/calls/impl/service/a.java` (utility-класс `CallServiceImpl`) и `org/webrtc/CameraVideoCapturer.java` — webrtc-овский video capture для звонка. Стандартный путь.

PmsKey `camera-freeze-detector-timeout` — клиент дополнительно следит, не «застряла» ли камера, и репортит таймаут в стат.

## 6. Микрофон через KWS (keyword spotting)

В коде `ru/ok/android/externcalls/sdk/audio/KeywordSpotterManagerImpl` (см. [[15-on-device-asr-kws-diarization]] и [[16-server-pushed-ml-models-in-calls]]) есть точка, где аудио из WebRTC pipeline отправляется в TFLite-модель KWS. Это **в рамках уже идущего звонка** — поток уже захвачен, KWS получает копию.

Самостоятельного запуска микрофона KWS-стек не делает. Но как часть звонка он живёт параллельно с обработкой голоса.

## 7. Сводка по триггерам

| Канал | Откуда стартует | Требует UI? | Серверный gate |
|---|---|---|---|
| MediaProjection (запись экрана) | `CallScreen` → системный prompt | Да (диалог Android) | Только в рамках звонка |
| Mic в звонке | WebRTC через `CallServiceImpl` | Старт звонка | `calls-sdk-disable-pipeline`, `calls-sdk-log-audio`, `calls-sdk-dnt-disable-audio` |
| Mic в голосухе | `kd0` / `OpusRecorder` | Зажать кнопку | `opus-recorder` |
| Mic в KWS | поток из звонка → TFLite | Звонок активен | модель скачивается с сервера, см. [[16-server-pushed-ml-models-in-calls]] |
| Камера в звонке | `CallServiceImpl` + `CameraVideoCapturer` | Старт звонка с видео | `calls-android-vtv2`, `calls-android-h265-s` |
| Камера в кружках | `VideoMessageRecordDelegate` | Жест записи | `video-msg-config` |

## 8. Что важно

- Все шесть каналов требуют либо runtime-permission, либо UI-действие. **Скрытого фонового запуска микрофона/камеры/экрана из jadx-кода я не нашёл.**
- Foreground-service `CallServiceImpl` имеет права на все три (`camera | microphone | mediaProjection | mediaPlayback | phoneCall`) одновременно. То есть в момент звонка один service удерживает все нужные капабилити.
- KWS работает только во время звонка, без отдельного «вне-звонка» прослушивания (серверной команды поднять микрофон без звонка я не вижу).
- Серверный gate `calls-sdk-log-audio` (см. FINDINGS «WIRETAP») потенциально включает запись аудио в SDK-логи — это уже происходит **во время** звонка, но создаёт side-channel «копия аудио в логе».

Скептический итог: «зерокликом» поднять микрофон/камеру в этой сборке нельзя без отдельной уязвимости. Backdoor-индикатор: один foreground-service объединяет камеру+микрофон+screenshare+phonecall, и серверные флаги имеют тонкое управление аудио-пайплайном (включить лог, отключить DNT, серверно вырубить локальную обработку).
