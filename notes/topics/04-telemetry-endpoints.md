---
tags: [telemetry, network, endpoints, third-party, surveillance]
status: confirmed
sources:
  - findings/raw/urls_quoted_jadx.txt
  - findings/raw/hosts_quoted_jadx.txt
  - work/apktool_base/AndroidManifest.xml
related:
  - "[[01-mobile-id-cleartext]]"
  - "[[09-native-libs]]"
  - "[[12-force-update-killswitch]]"
---

# Куда MAX шлёт телеметрию

## Внешние трекеры

### 1. Apptracer (OK.ru tracer)

Эндпоинт: `https://sdk-api.apptracer.ru`.

Что отправляется:
- `defpackage/yxb.java` строит URL вида `https://sdk-api.apptracer.ru/{path}?crashToken=...` с типами `CRASH`, `MINIDUMP` (нативные минидампы), `ANR`, и др.
- `defpackage/ktg.java`: `https://sdk-api.apptracer.ru/api/crash/trackSession?crashToken=...` — POST JSON. Это per-session трекер. То есть отдельный «жив ли процесс, как стартанул, какие параметры окружения, есть ли ANR/краши» поток.
- `ru.ok.tracer.upload.SampleUploadWorker` (из `findings/raw/`) и набор инициализаторов, объявленных прямо в манифесте: `NativeBridgeInitializer`, `CrashReportInitializer`, `PerformanceMetricsInitializer`, `DiskUsageInitializer`, `HeapDumpInitializer`, `LoggerInitializer`, `TracerInitializer`.

То есть в MAX встроен полный SDK Apptracer — крэши, нативные минидампы, периодические дампы кучи (heap dumps), метрики производительности, дисковое потребление — всё уезжает на `sdk-api.apptracer.ru` (это сервис OK.ru, общий по группе VK/OK/MAX).

`HeapDumpInitializer` особенно жирный — heap dump приложения (то есть содержимое памяти процесса, в котором лежат приватные сообщения, токены, контакты) при определённых условиях может уехать на сервер. Включается серверно (через PMS, см. `tracer-non-fatal-crashed-enabled`, `perf-events`).

### 2. MyTracker (VK)

Эндпоинт: `https://tracker-api.vk-analytics.ru/?` (видно в `com/my/tracker/core/handlers/AttributionHandler.smali` и `com/my/tracker/core/TrackerConfig.smali`).

MyTracker — это VK-овский SDK для:
- attribution (откуда поставился аппликейшн, install referrer);
- AdId сбора;
- session/event-аналитики.

В манифесте включено `com.google.android.finsky.permission.BIND_GET_INSTALL_REFERRER_SERVICE` — это для install attribution (Play Install Referrer), и `AD_ID` permission (`com.google.android.gms.permission.AD_ID`).

Включается серверно: `mytracker-enabled` (`isMytrackerEnabled`), уровень: `mytracker-log-level`. То есть и тут сервер MAX **в любой момент** включает/выключает поток к VK-аналитике.

### 3. OneLog (OK.ru)

Класс `ru.ok.android.onelog.UploadService` и `OneLog`/`OneLogImpl`/`Worker` (всё в пакете `ru/ok/android/onelog/`).

OneLog — это **внутренний** аналитический канал OK.ru: события, идущие в общий аналитический бэкенд группы. Endpoint строится динамически (через коллектор), сами события буферизуются в файлах в локальном кэше (`MAX_FILE_LENGTH = 10_000_000` байт — то есть 10 MB лога локально), затем `UploadService.startUpload(...)` отправляет.

Манифест публикует **два** OneLog-uploader-а:
```xml
<service exported="false" name="ru.ok.android.externcalls.analytics.internal.upload.UploadService" permission="android.permission.BIND_JOB_SERVICE">
    <intent-filter><action android:name="ru.ok.android.onelog.action.UPLOAD"/></intent-filter>
</service>
<service exported="false" name="ru.ok.android.onelog.UploadService" permission="android.permission.BIND_JOB_SERVICE">
    <intent-filter><action android:name="ru.ok.android.onelog.action.UPLOAD"/></intent-filter>
</service>
```

Один из них — для общего OneLog, второй — отдельный для **аналитики звонков** (`ru.ok.android.externcalls.analytics`). У звонков, таким образом, отдельная пайплайна.

### 4. Собственный backend MAX

Бизнес-API (тут идут чаты, медиа, регистрация и т.п.):
- `api.oneme.ru` — production
- `api-test.oneme.ru` — test
- `api-test2.oneme.ru` — второй test
- **`api-tg.oneme.ru`** — отдельный «tg» сервер (название `tg`, скорее всего, от внутреннего «test/group/tg=test gateway»; уверенности нет, но он стоит наравне с production)

Хосты не зафиксированы намертво — в `defpackage/r58.java` они хранятся в `SharedPreferences("dev_tools")` и пользователь через скрытое dev-меню может в release-билде переключать API между ними и даже задать «Custom». Это значит: **в production-сборке MAX можно средствами самого приложения переключиться на test-сервер**. (См. `notes/topics/05-dev-menu-in-prod.md`.)

### 5. Calls (звонки) — пара отдельных каналов

- `https://max.ru/joincall/...` — deeplink-генератор приглашения в звонок (`defpackage/x6f.java`).
- `WebRTC` поверх `libjingle_peerconnection_so.so` (12.7 MB, lib OK.ru `libjingle`). Вместе с `one-video-calls-sdk`.
- Отдельный `ru.ok.android.externcalls.sdk.api.ApiProtocol` (`ru.ok.android.externcalls`) — это «внешние звонки» SDK OK.ru; параметры берутся через PMS-ключ `calls-endpoint`, то есть **сервер задаёт endpoint звонкового сигналинга** на лету.

### 6. Cleartext-каналы (Mobile ID операторов)

Описано в `01-mobile-id-cleartext.md` — отдельный whitelist HTTP к шести операторским HE/Mobile ID-эндпоинтам.

### 7. DNS

`defpackage/xl2.smali` строит DoH-запрос к `https://dns.google.com/resolve` (DoH JSON API Google). Включается PMS-флагом `net-client-dns-enabled`. То есть **в обход системного DNS приложение может ходить на Google DoH**, минуя локальный DNS-резолвер. В контексте РФ это интересно вдвойне: при условиях DPI/блокировки можно резолвить домены через Google.

Параллельно — `time.android.com` (NTP синхронизация), `firebaseinstallations.googleapis.com` (Firebase Installations ID, для FCM-токена).

## Сводно

```
MAX-клиент
  ├─> api.oneme.ru / api-test.oneme.ru / api-test2.oneme.ru / api-tg.oneme.ru   (бизнес WebSocket)
  ├─> https://max.ru/...                                                          (deeplink shortener, joincall)
  ├─> https://sdk-api.apptracer.ru/...                                            (Apptracer: crashes, ANR, heap dumps, perf metrics — OK.ru стек)
  ├─> https://tracker-api.vk-analytics.ru/?...                                    (MyTracker — VK attribution, ad-id, события)
  ├─> OneLog UploadService → внутр. бэкенд OK.ru                                  (ru.ok.android.onelog)
  ├─> OneLog UploadService(externcalls) → внутр. бэкенд OK.ru                     (ru.ok.android.externcalls.analytics)
  ├─> mobileid.megafon.ru / idgw.mobileid.mts.ru / hhe.mts.ru / he-mc.tele2.ru / he-mc.t2.ru / balance.beeline.ru   [HTTP cleartext, header enrichment]
  ├─> dns.google.com/resolve                                                      (DoH, при включённом флаге)
  ├─> firebaseinstallations.googleapis.com                                        (FCM)
  ├─> geocode-maps.yandex.ru / static-maps.yandex.ru / tiles.api-maps.yandex.ru   (Yandex Maps)
  ├─> vkvideo.ru / live-streams                                                   (VK Video, при PMS-флаге)
  └─> pagead2.googlesyndication.com                                               (Google Ad Services — в манифесте AD_ID permission, build с гугл рекламой)
```

Всё это плюс: куча PmsKey-флагов, которые меняют верифицируемое поведение этих каналов в рантайме без апдейта.

## Что важно

- В одном мессенджере **четыре независимых телеметрических канала наружу** (Apptracer, MyTracker, OneLog общий, OneLog для звонков), плюс **бизнес-канал**, плюс **HE-каналы операторов в открытом HTTP**.
- Каждый канал серверно-управляемый.
- HeapDumpInitializer объявлен прямо в манифесте, то есть «снимок памяти процесса в crash-репорт» — штатный сценарий, по серверной команде.
- DoH через Google DNS в дополнение к локальному резолверу — это и про обход блокировок CDN, и про маршрут ухода от локального DNS-надзора.
