# MAX 26.15.3 — Инвентарь и точки интереса

Пакет: `ru.oneme.app` (внутри код использует `one.me.*` и `ru.ok.tamtam.*`)
Имя приложения: `MAX` (внутреннее: `OneMe`)
APK: `apk/max-26.15.3-base.apk` (29 MB) + `apk/max-26.15.3-arm64.apk` (25 MB, native libs).
SDK: compile/target = 36 (Android 16). minSdk не виден напрямую в xml; нужно вытащить из apktool.yml.
Класс приложения: `one.me.android.OneMeApplication`.
Бэкап: `allowBackup="false"` (защита от анализа через `adb backup`).
Native libs не извлекаются (`extractNativeLibs="false"`), мапятся прямо из APK.

## Структура APK

- `classes.dex`, `classes2.dex`, `classes3.dex` — Java/Kotlin байткод (≈15 782 классов в jadx).
- `assets/` — 572 файла:
  - `dexopt/baseline.prof`, `baseline.profm` — startup-профайл ART.
  - `february23_pattern.svg`, `march08_pattern.svg`, `lebedev_pattern.svg`, `moscow_pattern.svg`, `nature_pattern.svg`, `neon_pattern.svg`, `new_year_pattern.svg`, `space_pattern.svg` — фоны чатов. Включая 23 февраля и 8 марта (тематика РФ).
  - `max_colors_schemes.bin` — бинарная палитра.
  - `shaders/*.glsl` — OpenGL ES2/ES3 шейдеры для медиа/transformer.
  - `io/michaelrocks/libphonenumber/...` — оффлайн БД libphonenumber (≈540 файлов).
- `lib/arm64-v8a/` (16 SO):
  - `libEnhancementLibShared.so` (5.7 MB) — что-то крупное "enhancement";
  - `libffmpg.so` (1.9 MB) — FFmpeg (см. дальше что за версия);
  - `libjingle_peerconnection_so.so` (12.7 MB) — WebRTC (libwebrtc/libjingle, для звонков);
  - `libjlottie.so`, `libgifimage.so`, `libgleff.so`, `libnative-filters.so`, `libnative-imagetranscoder.so`, `libstatic-webp.so`, `libimagepipeline.so`, `libimage_processing_util_jni.so`, `libsurface_util_jni.so` — Fresco/медиа;
  - `libqrcode.so` — QR;
  - `libtracernative.so` (799 KB) — нативный мост `ru.ok.tracer` (трейсинг/краш-репорты OK.ru);
  - `libzstd.so` — Zstandard;
  - `libc++_shared.so`.

## AndroidManifest — что бросается в глаза

### 1. Manifest placeholder утёк в production

В строке `<uses-permission android:name="android.permission.POST_NOTIFICATIONS"/> ${REQUEST_INSTALL_PACKAGES} <uses-permission android:name="android.permission.REQUEST_IGNORE_BATTERY_OPTIMIZATIONS"/>` сохранилась неподставленная Gradle-переменная `${REQUEST_INSTALL_PACKAGES}`. Android-парсер это просто проигнорирует, но факт сам по себе говорит, что при включении флага сборка добавила бы `REQUEST_INSTALL_PACKAGES` — то есть в коде где-то заложен путь установки сторонних APK (вероятно auto-update side-load механизм).

### 2. Cleartext HTTP к Mobile ID операторов России (network_security_config)

```
mobileid.megafon.ru       -> cleartextPermitted=true
idgw.mobileid.mts.ru      -> cleartextPermitted=true
hhe.mts.ru                -> cleartextPermitted=true
he-mc.tele2.ru            -> cleartextPermitted=true
he-mc.t2.ru               -> cleartextPermitted=true
balance.beeline.ru        -> cleartextPermitted=true
```

Это endpoint-ы Header Enrichment у мобильных операторов: оператор сам подставляет в HTTP-заголовки идентификатор абонента (MSISDN/IMSI/CID) и сервер MAX проверяет «кто звонит» БЕЗ SMS-кода. Запросы намеренно идут в открытом HTTP, иначе HE не работает (TLS отделит абонента от заголовков). Это и есть «вход без SMS», и это же — wiretap-friendly канал: трафик читаемый посередине.

### 3. Полный TamTam-стек поверх MAX

- `ru.ok.tamtam.android.services.NotificationTamService`
- `ru.ok.tamtam.android.services.BootCompletedReceiver` (BOOT_COMPLETED + QUICKBOOT_POWERON + HTC QUICKBOOT)
- `ru.ok.tamtam.android.calls.MediaProjectionService` (запись экрана со звонком)
- `ru.ok.android.onelog.UploadService` — analytics OK.ru (onelog).
- `ru.ok.android.externcalls.analytics.internal.upload.UploadService` — отдельный аналитический uploader для звонков.
- `ru.ok.tracer.*` (NativeBridge, CrashReport, PerformanceMetrics, DiskUsage, HeapDump, Logger, Tracer) — трейсер OK.ru. Это форк-наследие; в коде ниже видно что тот же стек.

### 4. NFC HCE (Host Card Emulation) для WebApps

`one.me.webapp.util.WebAppNfcService` — `<service exported="true" permission="BIND_NFC_SERVICE">` с `host_apdu_service`, AID = `F0010203040303`. То есть мини-приложения внутри MAX могут эмулировать смарт-карту по NFC. Это редкая возможность для мессенджера: WebApp-у можно дать через JS-bridge доступ к APDU.

### 5. Фоновое слежение жизнью

- `one.me.background.wake.BackgroundListenService` — foregroundServiceType=`dataSync`.
- `one.me.background.wake.BackgroundCheckReceiver`.
- `one.me.background.wake.BackgroundWakeBootReceiver` — слушает `MY_PACKAGE_REPLACED` и `BOOT_COMPLETED`.

«Background wake» — то есть приложение специально просыпается после ребута и после собственного обновления, поднимает foreground-сервис под видом синхронизации.

### 6. Объединённый foreground-тип у звонков

`one.me.calls.impl.service.CallServiceImpl` имеет `foregroundServiceType="camera|mediaPlayback|mediaProjection|microphone|phoneCall"`. mediaProjection в звонке — это запись экрана. То есть инфраструктура звонка содержит right на запись экрана наряду с микрофоном и камерой; это шире чем просто видеозвонок.

### 7. MainActivity exported=true и широкие SEND-фильтры

`one.me.android.MainActivity` экспортирована, ловит `SEND` / `SEND_MULTIPLE` со всеми MIME-типами. Это нормальная share-target, но в комбинации с deeplink-handler `LinkInterceptorActivity` (autoVerify=true для http://max.ru, max://max.ru) даёт большой attack surface для intents.

### 8. Прочие интересные пермы

- `MANAGE_OWN_CALLS` + ConnectionService → MAX может встраиваться в системную звонилку как полноправный SIM-аналог.
- `SYSTEM_ALERT_WINDOW` — оверлеи поверх других приложений.
- `BLUETOOTH_CONNECT`, `NFC`, `RECEIVE_BOOT_COMPLETED`, `FOREGROUND_SERVICE_*` (camera/microphone/mediaPlayback/mediaProjection/phoneCall/dataSync) — почти полный набор для устойчивого фонового присутствия.
- `READ_CONTACTS` + `WRITE_CONTACTS` — контакты можно и читать, и писать обратно (записывает «друзей из MAX» в адресную книгу телефона).
- `com.google.android.gms.permission.AD_ID` — рекламный ID Google.

### 9. Hardcoded ключ

`com.google.android.geo.API_KEY = AIzaSyDJbuC3fODS_aR7jcOkoP6qWIsQen9XARI` — Google Maps Geo API key прямо в манифесте.

### 10. Иконка ic_launcher_9may

В релизной сборке стоит праздничная иконка «9 мая» (`@mipmap/ic_launcher_9may`). Зафиксировано как факт.

### 11. Deeplink-схемы

- `app_scheme = max`, `app_host = max.ru`.
- Регистрируется как обработчик `https://max.ru/...` (autoVerify=true) и `max://max.ru/...`.
- Также матчится `http://...` с тем же host (т.е. cleartext-вариант).

