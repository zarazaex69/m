---
tags: [critical, wiretap, audio-dump, apptracer, signaling, production, surveillance]
status: verified
severity: critical
sources:
  - smali_classes3/ru/ok/android/externcalls/sdk/dev/internal/MediaDumpManagerImpl.smali
  - smali_classes3/ru/ok/android/externcalls/sdk/dev/MediaDumpManager$Source.smali
  - smali_classes3/org/webrtc/PeerConnectionFactory.smali
  - smali_classes3/org/webrtc/NativeDumpCallback.smali
  - smali_classes3/y91.smali (recordAudioDump executor)
  - smali_classes2/l0.smali (enableFullAudioDump executor)
related:
  - "[[196-debug-media-dump-manager]]"
  - "[[401-media-dump-manager]]"
  - "[[402-debug-manager]]"
  - "[[322-apptracer-sample-upload]]"
  - "[[17-apptracer-uplink]]"
  - "[[153-signaling-commands]]"
  - "[[532-speaker-recognition-profanity]]"
---

# 531. Wiretap Chain: collect-debug-dump → Audio Dump → Apptracer Upload

## Суть

Полностью рабочая цепочка в **production-сборке** (`calls-sdk_release` модуль), позволяющая серверу MAX **молча записать обе стороны телефонного разговора и выгрузить запись** на `sdk-api.apptracer.ru` — без уведомления пользователя.

## Цепочка (верифицирована по smali)

```
СЕРВЕР (signaling WS)
    │
    │  JSON: {type: "collect-debug-dump", audio: true, video: true, duration: N}
    ▼
MediaDumpManagerImpl.requestMediaDump(duration, audio, video, listener)
    │
    │  Формирует JSONObject {audio, video, duration}
    │  Отправляет через SignalingProvider → pbh.h(command, callback)
    │  Callback: ka1 → requestMediaDump$lambda$0 → onRequestSent()
    ▼
PeerConnectionFactory.nativeSubmitDumpRequest(nativePtr, path, durationMs, sources[], callback)
    │
    │  Нативный WebRTC записывает аудио из 6 точек pipeline
    │  Файл: calldump_<yyyy-MM-dd HH:mm:ss>_<duration>s
    ▼
DumpCallback.onComplete(dumpFolderPath)
    │
    │  → LocalAudioDumpRecordListener.onRecordCompleted(path)
    ▼
ShrinkDumpWorker (WorkManager)
    │
    │  → xl2.v(context, config, file, name, size, properties, flags)
    │  Sets: tracer_sample_file_path, tracer_sample_file_size, tracer_sample_file_name
    ▼
SampleUploadWorker (WorkManager)
    │
    │  POST https://sdk-api.apptracer.ru/api/sample/initUpload?sampleToken=<token>
    │  POST https://sdk-api.apptracer.ru/api/sample/upload
    ▼
СЕРВЕР APPTRACER (VK/OK инфраструктура)
```

## 6 точек перехвата аудио (MediaDumpManager$Source enum)

| Source | Описание |
|--------|----------|
| `IN_ENTER_PROCESSING` | Сырой микрофон ДО любой обработки |
| `IN_AFTER_NS` | После шумоподавления |
| `IN_AFTER_ANIMOJI` | После animoji-обработки |
| `IN_EXIT_PROCESSING` | Финальный исходящий аудио-поток |
| `OUT_ENTER_PROCESSING` | Входящий аудио собеседника (сырой) |
| `OUT_EXIT_PROCESSING` | Финальный входящий аудио собеседника |

## Ключевые факты

1. **Нет UI-подтверждения** — в коде `requestMediaDump` нет вызова Activity/Dialog/Toast/Notification
2. **Нет звукового сигнала** — нет `call_record_start.m4a` в этом пути (это для штатной записи)
3. **Нет индикатора** — foreground service уже активен (звонок), дополнительного не создаётся
4. **Записывает ОБЕ стороны** — `IN_*` (микрофон) + `OUT_*` (собеседник)
5. **Длительность задаётся сервером** — параметр `duration` в JSON, `0x7fffffff` (max int) для enableFullAudioDump
6. **Загрузка автоматическая** — через WorkManager, переживает перезапуск приложения
7. **Production модуль** — `calls-sdk_release`, не debug-вариант

## Smali-доказательства

### requestMediaDump отправляет команду серверу (строка 319):
```smali
const-string p1, "collect-debug-dump"
invoke-static {v1, p1}, Livl;->b(Lorg/json/JSONObject;Ljava/lang/String;)Ldp7;
move-result-object p1
invoke-virtual {v0, p1, p2}, Lpbh;->h(Ldp7;Lmbh;)V
```

### Сервер отвечает — callback подтверждает (строка 111):
```smali
const-string v0, "collect-debug-dump"
invoke-virtual {v0, p1}, Ljava/lang/String;->equals(Ljava/lang/Object;)Z
move-result p1
if-eqz p1, :cond_0
invoke-interface {p0}, Lru/ok/android/externcalls/sdk/dev/MediaDumpManager$RemoteMediaDumpRequestListener;->onRequestSent()V
```

### enableFullAudioDump — бесконечная запись (l0.smali:1640):
```smali
const v5, 0x7fffffff    # duration = Integer.MAX_VALUE
const/4 v6, 0x0         # callback = null
invoke-virtual {v0, v4, v5, v6}, Lorg/webrtc/PeerConnectionFactory;->submitDumpRequest(...)V
```

## Связь с Apptracer upload

`SampleUploadWorker` (из [[322-apptracer-sample-upload]]) принимает **любой файл** с произвольными метаданными. Heap dumps идут тем же каналом. Audio dumps — тем же каналом. Серверный конфиг: PmsKey `perf-events`, `perf-registrar-config`.

## Статус в 26.16.0

**Без изменений.** `MediaDumpManagerImpl`, `DebugManagerImpl`, `PeerConnectionFactory.nativeSubmitDumpRequest` — всё на месте. Удалён только KWS (keyword spotter).

## Импликации

Это **готовый инструмент таргетированного прослушивания**:
- Сервер знает, что пользователь в звонке (WS `NOTIF_CALL_START`)
- Сервер знает, с кем (`participantId`)
- Сервер может молча записать обе стороны
- Запись автоматически загружается на инфраструктуру VK/OK
- Пользователь не получает никакого уведомления

Единственное ограничение: работает только **во время активного звонка** (нужен активный `PeerConnectionFactory`). Вне звонка микрофон этим путём не активируется.
