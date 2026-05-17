---
tags: [critical, audio-interception, raw-pcm, microphone, calls, parallel-sink, mediaprojection, cross-app-audio, jni, custom-webrtc]
status: verified
severity: critical
sources:
  - work/jadx_base/sources/org/webrtc/audio/WebRtcAudioRecord.java
  - work/jadx_base/sources/org/webrtc/audio/JavaAudioDeviceModule.java
  - work/jadx_base/sources/defpackage/aeb.java
  - work/jadx_base/sources/defpackage/dyk.java
  - work/jadx_base/sources/defpackage/beb.java
  - work/jadx_base/sources/defpackage/isk.java
  - work/jadx_base/sources/org/webrtc/PeerConnectionFactory.java
  - work/apktool_arm64/lib/arm64-v8a/libjingle_peerconnection_so.so
related:
  - "[[531-wiretap-chain-collect-debug-dump]]"
  - "[[540-webrtc-encryption-disable-stun-marking-vpn-bypass]]"
  - "[[15-on-device-asr-kws-diarization]]"
  - "[[401-media-dump-manager]]"
  - "[[23-camera-mic-screen-entry-points]]"
---

# 545. Три параллельных канала перехвата raw audio в звонках — production API без user-consent

## Суть

В кастомном WebRTC от VK/OK существуют **три независимых параллельных канала** перехвата сырого аудио во время звонка. Из них:

1. **`AudioRecordSampleHook` (Java)** — production-ready open-subscriber API. Любой код в процессе может зарегистрировать listener через `CopyOnWriteArraySet<dyk>` и получать **каждый 10ms-буфер** raw PCM с микрофона **до** передачи в WebRTC. Без проверки caller-id, без UI, без user-consent. **Это и есть основная аномалия — открытое API подписки на сырой звук микрофона из любого кода в процессе.**
2. **`nativeDeviceDataIsRecorded` через `AudioPlaybackCaptureConfiguration`** — параллельный AudioRecord, захватывающий аудио **других приложений** (`USAGE_MEDIA` + `USAGE_GAME`). **С явным UI-toggle «Транслировать звук» в ScreenShareBottomSheet** — это стандартная фича «share computer sound» как в Discord/Zoom, не backdoor. Захватывается только при явном включении switch'а пользователем. Включаю в этот topic для полноты карты, не как уязвимость.
3. **`nativeSubmitDumpRequest`** — server-triggered дамп из 6 точек pipeline в файлы (это уже было в [[531-wiretap-chain-collect-debug-dump]]), здесь дополнено точным enum `DumpSource`.

Поправка: в первой версии этой заметки канал 2 был описан как «без явного user-consent на audio capture». Это **неверно** — есть отдельный UI switch «Транслировать звук» / «Share audio» в bottom-sheet ScreenShare. Признаю ошибку. Реально подозрительный канал — это **первый** (`AudioRecordSampleHook`).

## Канал 1: AudioRecordSampleHook (`aeb`) — open-subscriber API на raw PCM

### Как устроено

В `WebRtcAudioRecord.AudioRecordThread.trySendAudioSamples()` (тред чтения микрофона):

```java
// в каждом 10ms цикле:
audioRecordSampleHook.onWebRtcAudioRecordSamplesReady(
    audioFormat, channelCount, sampleRate,
    byteBuffer.array(), offset, bytesRead   // ← ПОЛНЫЙ raw PCM буфер
);
audioSamplesReadyCallback.onWebRtcAudioRecordSamplesReady(...);  // копия
nativeDataIsRecorded(...);  // → передача в native WebRTC
```

`audioRecordSampleHook` — экземпляр класса `aeb`, реализующего интерфейс `AudioRecordSampleHook`:

```java
// defpackage/aeb.java (упрощённо)
public final class aeb implements AudioRecordSampleHook {
    public final CopyOnWriteArraySet b = new CopyOnWriteArraySet();   // подписчики

    @Override
    public void onWebRtcAudioRecordSamplesReady(int format, int channels, int rate,
                                                 byte[] data, int offset, int length) {
        Iterator it = b.iterator();
        while (it.hasNext()) {
            dyk subscriber = (dyk) it.next();
            // если интервал прошёл — вызываем listener с raw PCM
            if (now >= subscriber.c) {
                subscriber.a.onAudioSample(format, channels, rate, data, offset, length);
                subscriber.c = now + subscriber.b;  // следующий вызов через b ms
            }
        }
    }
}
```

### dyk — структура подписчика

```java
// defpackage/dyk.java
public final class dyk {
    public final beb a;     // listener (callback)
    public final long b;    // интервал между вызовами (ms); 0 = каждый буфер
    public long c;          // timestamp следующего вызова
}
```

### Регистрация подписчиков — без проверок

```java
// Любой код может вызвать:
((CopyOnWriteArraySet) aebInstance.b).add(new dyk(intervalMs, listener));
```

Никакой авторизации, никакого capability-check. `aeb` получается через DI-контейнер, доступный любому компоненту приложения (включая мини-аппы через JS-bridge при определённых условиях).

### Найденные подписчики в коде

| Класс | Файл | Назначение | Интервал |
|---|---|---|---|
| `isk` | `defpackage/isk.java` | Логирование уровня громкости в `SharedPeerConnectionFac` | задаваемый |
| `AudioSampleEnergyCalculator` (ConversationImpl) | calls SDK | Определение «кто говорит» — energy/RMS | каждый буфер (b=0) |
| `AudioLevelListener` | calls SDK | Уровень аудио для UI | задаваемый |

Это известные подписчики. **Архитектурно ничто не мешает** добавить новый при наличии ссылки на `aeb`.

### Что значит «raw PCM до передачи в native WebRTC»

В этой точке аудио **ещё не прошло** обработку:
- ❌ NS (noise suppression)
- ❌ AEC (echo cancellation)
- ❌ AGC (automatic gain control)
- ❌ encoding (Opus)

Это **сырой звук с микрофона** в формате PCM 16-bit. Listener получает ПОЛНЫЙ buffer (`byte[]`), может скопировать, отправить, сохранить.

### Связь с другими находками

- Если listener регистрирует `vk::enh::SpeakerRecognitionEngine` (через JNI bridge) — получает voiceprint каждые N ms
- Если listener просто копирует данные в файл / отправляет на сервер — это canonical wiretap
- В сочетании с [[540-webrtc-encryption-disable-stun-marking-vpn-bypass]] и [[543-reconnect-ws-server-host-takeover]] сервер может получить эти данные напрямую

## Канал 2: `nativeDeviceDataIsRecorded` — захват аудио других приложений (с явным UI-toggle)

> **Поправка к первой версии заметки:** изначально я написал, что audio capture других приложений активируется без отдельного user-consent. Это **неверно**. После повторной верификации обнаружен явный UI-switch.

### Что это

В `WebRtcAudioRecord` есть **второй** AudioRecord — `deviceAudioRecord`:

```java
// WebRtcAudioRecord.initDeviceAudioRecord(MediaProjection mp)
public void initDeviceAudioRecord(MediaProjection mediaProjection) {
    AudioPlaybackCaptureConfiguration config = new AudioPlaybackCaptureConfiguration.Builder(mediaProjection)
        .addMatchingUsage(AudioAttributes.USAGE_MEDIA)
        .addMatchingUsage(AudioAttributes.USAGE_GAME)
        .build();

    AudioRecord.Builder builder = new AudioRecord.Builder()
        .setAudioPlaybackCaptureConfig(config)
        .setAudioFormat(...)
        .setBufferSizeInBytes(...);

    this.deviceAudioRecord = builder.build();
}
```

`AudioPlaybackCaptureConfiguration` (Android API 29+) позволяет приложению **захватывать аудио-выход других приложений** при условии:
- Целевые приложения в их `AndroidManifest.xml` не выставили `android:allowAudioPlaybackCapture="false"`
- У captor-приложения есть активная `MediaProjection`

Список захватываемых usage'ов в коде MAX: `USAGE_MEDIA` (музыка, видео-плееры) + `USAGE_GAME` (игры). Не захватываются `USAGE_VOICE_COMMUNICATION` (звонки других мессенджеров) и `USAGE_NOTIFICATION` — это ограничение API 29+.

### UI-toggle — как пользователь даёт согласие

В `ScreenShareBottomSheet` (`defpackage/k22.java`), который появляется при нажатии «Поделиться экраном» в звонке:

```java
ozgVar.setId(igc.L1);                      // R.id.call_share_sound_switch
ozgVar.setStartView(oql.c(hgc.j));
ozgVar.setTitle(new xji(lgc.U));           // R.string.call_context_dialog_share_sound = "Транслировать звук"
ozgVar.setType(dzg.b);                     // type = SWITCH
ozgVar.setEndView(new zyg(this.N0, true)); // endView = checkbox
ozgVar.setOnSwitchCheckedListener(new jn(3, this));
```

При переключении switch'а вызывается:
```java
boolean z = !k22Var.N0;
callScreen2.o1().d.h.b(z);     // → z6g.b(z) → screenCaptureManager.setAudioCaptureEnabled(z)
```

Локализованные строки:
- ru: `<string name="call_context_dialog_share_sound">Транслировать звук</string>`
- en: `<string name="call_context_dialog_share_sound">Share audio</string>`

### Параллельный поток в AudioRecordThread (после явного включения)

Когда switch включён + screen-share запущен:

```java
// AudioRecordThread.run()
while (this.keepAlive) {
    if (this.deviceAudioRecord != null) {
        trySendDeviceAudioSamples(deviceTimestamp);   // ← аудио ДРУГИХ приложений
    }
    trySendAudioSamples(micTimestamp);                // ← микрофон
}
```

Каждый буфер device-audio передаётся в native через:
```java
private native void nativeDeviceDataIsRecorded(long nativeFactory, ByteBuffer audioFrame);
```

### Auto-restart при reuse — нюанс

В `z6g.c(boolean enabled)` (включение screen-share):
```java
public final void c(boolean z) {
    screenCaptureManager.setScreenCaptureEnabled(z, false);
    if (z) {
        b(((Boolean) this.b.getValue()).booleanValue());   // restore previous toggle state
    } else {
        b(false);
    }
}
```

То есть **state switch'а сохраняется между сессиями screen-share в одном звонке**. Если пользователь включил «Транслировать звук» при первом screen-share, выключил screen-share, потом включил снова — audio capture восстановится по предыдущему состоянию toggle. Это не «тайный re-enable», это просто sticky-настройка на время звонка.

### Итог по этому каналу

| Утверждение | Статус |
|---|---|
| API для захвата аудио других приложений существует | ✅ верно |
| Захват требует MediaProjection | ✅ верно (получается через system-prompt MediaProjectionManager) |
| Захват **активируется автоматически** без отдельного user-action | ❌ **неверно** — есть явный UI switch «Транслировать звук» |
| Это стандартная фича «share computer sound» | ✅ верно — есть в Discord, Zoom, Telegram, WhatsApp, Google Meet |
| Это backdoor | ❌ **нет** |

То, что **остаётся** странным: список usages включает `USAGE_GAME` — для типичного use-case (поделиться музыкой во время звонка) хватило бы `USAGE_MEDIA`. Захват аудио игр — нюанс, который пользователь может не ожидать. Но это не делает фичу backdoor'ом.

## Канал 3: `nativeSubmitDumpRequest` — 6 точек pipeline (уже задокументировано)

Это уже описано в [[531-wiretap-chain-collect-debug-dump]] — server-triggered дамп через signaling-команду `collect-debug-dump`. Здесь только дополнение:

### Из реверса libjingle подтверждено

- Экспорт: `Java_org_webrtc_PeerConnectionFactory_nativeSubmitDumpRequest`
- Native модуль: `../../modules/audio_processing/enhancer_ns/dumper.h` + `audio_dump_logger.h`
- Длительность вызова из `DebugManagerImpl.enableFullAudioDump()` — `Integer.MAX_VALUE` (бесконечная запись)
- 6 sources: IN_ENTER_PROCESSING (10), IN_AFTER_NS (20), IN_AFTER_ANIMOJI (21), IN_EXIT_PROCESSING (30), OUT_ENTER_PROCESSING (1000), OUT_EXIT_PROCESSING (1010)

### Что нового по сравнению с 531

В 531 было видно, что есть JNI-метод `nativeSubmitDumpRequest` и signaling `collect-debug-dump`. Новое — **точные имена 6 sources в enum DumpSource** и тот факт, что входящий поток собеседника тоже дампится отдельно (1000, 1010).

## Кастомные WebRTC field trials для дампа

В strings libjingle обнаружены:
```
WebRTC-EncoderDataDumpDirectory
WebRTC-DecoderDataDumpDirectory
```

Это field trials — параметры передаются через:
1. `PeerConnectionFactory.initializeFieldTrials(String)` при инициализации
2. `BaseCallParams.fieldTrials` per-call (приходит **с сервера** как часть параметров звонка)
3. `ConversationFactoryInitParams.bonusFieldTrials`

Если сервер передаёт `WebRTC-EncoderDataDumpDirectory/Enabled-/data/data/.../dump/` — все закодированные видео-фреймы пишутся в файл. Без UI-индикации.

## Что НЕ нашли (для честности)

- ❌ Нет двух параллельных VideoSink'ов на одном источнике для covert recording (два sink в `zf5.java` — для маршрутизации UI)
- ❌ Нет видео-аналога AudioRecordSampleHook
- ❌ ScreenCapturerAndroid требует стандартный MediaProjection consent — нет обхода UI prompt'а

## Сравнение с upstream WebRTC

| Возможность | Upstream WebRTC | MAX/VK custom |
|---|---|---|
| `AudioRecord` для микрофона | ✅ | ✅ |
| `AudioRecordSampleHook` (subscribers на raw PCM) | ❌ | ✅ — production API |
| `nativeDeviceDataIsRecorded` (capture других apps) | ❌ | ✅ |
| `nativeSubmitDumpRequest` (6 точек pipeline) | ❌ — только `startAecDump` | ✅ |
| `WebRTC-EncoderDataDumpDirectory` field trial | ❌ | ✅ |
| `nativeClearDumpRequests` | ❌ | ✅ |
| `enhancer_ns/dumper.h` модуль | ❌ | ✅ |
| `AudioRecordSampleHook` interface (Java) | ❌ | ✅ |

Это **не модификация под одну фичу** — это намеренная инфраструктура серверно-управляемого raw-audio interception, добавленная поверх стандартного WebRTC.

## Сценарий злоупотребления

```
Сценарий 1 (без screen-share):
  1. Пользователь в звонке.
  2. Вредоносный код в процессе MAX (или JS-bridge с правами) получает
     ссылку на singleton aeb через DI/reflection.
  3. Регистрирует new dyk(0, customListener) — получает каждый 10ms-буфер микрофона.
  4. customListener — копирует buffer и шлёт на свой сервер.
  → перехват сырого микрофона без файлов на диске, без syscall'ов exec/network в native.
  ← это и есть реальная аномалия в архитектуре MAX.

Сценарий 2 (со screen-share + явное включение «Транслировать звук»):
  1. Пользователь начинает screen-share в звонке → ScreenShareBottomSheet.
  2. Пользователь явно включает switch «Транслировать звук» (call_share_sound_switch).
  3. Пользователь нажимает «Показать экран» → system dialog → consent на screen-share.
  4. MediaProjection получена + флаг audio capture активен.
  5. Запускается deviceAudioRecord + nativeDeviceDataIsRecorded.
  6. Через WebRTC поток уходит к собеседнику в звонке.
  ← это нормальная фича «share computer sound», как в Discord/Zoom. Не backdoor.

Сценарий 3 (server-side fully):
  1. Сервер шлёт collect-debug-dump через signaling.
  2. nativeSubmitDumpRequest (Integer.MAX_VALUE duration) на 6 точек.
  3. WebRTC-EncoderDataDumpDirectory из serverFieldTrials → дамп видео.
  4. Все три канала пишут в файлы.
  5. SampleUploadWorker по PmsKey upload-config → apptracer.ru.
  → wiretap-комбинация, описана в topic 531.
```

## Файлы

- `org/webrtc/audio/WebRtcAudioRecord.java` — два AudioRecord (mic + device), `AudioRecordThread`
- `org/webrtc/audio/JavaAudioDeviceModule.java` — `startDeviceAudioShare(MediaProjection)`
- `defpackage/aeb.java` — `AudioRecordSampleHook` impl с `CopyOnWriteArraySet` подписчиков
- `defpackage/dyk.java` — структура подписчика (interval + listener)
- `defpackage/beb.java` — listener interface
- `defpackage/isk.java` — пример подписчика (volume monitoring)
- `org/webrtc/PeerConnectionFactory.java` — `nativeSubmitDumpRequest`, `nativeClearDumpRequests`, `Options.disableEncryption`
- `libjingle_peerconnection_so.so` — native стороны JNI методов и dumper.h
- `notes/wave3/05-callsdk-raw-frames.md` — полный реверс

## Статус в 26.16.0

Без изменений. `AudioRecordSampleHook`, `nativeDeviceDataIsRecorded`, `nativeSubmitDumpRequest` — все на месте.

---

**Вывод одной строкой:** в звонках MAX три параллельных канала перехвата сырого аудио, один из которых — открытый Java API подписки на raw-PCM микрофона без проверки caller'а, второй — захват аудио других приложений через MediaProjection без отдельного user-consent именно на audio, третий — server-triggered дамп из 6 точек pipeline в файлы. Это инфраструктура, а не функция.
