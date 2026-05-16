# MAX 26.15.3 — что внутри. Сухие факты для статьи

Анализ APK `ru.oneme.app` версии 26.15.3 (база `apk/max-26.15.3-base.apk`, нативные либы `apk/max-26.15.3-arm64.apk`). Декомпиляция — apktool 2.x + jadx 1.5.5 (≈15 782 классов, 23 795 java-файлов в jadx-выводе, ≈2 783 ресурса).

Ниже — выжимка из подробных тем в `notes/topics/`. Это не «список CVE», это про то, **как продукт устроен и что из этого устройства следует**.

## TL;DR (один абзац)

MAX — это форк-клиент TamTam (`ru.ok.tamtam.*` пакеты живут поверх `one.me.*`), с полным OK.ru-стеком трейсинга и аналитики, со встроенным каналом авторизации по номеру через мобильных операторов в **открытом HTTP**, с активным детектором VPN и навязчивой «отключите VPN» плашкой по серверной команде, с полноценной мини-апп платформой, в которой мини-апа может через JS-bridge получить MSISDN абонента и эмулировать NFC-карту, с серверно-управляемым killswitch-ом, который по флагу превращает любую версию в «обновитесь по нашей ссылке `https://download.max.ru/`», и с 334 серверно-управляемыми параметрами поведения (логирование чувствительных данных, отправка геолокации, отключение валидации TLS-сессии, fake-чаты, fake-in-app-review, конфиг сбора не-контактов и т. д.). E2E-шифрования нет.


## 1. Авторизация по номеру через мобильного оператора в открытом HTTP

`res/xml/network_security_config.xml` whitelistит cleartext HTTP для шести доменов:

```
mobileid.megafon.ru
idgw.mobileid.mts.ru
hhe.mts.ru
he-mc.tele2.ru
he-mc.t2.ru
balance.beeline.ru
```

Это endpoints Header Enrichment / Mobile ID четырёх крупнейших операторов РФ. Запросы намеренно идут без TLS — иначе оператор не может дописать в headers MSISDN/IMSI/абонентский ID. Сервер MAX, видя enrichment-заголовки, опознаёт владельца SIM-карты без SMS.

Цена: канал идентификации в чистом HTTP. Что туда уходит и что приходит — видит и оператор, и любой посередине (домашний роутер, корпоративный Wi-Fi, прокси).

Подробно: `notes/topics/01-mobile-id-cleartext.md`.

## 2. «Отключите VPN»: серверно-управляемая нагибалка

Внутри `one/me/vpnconnectedwarning/` — отдельный модуль `vpn-connected-warning_release` со строками:

```
oneme_vpn_connected_title       = «Отключите VPN»
oneme_vpn_connected_description = «Чтобы пользоваться MAX»
call_screen_snackbar_title      = «Лучше без VPN»
```

Плашка вызывается из контактного списка, истории звонков, экрана начала беседы, экрана чата, экрана звонка. Детектится через `NetworkCapabilities.hasTransport(TRANSPORT_VPN=4)`. **Когда** показывать — определяется серверными параметрами:

```
PmsKey  show-vpn-chat-bottomsheet   (Int)
PmsKey  show-vpn-call-bottomsheet   (Int)
PmsKey  show-vpn-snackbar           (Bool)
```

Уровень `1` — плашка при каждом действии в чате/звонке, пока юзер не отключит VPN. Параллельно факт активного TRANSPORT_VPN отдельно записывается в сетевую статистику (`defpackage/ura.java`) и улетает в аналитику.

В мини-аппах — отдельный экзешн `WebAppHttpClient$WebAppHasVpnException`: HTTP-клиент мини-апы кидает его, если у юзера VPN. Мини-апы (включая, видимо, государственные) **намеренно не работают через VPN**.

Подробно: `notes/topics/02-vpn-warning.md`, `notes/topics/10-webapp-jsbridge.md`.

## 3. PmsKey: 334 серверно-управляемых параметра

`ru/ok/tamtam/android/prefs/PmsKey.java` — 334 enum-значения. Сервер пушит ключ-значение, клиент применяет в рантайме. Полный список — `findings/raw/pms_keys.txt`.

Из явно «не-обычных»:

- `log-full`, `log-sensitive`, `log-messages-meta`, `log-chat-meta`, `log-violations` — серверно включаемое **логирование чувствительных данных и метаданных сообщений/чатов** в продакшен-сборке.
- `send-location-enabled` — серверный switch отправки геолокации.
- `mytracker-enabled`, `analytics-enabled` — kill-switch внешней аналитики.
- `fake-chats`, `fake-in-app-review`, `calls-fakeboss-incoming-call-enabled` — поддельные UI/чат-объекты, включаемые сервером.
- `net-ssl-session-validate` — серверный флаг для **выключения валидации TLS-сессии**.
- `proxy`, `proxy-domains` — серверно-задаваемое проксирование.
- `net-client-dns-enabled` — DoH через `dns.google.com/resolve` мимо системного резолвера.
- `non-contact-collection-interval`, `non-contact-sync-time`, `non-contact-max-chunk-size` — серверный конфиг сбора **не-контактов** (записей телефонной книги, не являющихся пользователями MAX).
- `gost-check-env` — флаги проверки ГОСТ-окружения.
- `family-protection-botid`, `digitalid-botid` — server-controlled chat-id для системных ботов «Семейная защита» и «Цифровой ID».
- `webapp-phone-hash` — режим, при котором мини-аппам выдаётся **хеш номера телефона** как стабильный фингерпринт.
- `outgoing-call-uri` — URI исходящего звонка, задаваемый сервером.
- `devnull` (`DevNullServerConfig`) — отдельный серверный конфиг для «чёрной дыры» событий.

Подробно: `notes/topics/03-pms-server-flags.md`.

## 4. Четыре независимых телеметрических канала + бизнес-API

```
api.oneme.ru / api-test.oneme.ru / api-test2.oneme.ru / api-tg.oneme.ru   ← бизнес WS
sdk-api.apptracer.ru/api/crash/...                                         ← Apptracer (OK.ru): crashes, ANR, минидампы, heap dumps, perf metrics
tracker-api.vk-analytics.ru/?...                                            ← MyTracker (VK): attribution, AdId, события
ru.ok.android.onelog.UploadService                                          ← OneLog (OK.ru) общий
ru.ok.android.externcalls.analytics.internal.upload.UploadService           ← OneLog отдельный для звонков
mobileid.megafon.ru / idgw.mobileid.mts.ru / hhe.mts.ru / he-mc.tele2.ru / he-mc.t2.ru / balance.beeline.ru   ← cleartext HTTP, header enrichment
dns.google.com/resolve                                                      ← DoH-fallback
firebaseinstallations.googleapis.com / time.android.com                     ← инфра
geocode-maps.yandex.ru / static-maps.yandex.ru / tiles.api-maps.yandex.ru   ← Yandex Maps
download.max.ru                                                             ← для скачивания APK мимо Play
```

Apptracer стек встроен через manifest-инициализаторы: `NativeBridgeInitializer`, `CrashReportInitializer`, `PerformanceMetricsInitializer`, `DiskUsageInitializer`, **`HeapDumpInitializer`**, `LoggerInitializer`, `TracerInitializer`. То есть «снапшот памяти процесса» при определённых условиях — штатно, и условия задаются с сервера через PmsKey `perf-events`/`perf-registrar-config`.

`api-tg.oneme.ru` — отдельный сервер, наравне с production и двумя test-серверами, переключаемый через скрытое dev-меню.

Подробно: `notes/topics/04-telemetry-endpoints.md`.

## 5. DevMenu остался в production-сборке

В `one.me.devmenu` лежит полноценный модуль `dev-menu_release`: переключение API-сервера (`api.oneme.ru`, `api-test.oneme.ru`, `api-tg.oneme.ru`, `api-test2.oneme.ru`, `Custom`), переключение всех 334 фича-флагов, просмотр логов, тредов, дамп памяти.

Хранилище — `SharedPreferences("dev_tools")`. Любой пользователь, нашедший точку входа (по паттерну OK.ru — несколько тапов на «версия приложения» в About), может:

- переключить production-клиент на тестовый бэкенд;
- вручную включить себе `log-sensitive`, `fake-chats`, `gost-check-env`, и т. п.

Это не disabled-в-release заглушка, а штатный остаток в AAB.

Подробно: `notes/topics/05-dev-menu-in-prod.md`.

## 6. Killswitch версии и обновление APK мимо Google Play

`defpackage/mec.java` (упрощённо):

```java
public final boolean a() {
    if (((rtd) c).m(hbg.a, 0) == 1
        && zm0.c(serverProvidedVersion, "26.15.3")) {
        return true;
    }
    return false;
}
```

Сервер выставляет флаг + строку «устаревшей» версии, и клиент целиком превращается в `ForceUpdateScreen` с текстом

> «Ваша версия MAX устарела. Писать и звонить в этой версии не получится. Обновите приложение, чтобы продолжить использовать MAX»

В `defpackage/pb8.java` явно: при `forceUpdateLogic.isNeedForceUpdate()` входящие звонки молча сбрасываются (`Early return in onNotifCallStart cuz of forceUpdateLogic.isNeedForceUpdate()`).

Кнопка «Обновить» ведёт на `https://download.max.ru/#android?version=26.15.3` — собственный CDN, мимо Google Play. Дополнительно в `AndroidManifest.xml` сохранилась **неподставленная Gradle-переменная `${REQUEST_INSTALL_PACKAGES}`** в позиции `<uses-permission>` — заготовка под прямую установку APK из приложения.

Это полноценный механизм «всех гнать на новую версию по нашей ссылке», независимый от Play Store.

Подробно: `notes/topics/12-force-update-killswitch.md`.

## 7. Контакты: ContentObserver на адресной книге + двусторонняя синхронизация

`defpackage/a40.java`:

```java
this.a.getContentResolver()
      .registerContentObserver(ContactsContract.Contacts.CONTENT_URI, /*notifyForDescendants=*/true, u30Var);
```

Любое изменение в любой записи телефонной книги вызывает callback в реальном времени.

`defpackage/ndi.java` через SyncAdapter (`caller_is_syncadapter=true`) пишет в адресную книгу телефона записи о найденных пользователях MAX: имя, телефон, плюс собственный MIME-тип (`tt_contact_mimetype` — TamTam-наследие) с привязкой к MAX-id (`sync1=tamtam_user_id`).

Серверный конфиг сбора **не-контактов** — три PmsKey-параметра:

```
non-contact-sync-time
non-contact-max-chunk-size
non-contact-collection-interval
```

То есть сервер задаёт частоту, размер чанка и сам факт «сбора» записей телефонной книги, не являющихся пользователями MAX.

Подробно: `notes/topics/06-contacts.md`.

## 8. NFC HCE для мини-апок: телефон отвечает APDU тем, что положит мини-апа

В манифесте сервис `one.me.webapp.util.WebAppNfcService` extends `HostApduService` с AID `F0010203040303` (proprietary, не EMV). В `processCommandApdu`:

```java
if (b != -92) return new byte[]{0x6F, 0x00};  // 0xA4 = SELECT
byte[] bArr2 = (byte[]) ((jxb) this.b.getValue()).d.get();
if (bArr2 != null) return bArr2;              // что положила WebApp — то телефон и отдаст терминалу
```

Источник `jxb.d` — мини-апа внутри MAX через JS-bridge. То есть произвольная мини-апа может в рантайме «зарядить» телефон произвольным APDU-ответом и при касании к NFC-терминалу пользователь отдаст этот payload. Не банковский HCE (категория `other`), но любой другой сценарий — пропуск, лояльность, идентификация — реализуется без отдельного приложения.

Подробно: `notes/topics/07-nfc-hce-webapp.md`.

## 9. WebApp JS-bridge: `verify_mobile_id` отдаёт мини-апе MSISDN абонента

`defpackage/zij.java`:

```java
public final String c() { return "WebAppVerifyMobileId"; }
public final String d() { return "verify_mobile_id"; }
public final boolean e() { return true; }   // private = true
```

Контракт в `defpackage/snk.java`/`vnk.java`:

```
WebAppVerifyMobileIdRequest  { requestId, url }
WebAppVerifyMobileIdResponse { requestId, statusCode, headers, data }
```

Мини-апа передаёт URL мобильного оператора (один из шести whitelisted в `network_security_config`), нативный клиент MAX выполняет запрос **с мобильной сети устройства** (HTTP cleartext, чтобы оператор подставил headers) и возвращает мини-апе **полный ответ с заголовками**. Мини-апа парсит `headers` и узнаёт MSISDN. Метод помечен `private=true` — то есть для системных мини-апок, видимо, без отдельного OS-prompt-а.

Сюда же:

- `WebAppHasVpnException` — мини-апы кидают исключение и не работают через VPN.
- PmsKey `webapp-phone-hash` — серверно включаемая отдача мини-апам стабильного хеша номера телефона.
- PmsKey `webapp-push-open` — открытие мини-апы прямо из push-уведомления.
- PmsKey `webapp-exc` — серверный список chat-id мини-апок, которые забирают себе системные жесты.

Публичный JavascriptInterface — три точки входа: `postEvent(String, String)` (общий router, приватный/публичный варианты в `i6e.java`/`upk.java`), `resolveShare(String, byte[], String, String)`, `trackFcp(long)`. Все остальные методы маршрутизируются JSON-сообщениями через `postEvent`.

Подробно: `notes/topics/10-webapp-jsbridge.md`.

## 10. Background wake — «работа в фоне при перебоях интернета»

Пакет `one.me.background.wake`:

- `BackgroundListenService` — foreground-service `dataSync`, `notificationId=9001`, channel `ru.oneme.app.misc`, ongoing-нотификация с текстом «MAX работает в фоне / Вы будете получать уведомления». Internal log tag — `KeepBackground`.
- `BackgroundCheckReceiver` — broadcast-receiver, поддерживает alarm-цикл.
- `BackgroundWakeBootReceiver` — слушает `BOOT_COMPLETED` и `MY_PACKAGE_REPLACED`, после ребута/обновления переставляет alarm-ы и снова поднимает фоновое присутствие.

UX-обвязка вшито подаёт это как контрмеру блокировок:

```
oneme_main_background_wake_suggestion = «Оставайтесь на связи при перебоях интернета»
oneme_notifications_settings_background_wake_section_title = «Настройки при ограничениях сети»
```

Серверные параметры цикла: `keep-background-socket`, `ping-background-interval`. Сам факт включения этого режима репортится в аналитику отдельным эвентом `BACKGROUND_MODE / system_curtain_shown`.

Подробно: `notes/topics/08-background-wake.md`.

## 11. Состояние пользователя: серверно-управляемые «семейная защита» и «уровень контента»

`defpackage/sgj.java` — модель `UserSettings`, приходящая с сервера:

```
safeMode, safeModeNoPin,
contentLevelAccess,
familyProtection,
phoneNumberPrivacy,
searchByPhone,
unsafeFiles,
audioTranscriptionEnabled,
hiddenOnline, ...
```

`familyProtection` и `contentLevelAccess` приходят с сервера **в одном объекте с прочими настройками аккаунта**, синхронизируются между устройствами, и публичных переключателей в UI на них (по тому, что я нашёл по строкам) нет. Это значит, что внешняя сторона (включая, например, государственный сервис, если он подключён к серверу MAX) может включить пользователю «семейную защиту» и не дать выключить через настройки.

Системные боты, ID которых задаются с сервера: **`family-protection-botid`** (Семейная защита), **`digitalid-botid`** (Цифровой ID — государственная цифровая идентификация), `money-transfer-botid`, `stickers-botid`, `channel-statistics-botid`. UI вокруг них уже зашит в код.

PmsKey `gost-check-env` (`gostEnvironmentCheckFlags: Int`) — серверный hook на проверку ГОСТ-окружения (типичный для российских корпоративных приложений интерфейс под пакеты КриптоПро/ВипНет).

Подробно: `notes/topics/11-state-bots-and-content-policy.md`.

## 12. Native libs

16 .so в `lib/arm64-v8a/`, NDK r27b, `extractNativeLibs="false"`. Главное:

- `libjingle_peerconnection_so.so` (12.7 MB) — WebRTC/libjingle (звонки).
- `libEnhancementLibShared.so` (5.7 MB) — VK-овский DSP для аудио в звонках, namespace `vk::enh::EnhancementFactory`. Получает raw audio до WebRTC encoder.
- `libffmpg.so` (1.9 MB) — **FFmpeg n4.4.3** (старый, известные CVE), используется для перекодирования медиа.
- `libtracernative.so` (799 KB) — нативный мост Apptracer: handler SIGSEGV/SIGABRT, минидамп-writer (`Minidump.installMinidumpWriterImpl`).
- Остальные — Lottie, Fresco-декодеры, Zstd, QR.

Динамическая загрузка `.so` отключена: `<meta-data android:name="com.facebook.soloader.enabled" android:value="false"/>`. Скачиваний `.so` с сервера в коде не нашёл. Из `RtmpClient.java` есть `System.loadLibrary("rtmp-jni")` для библиотеки, **которой в APK нет** — мёртвый код, не runtime-side-loading.

Подробно: `notes/topics/09-native-libs.md`.

## 13. Прочие штрихи

- Иконка приложения в release — `@mipmap/ic_launcher_9may` («9 мая»).
- Темы 23 февраля 2026, 8 марта 2026, новогодняя 2026 — отдельные PmsKey-флаги.
- `ru.trace_flow.dps.internal.DpsInitProvider` — провайдер DPS (Data Processing Service?), в манифесте `enabled="false"`, но регистрация осталась. PmsKey `dps` есть.
- `MANAGE_OWN_CALLS` + `ConnectionService` (`CallServiceImpl`) — MAX встраивается в системную звонилку и может имитировать SIM-аналог.
- `foregroundServiceType` у звонкового сервиса включает `mediaProjection` — то есть звонок инфраструктурно может писать экран.
- Network statistics (`defpackage/ura.java`) отдельной категорией репортит `transport=vpn` — факт активного VPN-туннеля у пользователя записывается в телеметрию.
- `SYSTEM_ALERT_WINDOW`, `USE_FULL_SCREEN_INTENT`, `REQUEST_IGNORE_BATTERY_OPTIMIZATIONS` — полный набор привилегий для продавливания UI-присутствия.
- Hardcoded Google Maps API key прямо в манифесте: `AIzaSyDJbuC3fODS_aR7jcOkoP6qWIsQen9XARI`.

## 14. Чего нет

- **Нет** end-to-end шифрования. В коде нет следов libsignal, olm, matrix-rust-sdk и т.п. Сообщения проходят через сервер MAX в виде, который сервер обрабатывает и хранит.
- **Нет** runtime-загрузки `.so` с сервера (Facebook SoLoader выключен, ссылок на `.so` в http нет).
- **Нет** анти-tampering / детектора root/Frida/Xposed/debuggable, который мог бы помешать ресёрчу.

## Что это в сумме

Под капотом MAX — переупакованный TamTam с полным OK.ru-стеком (трейсер, OneLog, MyTracker, externcalls SDK), плотно интегрированный с инфраструктурой российских мобильных операторов (Mobile ID/HE) и со штатными «хуками» под российский гос-стек (DigitalID, Family Protection, GOST env check). Поведение клиента на ⅔ контролируется сервером в рантайме (PmsKey, UserSettings), без необходимости обновления приложения. Канал доставки самих обновлений — собственный (`download.max.ru`), и существует серверный killswitch, мгновенно превращающий любую устаревшую версию в нерабочую.

Если коротко: это не «ещё один мессенджер с особенностями», это **универсальный контейнер**, у которого «новые функции» добавляются переключением серверного флага, а «нежелательные пользователи/версии» отрезаются одним же серверным флагом.

---

Файлы:

- `notes/topics/01-mobile-id-cleartext.md`
- `notes/topics/02-vpn-warning.md`
- `notes/topics/03-pms-server-flags.md`
- `notes/topics/04-telemetry-endpoints.md`
- `notes/topics/05-dev-menu-in-prod.md`
- `notes/topics/06-contacts.md`
- `notes/topics/07-nfc-hce-webapp.md`
- `notes/topics/08-background-wake.md`
- `notes/topics/09-native-libs.md`
- `notes/topics/10-webapp-jsbridge.md`
- `notes/topics/11-state-bots-and-content-policy.md`
- `notes/topics/12-force-update-killswitch.md`
- `findings/raw/pms_keys.txt` — полный список 334 серверных флагов
- `findings/raw/urls_quoted_jadx.txt`, `findings/raw/hosts_quoted_jadx.txt` — реквизиты outbound-каналов
- `notes/00-INVENTORY.md` — что лежит в APK и что бросается в глаза в манифесте


---

## Дополнения (темы 15-23)

### 15. On-device ASR / KWS / Diarization в `libEnhancementLibShared.so`

`libEnhancementLibShared.so` (5.7 MB) — не «DSP для звонков», а embedded-runtime нейросетевой обработки звука: Conformer-CTC ASR, BCResNet KWS (streaming, для детекции ключевых слов), Diarization, AudioClassifier, Punctuation predictor. Использует TFLite + XNNPACK + опционально Android NNAPI. Build path указывает на собственный `webrtc4` форк с интегрированным `one-ann-audio-processing`. Подробно: `notes/topics/15-on-device-asr-kws-diarization.md`.

### 16. ML-модели KWS/NS качаются с сервера в рантайме

Сервер пушит JSON `{ url, checksum, enabled }` под ключом `android.mlfeatures.ws_0` (KWS) и аналог для NS. Клиент через `DownloadService` качает `.tflite + .cfg`, валидирует checksum, активирует. Никакого whitelist URL в коде нет. KWS работает в streaming-режиме на сыром аудио из звонка, статистика срабатываний — в `ConversationKwsStat`. Подробно: `notes/topics/16-server-pushed-ml-models-in-calls.md`.

### 17. Apptracer uplink: что и куда

URL `https://sdk-api.apptracer.ru` захардкожен в Java. Endpoints: `/api/sample/initUpload`+`/api/sample/upload` (общий sample-uploader), `/api/perf/upload`, `/api/crash/trackSession`. `SampleUploadWorker` принимает любой файл + произвольные key/value-метаданные. Heap-dumps (через `ShrinkDumpWorker`) идут тем же каналом. Декомпиляция нативной части (`findings/native/decomp_tracer/`): функции `tracer_set_api_endpoint`, `tracer_set_ssl_cainfo`, `tracer_supports_arbitrary_dump` — vtable-обёртки backend-объекта, в JNI не выставлены. Серверные ключи: `tracer-non-fatal-crashed-enabled`, `perf-events`, `perf-registrar-config`, `log-full`, `log-sensitive`, `log-violations`. Подробно: `notes/topics/17-apptracer-uplink.md`.

### 18. Manifest deep-dive

55 пермов (37 штатных + 18 кастомных) в т. ч. RECORD_AUDIO, CAMERA, FOREGROUND_SERVICE_{CAMERA,MEDIA_PROJECTION,MICROPHONE,PHONE_CALL}, MANAGE_OWN_CALLS, ACCESS_FINE_LOCATION, READ/WRITE_CONTACTS, NFC, SYSTEM_ALERT_WINDOW. 26 services, 16 receivers, 8 providers. Manifest-placeholder `${REQUEST_INSTALL_PACKAGES}` остался неподставленным. `FirebaseInitProvider` отключён (Firebase используется только для FCM-push). `DpsInitProvider` (`ru.trace_flow.dps`) — отключён, но регистрация в манифесте есть. Подробно: `notes/topics/18-manifest-deep-dive.md`.

### 19. FCM push — командный канал

Типы push (поле `type` в payload): обычное сообщение, `MessageRemoved`/`ChatMessageRemoved` (сервер удаляет сообщение из локальной БД клиента), `InboundCall` (включает `eKey`, `vcId`, `userName`, `country` и т.д.), `TamtamSpam` (deeplink с произвольным `uri`, без whitelist схем), `LocationRequest` (wake-trigger: lastPushTime + force WS reconnect — координаты не отправляются прямо в push-ответе). Гейтинг: ключ `c` в payload — recipient userId; не-свой пуш отбрасывается. Подробно: `notes/topics/19-fcm-push-payload.md`.

### 20. WS-протокол: 159 опкодов

Полный enum в `defpackage/u0d.java`. Полный список — `findings/raw/ws_opcodes.txt`. Несколько примечательных:

- `LOCATION_REQUEST(126)` / `LOCATION_SEND(125)` — серверный → клиентский запрос координат (пара к push-типу `LocationRequest`).
- `EXTERNAL_CALLBACK(105)` — серверная подпись OAuth-callback внешнему приложению (MAX как Identity Provider).
- `PHONE_WEBAPP_SHARE(106)` — отдача MSISDN мини-приложению.
- `LINK_INFO(89)` — серверный resolve URL (любой URL из переписки уходит на сервер для preview).
- `DRAFT_SAVE(176)` / `NOTIF_DRAFT(152)` — синхронизация черновиков через сервер; то, что юзер набирает (но не отправил) — у сервера в открытом виде.
- `NOTIF_PROFILE(159)` — серверный апдейт собственного профиля пользователя (отображаемое имя/аватар).
- `TRANSCRIBE_MEDIA(202)` — серверная транскрипция (альтернатива on-device ASR).
- `OK_TOKEN(158)` — обмен SSO-токеном с другими сервисами VK/OK экосистемы.
- `CHAT_SEARCH_COMMON_PARTICIPANTS(198)` — поиск общих участников чатов (раскрывает социальный граф).

Опкодов E2E-handshake / pre-key bundle / GOST-крипто — нет. Это согласуется с отсутствием сквозного шифрования в протоколе.

Подробно: `notes/topics/20-ws-protocol-opcodes.md`.

### 21. SharedPreferences карта

Имена prefs-файлов: `dev_tools` (DevMenu в release), `stat_prefs` (server-pushed phonebook-size метрики), `presences.pref`, `permissions_prefs`, `app_crash_prefs`, `in_app_review_prefs`, `keyboard_prefs`, `webrtc-android-sdk-pref`, `one.me.sdk.design.theme`, `tracer`, `tracer-<подмодуль>`. Из MyTracker: `mytracker_prefs` с полями `referrer`, `attribution`, `installTimestamp`, `preinstallRead` (флаг прочтения preinstall-vendor-info; код умеет ловить атрибуцию принудительной OEM-предустановки) + отдельная ветка `huaweiApiReferrerSent` для Huawei AppGallery. Подробно: `notes/topics/21-shared-prefs-map.md`.

### 22. GOST / DigitalID / FamilyProtection

Три PmsKey: `family-protection-botid` (#187), `digitalid-botid` (#305), `gost-check-env` (#127). Все три — server-pushed точки расширения. Бот-ID — chat-id специальных серверных ботов; gost-check-env — bitmask проверок ГОСТ-окружения. Сам ГОСТ-крипто-провайдер в APK не вкомпилен (нет импорта `ru.CryptoPro.*` или подобных) — только UI-хуки. UserSettings.familyProtection приходит с сервера в одном объекте с `safeMode`, `contentLevelAccess`, `phoneNumberPrivacy`, `audioTranscriptionEnabled` и т.д. — то есть гос-фильтры технически перемешаны с настройками приватности и приходят одним пакетом. Подробно: `notes/topics/22-gost-digitalid-family.md`.

### 23. Точки запуска камеры / микрофона / записи экрана

Шесть путей: (1) `MediaProjectionManager.createScreenCaptureIntent()` в `CallScreen` (только из звонка), (2) WebRTC mic в звонке (нативный `AudioRecord` в `libjingle_peerconnection_so.so`), (3) `MediaRecorder` в голосовых сообщениях (`defpackage/kd0.java`) или альтернатива `OpusRecorder` (PmsKey `opus-recorder`), (4) KWS использует поток из звонка (вне-звонковой запуск микрофона из jadx-кода не виден), (5) CameraX-стек видеосообщений-«кружков» (`VideoMessageRecordDelegate`), (6) WebRTC camera в видеозвонке. Все шесть требуют либо runtime-permission, либо UI-действие. Один foreground-service `CallServiceImpl` имеет `foregroundServiceType="camera | microphone | mediaPlayback | mediaProjection | phoneCall"` — то есть в момент звонка единый узел держит все капабилити захвата. Серверный gate `calls-sdk-log-audio` создаёт side-channel «копия аудио в SDK-логе». Подробно: `notes/topics/23-camera-mic-screen-entry-points.md`.

---

## Что добавилось в общую картину

К 14 исходным разделам:

1. **Голосовая аналитика на устройстве** (15) — ASR/KWS/Diarization/Audio classifier ШТАТНО встроены в звонковый стек. Это переводит «MAX не хранит сырое аудио» в «MAX крутит ASR на сыром аудио прямо в клиенте, и серверная команда может включить любую модель».
2. **Серверно-доставляемые ML-модели** (16) — KWS/NS-модели не лежат в APK, а тянутся по URL из серверного конфига `android.mlfeatures.ws_0`. То есть «что детектируется» обновляется без релиза клиента.
3. **Apptracer uplink** (17) — `sample`-uploader штатно умеет лить любой файл + произвольные метаданные на `sdk-api.apptracer.ru`. Heap-dumps процесса MAX (где в памяти есть и токены, и сообщения, и контакты) идут тем же каналом по серверной команде.
4. **Manifest deep-dive** (18) — формальная карта attack surface клиента: 55 пермов, оставшийся `${REQUEST_INSTALL_PACKAGES}`-плейсхолдер, отключённые-но-зарегистрированные провайдеры (`DpsInitProvider`).
5. **FCM как командный канал** (19) — подтверждено: `MessageRemoved` (удаление сообщения из локалки), `TamtamSpam` (произвольный URI deeplink), `LocationRequest` (wake-trigger).
6. **WS-протокол на 159 опкодов** (20) — серверные нотификации включают `NOTIF_PROFILE` (изменить профиль), `LINK_INFO` (резолв URL на сервере), `LOCATION_REQUEST`, синхронизацию черновиков, серверную транскрипцию.
7. **GOST-крипто отсутствует**, но хуки оставлены (22). Family Protection и DigitalID — серверно-управляемые секции UI.
8. **Камера/микрофон/screen capture** — карта всех шести путей запуска (23). Нет «зерокликом» сценария в этой версии.

Если нужно одной фразой — MAX 26.15.3 это **тонкий клиент серверно-управляемого UX, с локально-исполняющейся ML-обработкой звука по серверным моделям, с собственным каналом доставки артефактов клиента (heap dumps, samples) на инфраструктуру VK/OK, и без сквозного шифрования.**

---

Файлы (продолжение):

- `notes/INDEX.md` — Map of Content (Obsidian)
- `notes/topics/13-deeplinks-idp.md`
- `notes/topics/14-stat-prefs-metrics.md`
- `notes/topics/15-on-device-asr-kws-diarization.md`
- `notes/topics/16-server-pushed-ml-models-in-calls.md`
- `notes/topics/17-apptracer-uplink.md`
- `notes/topics/18-manifest-deep-dive.md`
- `notes/topics/19-fcm-push-payload.md`
- `notes/topics/20-ws-protocol-opcodes.md`
- `notes/topics/21-shared-prefs-map.md`
- `notes/topics/22-gost-digitalid-family.md`
- `notes/topics/23-camera-mic-screen-entry-points.md`
- `findings/raw/ws_opcodes.txt` — полный список 159 WS-опкодов
- `findings/native/decomp_tracer/` — rzghidra-декомпиляция libtracernative + skeptic README


---

## Дополнения (темы 24-28)

### 24. HostReachabilityChecker — server-controlled probe + IP/operator/VPN reporting

`defpackage/uq5.java` + `f58.java` — фоновая задача `HostReachabilityChecker`. Список хостов задаётся сервером через PmsKey `host-reachability` (#227). На каждый хост клиент: (1) DNS-резолв, (2) `InetAddress.isReachable()`, (3) прямой `new Socket()` connect. Результаты + `operator` (TelephonyManager NetworkOperator/Name) + `connection_type` + `ip` + `vpn`-флаг → событие OneLog `HOST_REACHABILITY/GET_HOST_REACHABILITY`. Это подтверждает прессовские наблюдения «MAX обращается к Telegram/WhatsApp/AWS/Google». Список конкретных хостов — серверный, в коде не зашит. Подробно: `notes/topics/24-host-reachability-probe.md`.

### 25. Resources audit — встроенная функция записи звонка

В `assets/`: 8 SVG-паттернов фонов (включая 23 февраля, 8 марта, новогодний 2026), 18 GLSL-шейдеров для медиа, 540+ файлов libphonenumber. ML-моделей `.tflite` в APK **нет** (качаются с сервера, см. topic 16). В `res/raw/`: звуки звонка плюс **`call_record_start.m4a` / `call_record_stop.m4a`** — встроенные звуковые маркеры записи. Strings.xml содержит группу `call_screen_record_*`: «Запись пришлём в избранное», «Администратор остановил запись экрана» — **запись звонка/экрана это штатная клиентская фича**. Запускается через `MediaProjectionManager.createScreenCaptureIntent()` из `CallScreen` (см. topic 23), записывается локально, заливается в чат «Избранное» инициатора (без E2E — на сервер). Также найдены `fake_boss_*` строки + PmsKey `calls-fakeboss-incoming-call-enabled` — сервер может помечать профили как «принадлежит организации». Подробно: `notes/topics/25-resources-audit.md`.

### 26. Upstream — только bot SDK публично

`github.com/max-messenger` содержит 5 публичных репозиториев — **все bot-SDK** (TS/Go/Python + UI компоненты). **Исходного кода клиента в публичном виде нет**. Python-SDK `max-botapi-python` форкнут с третьестороннего `love-apples/maxapi`. Java-SDK (упоминавшийся в источниках 2025 года) публично не висит. Реверс самого клиента остаётся black-box reverse engineering из APK. Подробно: `notes/topics/26-upstream-public-repos.md`.

### 27. Hardcoded keys audit — без backdoor-ключей

Все хардкоженные значения — публичная Firebase/Google/MyTracker конфигурация: `gcm_defaultSenderId=659634599081`, `project_id=max-messenger-app`, два Google Maps API key (один — Maps Geo, второй — Firebase shared), MyTracker app id `34982109644049932883`. Captive private RSA/EC keys, JWT-секреты, HMAC-секреты, captive PEM/DER сертификаты в APK **не найдены**. Это значит — нет очевидных «backdoor-ключей» в смысле shared secret для отдельного канала. Подробно: `notes/topics/27-hardcoded-keys-audit.md`.

### 28. `vk::enh::decrypt` — статический ключ для obfuscation

В `libEnhancementLibShared.so` экспортирован `vk::enh::decrypt(std::vector<unsigned char>)`. Декомпиляция показывает, что функция передаёт **два захардкоженных 16-байтовых блока из `.rodata`** (по адресам 0x83513 и 0x83523) во внутренний крипто-контекст и обрабатывает входной buffer. По формату ключ+IV (16+16) похоже на AES-128. Это light-obfuscation для каких-то внутренних блобов (вероятно зашифрованные TFLite-модели или checksums). Не secret в security-смысле — кто угодно с APK может расшифровать; backdoor-индикатором не является, но архитектурно собирается с `vk::enh::dll::Dll::open` + downloadable models в путь «расшифровать blob → подгрузить как .so» (прямой улики использования такого пути в этой сборке нет). Подробно: `notes/topics/28-vk-enh-decrypt-key.md`.

---

## Полная сводка после 28 тем

К 14 исходным разделам и 9 дополнениям (15-23) добавились ещё 5:

1. **HostReachabilityChecker (24)** — это и есть тот самый код, который пресса (март 2026) обозначила как «MAX проверяет блокировки и собирает IP/оператора». Архитектурно — обычный фоновый job, серверно-управляемый список хостов, отчёт через OneLog. Подтверждается полностью.
2. **Built-in call recording (25)** — отдельная штатная фича в звонке («Запись пришлём в избранное»), не «случайное» использование MediaProjection. Записи без E2E хранятся на серверах MAX.
3. **Закрытость исходников (26)** — публично только bot SDK; полный код клиента — закрытый, реверсу подлежит только из APK.
4. **Нет backdoor-ключей в Java (27)** — все хардкоженные значения публичные. Это закрывает один из распространённых страхов «зашитый ключ для бэкдор-канала».
5. **Light obfuscation в native (28)** — статический ключ есть, но для интернала (модели/конфиги), не для канала. В сочетании с `dll::open`-обвязкой архитектурно поддерживает «runtime extension», прямой улики в этой версии нет.

---

Дополнительные файлы:

- `notes/topics/24-host-reachability-probe.md`
- `notes/topics/25-resources-audit.md`
- `notes/topics/26-upstream-public-repos.md`
- `notes/topics/27-hardcoded-keys-audit.md`
- `notes/topics/28-vk-enh-decrypt-key.md`


---

## Дополнения (темы 29-36)

### 29. ExternalCallback / OK_TOKEN — MAX как Identity Provider

WS-опкод `EXTERNAL_CALLBACK(105)` в паре с deeplink `:auth?externalCallback=1` и UI `ExternalCallbackWidget` (`one.me.android.externalcallback.*`). Цепочка: внешнее приложение открывает deeplink → MAX показывает bottom-sheet согласия → клиент по WS оформляет подпись через сервер → внешнее приложение получает токен. `OK_TOKEN(158)` — параллельно для SSO в OK.ru/VK ID. В сочетании с `digitalid-botid` MAX-аккаунт становится единым гос-привязанным user identity, single-point-of-control для всей цифровой личности в РФ. Подробно: `notes/topics/29-external-callback-idp-flow.md`.

### 30. Root-detection — без блокировки, но в телеметрию

В клиенте нет активной блокировки rooted-устройств / Frida / Xposed / debuggable. Но MyTracker SDK (`com/my/tracker/core/o/m.java`) детектит Magisk через файловые пути (`/sbin/.magisk/`, `/sbin/.core/mirror`...) и передаёт флаг `isRooted` в attribution-payload. Apptracer (`defpackage/ij9.java`) и общий event-stack (`defpackage/mpf.java`) добавляют `isRooted` в JSON-события на `sdk-api.apptracer.ru`. Не блокировка, но **detect-and-report**. Подробно: `notes/topics/30-root-detection-telemetry.md`.

### 31. OneLog event categories

Карта категорий событий, которые улетают через `ru.ok.android.onelog.UploadService` (BIND_JOB_SERVICE). Обнаружено 60+ файлов с вызовами `ok9.h(logger, category, operation, attributes)`. Категории: `PUSH/Action` (с `p_op`: `n_q_rep`, `n_canceled`, `open_chat`, `open_url`...), `CALL`, `AUDIO_STATS`, `VIDEO_MESSAGE`, `SETTINGS`, `BACKGROUND_MODE`, `POWER_SAVING`, **`PERMISSION/permission_status`** (DailyAnalyticsWorker — отчёт всех runtime-permission'ов), `PERMISSION/permission_changed_state`, `CONTACT_OR_BLOCK`, `CONTEXT_MENU`, `CHANNEL_RECSYS_FOLDER`, `INVITE_MAX_BANNER`, `SHARE_TO_MAX`, `sticker/send_sticker`, `CLICK`, `HOST_REACHABILITY`, `DEV`. Поведенческий профиль строится в очень мелкой гранулярности. Подробно: `notes/topics/31-onelog-event-categories.md`.

### 32. Presence — server-controlled видимость

Поле `hiddenOnline` в UserSettings приходит с сервера и побеждает локальный чекбокс «скрыть онлайн». 9 PmsKey семейства `presence-*`: `presence-ttl`, `presence-view-port`, `presence-external` (видимость для смежных сервисов), `presence-seen-eq`, `presence-stat` (лог в OneLog), `presence-keep-bg-cache`, `presence-offline-move-timeout`, `presence-offline-log`, `notif-typing-presence`. Сервер контролирует, кто и когда видит онлайн-статус пользователя. Подробно: `notes/topics/32-presence-server-controlled.md`.

### 33. WS framing — MessagePack

Бинарный msgpack-RPC через `org.msgpack.core` (`defpackage/j8h.java`, `kxa.java` — обёртка `MessageUnpacker`). Формат frame-а — array первым элементом opcode short, далее payload. Типы — стандартные msgpack (varint64 для id, utf-8 length-prefix для строк, рекурсивные map/array). Сжатие zstd опционально (`libzstd.so` есть). `wss://` на TLS, без E2E внутри. Сервер может отключить TLS-валидацию через PmsKey `net-ssl-session-validate`. Подробно: `notes/topics/33-ws-msgpack-framing.md`.

### 34. Yandex Maps — три endpoint-а с утечкой координат

Все картографические запросы клиента идут на Yandex (не через MAX-сервер): `https://geocode-maps.yandex.ru/v1?...&geocode=<lat,lon>&apikey=...` (reverse geocoding), `https://static-maps.yandex.ru/v1?...&maptype=future_map&...` (raster preview), `https://tiles.api-maps.yandex.ru/v1/tiles/?...&projection=web_mercator&...` (tile server). Yandex видит координаты и IP пользователей MAX отдельно от сервера MAX. PmsKey `y-map` — серверный switch (теоретически может быть переключено на другой backend). Подробно: `notes/topics/34-yandex-maps-integration.md`.

### 35. MyTracker AntiFraud — сенсорный fingerprint

MyTracker SDK (VK) использует 5 сенсоров устройства для fingerprint'а и детекта эмулятора: light sensor, magnetic field sensor, gyroscope, pressure sensor, proximity sensor. По умолчанию все включены. Результат + derivative-метрики уходят на `tracker-api.vk-analytics.ru` в составе attribution-payload. Sber-antifraud (анонсированный для MAX в прессе для антимошеннических звонков) в этой версии **отсутствует** — есть только VK-овский attribution antifraud. Сенсорный fingerprint позволяет идентифицировать устройство сквозь переустановку приложения. Подробно: `notes/topics/35-mytracker-antifraud-sensors.md`.

### 36. Channels — server-gated rollout

Каналы (broadcast 1-N) — feature-flagged через PmsKey `channels-enabled` (#76). По состоянию на август 2025 (Wikipedia: «функционал недоступен обычным пользователям») фича выключена для общих пользователей. PmsKey семейства каналов: `channel-statistics-botid` (#192, серверный бот статистики), `channels-complaint-enabled`, `channels-suggests-folder`, `channel-view-config`, `bots-channel-adding`. WS-опкод `CHAT_HIDE(196)` — отдельный механизм для **серверного скрытия канала из списка пользователя без удаления** (тихая server-side модерация). Подробно: `notes/topics/36-channels-feature-gated.md`.

---

## Полная сводка после 36 тем

К 14 исходным разделам и трём наборам дополнений (15-23, 24-28, 29-36) добавились ещё 8:

1. **MAX как Identity Provider (29)** — WS+deeplink+UI цепочка для OAuth-style передачи личности внешним сервисам, плюс SSO в экосистему VK/OK через `OK_TOKEN`.
2. **Detect-and-report для root (30)** — не блокирует, но репортит. Сервер всегда знает root-статус устройства.
3. **OneLog событийная карта (31)** — поведенческий профиль на уровне «каждый клик / каждый swipe push / каждое изменение пермы / каждый стикер с контекстом».
4. **Presence как серверная политика (32)** — `hiddenOnline` server-controlled, не локальный privacy-control.
5. **MessagePack-framing (33)** — бинарный протокол; не E2E, на TLS, с серверным kill-switch валидации сессии.
6. **Yandex как отдельный канал утечки геоданных (34)** — координаты идут на Yandex напрямую, не через MAX-server.
7. **Сенсорный device-fingerprint (35)** — 5 датчиков для анти-эмулятор / device-identity сквозь переустановку.
8. **Server-gated каналы + CHAT_HIDE (36)** — server-side модерация архитектурно достаточная для тихой цензуры.

---

Дополнительные файлы:

- `notes/topics/29-external-callback-idp-flow.md`
- `notes/topics/30-root-detection-telemetry.md`
- `notes/topics/31-onelog-event-categories.md`
- `notes/topics/32-presence-server-controlled.md`
- `notes/topics/33-ws-msgpack-framing.md`
- `notes/topics/34-yandex-maps-integration.md`
- `notes/topics/35-mytracker-antifraud-sensors.md`
- `notes/topics/36-channels-feature-gated.md`

---

## Дополнения 37-48

### 37. WebApp privilege configuration — webapp-exc bypass user-touch check

`webapp-exc` (#214) — server-pushed список chat-id мини-апок, которые могут вызывать JS-bridge методы **без подтверждения пользовательским касанием**. Из `e2h.java`: «Боты-исключения из правила проверки пользовательского касания перед выполнением методов бриджа». Default `[0]`, тестовый бот `1496626`, продовый `4810464`. Дополнительно: `webapp-phone-hash` (#295) — server-pushed хеш номера телефона для мини-апок; `webapp-push-open` (#268) — открытие мини-апки прямо из push. Подробно: `notes/topics/37-webapp-privilege-configuration.md`.

### 38. Deeplink routes — 8 внутренних роутов

Роутер `ps0.A(...)` регистрирует 8 роутов: `:auth`, `:external_callback`, `:complaint`, `:settings`, `:chats`, `:profile`, `:join`, `:share`. Все три схемы (`https://max.ru/`, `http://max.ru/`, `max://max.ru/`) ведут в один роутер. Внешние приложения через deeplink могут открыть профиль, чат, форму жалобы, форму отправки с предзаполненным текстом, IdP-flow. Подробно: `notes/topics/38-deeplink-routes-full-map.md`.

### 39. libffmpg.so — FFmpeg n4.4.3, GPLv3 в проприетарном продукте

`libffmpg.so` — кастомный VK-build FFmpeg n4.4.3 (ноябрь 2022, 3.5 года старый). Собран с `--enable-gpl --enable-version3` (строка `libavcodec license: GPL version 3 or later`). GPLv3-компонент в проприетарном приложении без видимой attribution-страницы. Поверхность атаки: все codecs FFmpeg, которые клиент пускает на присланные медиа (thumbnail-генерация, seek-предпросмотр). Подробно: `notes/topics/39-libffmpg-version-license.md`.

### 40. Multi-account (two-account-mvp) — server-gated

PmsKey `two-account-mvp` (#195) — server-gated включение второго аккаунта. Два аккаунта = два WS-сессии одновременно. Переключение в рамках одного процесса через `hr9.java` («Swap user account from X to Y»). Общий FCM-token для обоих аккаунтов. Подробно: `notes/topics/40-multi-account-server-gated.md`.

### 41. Server-side client diagnostic struct — proxy/isVpn/location/app-update

`defpackage/ksg.java` парсит серверную структуру с полями: `proxy` (URL прокси), `proxy-domains` (список доменов для прокси), `isVpn` (серверная детекция VPN), `lang`, `callsSeed`, `reg-country-code`, `app-update-type` (0/1/2 = none/optional/force), `location` (server-detected country code по IP). Сервер знает страну пользователя по IP и может задать proxy-routing для конкретных доменов. Подробно: `notes/topics/41-server-side-client-diagnostic-struct.md`.

### 42. Voice messages — server-controlled OPUS encoder

PmsKey `opus-recorder` (#308), `opus-recorder-bitrate` (#309), `opus-recorder-sample-rate` (#310) — сервер задаёт encoder (OPUS vs MediaRecorder/AAC), bitrate и sample rate голосовых сообщений. `audio-transcription-locales` (#12) — whitelist языков для транскрипции. Полный server-controlled voice pipeline: запись → параметры → транскрипция. Подробно: `notes/topics/42-voice-messages-opus-server-controlled.md`.

### 43. libjingle_peerconnection_so.so — кастомный WebRTC-форк VK/OK

4 нестандартных feature flag: `WebRTC-VK-OpusMaxPlcDurationMs`, `WebRTC-OK-StunCustomAttr` (нестандартный STUN-атрибут — идентификатор клиента в UDP-трафике), `WebRTC-OK-TurnChannelDataMark`, `WebRTC-OK-FrameDropper-Alt`. 14 `CallsSDK-*` флагов, включая `CallsSdk-LogAudioCapture` и `Calls-SDK-LogDCTraffic`. Opus 1.5 с AI BWE и DRED. Подробно: `notes/topics/43-libjingle-webrtc-custom-build.md`.

### 44. Informer banners, FakeBoss, LiveStreams

`informer_banner` — Room-таблица с server-pushed баннерами (поля: `id`, `title`, `description`, `url`, `priority`, `repeat`, `show_count`). Баннеры с `url` — server-pushed deeplink'и в UI. Телеметрия каждого взаимодействия. `FakeBoss` — server-gated фича «защита от мошенников-начальников»; при включении сервер узнаёт `contactServerId` и `phoneNumber` «начальника» пользователя. `LiveStream` — Protobuf-объект с server-pushed URL-prefix стриминг-сервера. Подробно: `notes/topics/44-informer-banners-fakeboss-livestreams.md`.

### 45. Calls SDK — 30+ server-controlled PmsKey

`calls-sdk-*` (12 ключей) и `calls-android-*` (20 ключей) управляют каждым аспектом звонкового стека. Ключевые: `calls-sdk-log-audio` (#131) — server-gated аудио-логирование; `calls-android-signaling-ip` (#119) — server-pushed IP сигналинг-сервера; `calls-sdk-wt-enabled` (#102) — переключение на WebTransport/QUIC; `calls-sdk-disable-pipeline` (#130) — отключить весь APM. Подробно: `notes/topics/45-calls-sdk-pmskey-cluster.md`.

### 46. Server-controlled logging flags

`log-full`, `log-sensitive`, `log-messages-meta`, `log-chat-meta` — server-gated PmsKey. При `log-full=true` и `log-sensitive=false` в logcat попадают контакты и токен аутентификации (из `ul9.java` LOGIN.Response handler). `allowLogSensitiveData` — дополнительный локальный dev-флаг в SharedPreferences. Подробно: `notes/topics/46-server-controlled-logging-flags.md`.

### 47. Стикеры — server-controlled pipeline

9 PmsKey управляют стикерами: размеры, лимиты, `stickers-botid` (#272) — ID бота-создателя, `welcome-sticker-ids` (#81) — стикеры для новых пользователей. `STICKER_SUGGEST(194)` — контекст набираемого текста/emoji уходит на сервер для получения предложений. Подробно: `notes/topics/47-stickers-server-pipeline.md`.

### 48. Поиск — WS-опкоды и server-controlled лимиты

6 WS-опкодов: `CONTACT_SEARCH(37)`, `PUBLIC_SEARCH(60)`, `CHAT_SEARCH(68)`, `MSG_SEARCH_TOUCH(72)` (аналитика клика по результату), `MSG_SEARCH(73)`, `CHAT_SEARCH_COMMON_PARTICIPANTS(198)`. `pub-search-limit` (#291) — server-controlled лимит результатов. `MSG_SEARCH_TOUCH` — сервер знает не только что искали, но и что выбрали. `CHAT_SEARCH_COMMON_PARTICIPANTS` — потенциальный инструмент деанонимизации участников чатов. Подробно: `notes/topics/48-search-opcodes-server-controlled.md`.

---

## Дополнения 49-54

### 49. Опросы — server-gated по типу чата

5 PmsKey: `render-polls` (bool, default=false), `polls-in-p2p-chats` (bool), `polls-in-p2g-chats` (int — лимит участников), `polls-in-channels` (int), `poll-ttl` (JSON). Все default=false/0. `GET_POLL_UPDATES(306)` — сервер получает все голоса. Подробно: `notes/topics/49-polls-server-gated.md`.

### 50. Реакции и отложенные сообщения

7 WS-опкодов реакций: `MSG_REACTION(178)`, `MSG_CANCEL_REACTION(179)`, `MSG_GET_REACTIONS(180)`, `MSG_GET_DETAILED_REACTIONS(181)` (кто именно поставил), `CHAT_REACTIONS_SETTINGS_SET(257)` (per-chat политика реакций), `REACTIONS_SETTINGS_GET_BY_CHAT_ID(258)`, `NOTIF_MSG_REACTIONS_CHANGED(155)`. Отложенные сообщения: 3 PmsKey (`scheduled-messages-enabled` #48, `scheduled-posts-enabled` #49, `scheduled-faves-enabled` #50) — хранятся на сервере до отправки. Подробно: `notes/topics/50-reactions-scheduled-messages.md`.

### 51. Организации, официальные аккаунты, денежные переводы

`org-profile` (#221) — «Плашка представителя организации в профиле» (server-gated). `official-org` (#212) — верификация организаций без прозрачных критериев. `money-transfer-botid` (#81) — денежные переводы через бот-аккаунт (сервер видит все транзакции). `comments-enabled` — server-gated комментарии. Подробно: `notes/topics/51-organizations-money-transfer.md`.

### 52. Network session PmsKey — server-controlled TLS validation

`net-ssl-session-validate` (default=true) — сервер может отключить TLS-валидацию через PmsKey. `net-session-rbc-enabled` — «Reduce battery consumption». `net-client-dns-enabled` — кастомный DNS-клиент. `watchdog-config` — server-pushed конфиг watchdog-а. Подробно: `notes/topics/52-network-session-tls-flags.md`.

### 53. ab-status и cfs

`ab-status` — long-идентификатор A/B-группы пользователя, отправляемый на сервер при каждом подключении. `cfs` (default=true) — «Быстрый старт через клиентский бэкенд» для P2P-звонков. Подробно: `notes/topics/53-ab-status-cfs-pmskey.md`.

### 54. WS session config fingerprint — 21 поле при каждом подключении

`ubi.java` формирует 21-поле диагностический fingerprint, отправляемый серверу при каждом WS-подключении: `net-ssl-session-validate`, `ab-status`, `calls-sdk-log-audio`, `db-tr-ex-count`, `db-query-ex-count`, `enable-audio-messages-transcription`, `enable-video-messages-transcription` и др. Сервер получает полный snapshot конфигурации клиента при каждом подключении. Подробно: `notes/topics/54-ws-session-config-fingerprint.md`.

---

## Дополнения 55-60

### 55. 2FA, Stories, ilm, blocked-users

2FA-flow в `one.me.settings.twofa.creation` с `creation_2fa_track_id_key` — tracking ID для аналитики каждого шага настройки 2FA. `story` PmsKey — server-gated Stories (default=false). `ilm` (default=true) — «Отключить инвалидацию последних сообщений при смене локали». `blocked-users` — «Уведомление о заблокированных пользователях». Подробно: `notes/topics/55-2fa-stories-misc-pmskey.md`.

### 56. perf-events — server-configurable performance telemetry

`perf-events` — JSON-конфиг событий производительности для сбора. `nei.java` отправляет POST на `https://sdk-api.apptracer.ru/api/perf/upload?crashToken=<token>` с payload `{samples: [{name, value, unit, attributes}]}`. Сервер контролирует, какие именно метрики собираются. Ответ сервера — `PERFORMANCE_METRICS` JSON. Подробно: `notes/topics/56-perf-events-apptracer-upload.md`.

### 57. devnull, net-stat-config, opcode-stat-config

`devnull` — `DevNullServerConfig(events=...)` — server-pushed blacklist событий аналитики для игнорирования. `net-stat-config` — конфиг сетевой статистики. `opcode-stat-config` — конфиг статистики WS-опкодов. Все три позволяют серверу точно управлять тем, что клиент собирает. Подробно: `notes/topics/57-devnull-telemetry-filter.md`.

### 58. Non-contact sync — граф взаимодействий за пределами телефонной книги

`myb.java` — воркер синхронизации не-контактов (пользователей, с которыми было взаимодействие, но которых нет в телефонной книге). Батчи по `non-contact-max-chunk-size` (default 10). PmsKey: `non-contact-sync-time`, `non-contact-max-chunk-size`, `non-contact-collection-interval`. Сервер получает полный граф общения пользователя, включая людей вне телефонной книги. Подробно: `notes/topics/58-non-contact-sync.md`.

### 59. keep-background-socket + ping-background-interval

`keep-background-socket` — server-controlled постоянное WS-соединение в фоне. `ping-background-interval` — интервал ping-пакетов когда приложение не интерактивно. `jn0.java` — обработчик изменения PmsKey. `hod.java` — планировщик ping. Сервер может держать MAX постоянно подключённым и пингующим, обеспечивая постоянный канал и presence-сигнал. Подробно: `notes/topics/59-keep-background-socket.md`.

### 60. UserSettings — полная карта 26+ server-pushed полей

`sgj.java` — модель UserSettings, приходящая с сервера при каждом подключении. Ключевые поля: `hiddenOnline`, `safeMode`/`safeModeNoPin` (server-pushed ограничение функциональности), `searchByPhone` (приватность номера), `audioTranscriptionEnabled` (второй канал управления транскрипцией), `contentLevelAccess`, `familyProtection`, `phoneNumberPrivacy`, `inactiveTtl`. Все 26+ полей server-pushed, не только user-controlled. Подробно: `notes/topics/60-user-settings-full-map.md`.

---

## Дополнения 61-64

### 61. LeakCanary в production + gostLicenseCheckEnabled + isDisableWebAppSsl

`ri9.leakCanaryEnabledStateFlow` — LeakCanary (библиотека heap dump) включён в production APK и активируется через SharedPreferences. Heap dump содержит все объекты в памяти включая токены и сообщения. `gostLicenseCheckEnabled` — локальный флаг GOST-проверки. `gostEnvironmentCheckFlags` — server-pushed битовая маска GOST-проверок. `areMockCommentsEnabled` — debug-флаг в production. `isDisableWebAppSsl` — dev-флаг отключения SSL для мини-апок. Подробно: `notes/topics/61-leakcanary-gost-debug-flags.md`.

### 62. white-list-links + show-warning-links — серверный контроль ссылок

`white-list-links` (#314) — server-pushed список «доверенных» доменов. При клике на ссылку клиент проверяет hostname против этого списка. Сервер контролирует, какие внешние ресурсы открываются без предупреждения. `show-warning-links` — server-gated включение предупреждений о внешних ссылках. Подробно: `notes/topics/62-white-list-links-content-control.md`.

### 63. Жалобы — server-controlled список причин

`available-complaints` (#13) — server-pushed список причин жалобы (enum-строки). Клиент показывает только те причины, которые разрешены сервером. `server-side-complains-enabled` (#266) — переключение на серверный flow жалоб. `complainReasonsSync` — периодическая синхронизация причин. Сервер может убрать «политический контент» из списка причин жалоб. Подробно: `notes/topics/63-complaints-server-controlled.md`.

### 64. calls-endpoint — server-pushed URL звонкового сервера

`calls-endpoint` — server-pushed строка с URL(ами) звонкового сервера. В сочетании с `calls-android-signaling-ip` (#119) — полный server-control над маршрутизацией звонкового трафика. Сервер может перенаправить все звонки на произвольный endpoint без обновления клиента. Подробно: `notes/topics/64-calls-endpoint-server-redirect.md`.

---

## Дополнения 65-67

### 65. In-App Review — server-controlled triggers + FakeInAppReviewBottomSheet

`in-app-review-triggers` — server-pushed битовая маска условий показа запроса оценки. `FakeInAppReviewBottomSheet` — production-ready компонент, имитирующий Google Play In-App Review UI. При `fake-in-app-review=true` пользователь видит поддельный диалог оценки, оценки уходят на серверы MAX, а не в Google Play. Подробно: `notes/topics/65-in-app-review-fake.md`.

### 66. utm-tag-for-trigger-link-share — автоматический UTM-трекинг

При включённом PmsKey клиент автоматически добавляет `utm_source=trigger` ко всем ссылкам, которыми пользователь делится из MAX. Внешние сайты видят этот маркер. Дополнительно: `client=613` hardcoded в URL-параметрах для ссылок на профили. Подробно: `notes/topics/66-utm-tag-link-tracking.md`.

### 67. yag SharedPreferences — per-account состояние

60+ полей: `currentProxyList`/`pushProxyList` (прокси-серверы), `okToken` (токен аутентификации), `deviceId`, `installationMarket` (откуда установлено). Три debug-флага в production: `isDebugHostRotationEnabled`, `isDebugUaDnsEmulationEnabled`, `isIceCandidateEmulationEnabled`. Подробно: `notes/topics/67-yag-shared-prefs-per-account.md`.

---

## Дополнения 68-70

### 68. cis-enabled + multi-lang

`cis-enabled` (#81) — server-gated «UI улучшения для СНГ стран» (из `pl5.java`). Сервер знает страну пользователя и включает специфический UI. `multi-lang` — server-gated мультиязычность. Подробно: `notes/topics/68-cis-enabled-multilang.md`.

### 69. RemoteSettings calls SDK — server-pushed конфиги звонков

`android.dump.bitrate` — server-gated запись bitrate dump в файл `target_bitrate_dump_<timestamp>` во время звонков. `android.rating.limits` — пороги качества звонка. `android.p2prelay.config` — конфиг P2P relay. `android.wordspotter.config` — конфиг KWS (см. topic 70). Все управляются через `RemoteSettings` интерфейс. Подробно: `notes/topics/69-remote-settings-calls-sdk.md`.

### 70. KeywordSpotter в звонках — server-controlled turnOffInMs

KWS активен во время звонков. `android.wordspotter.config` задаёт `turnOffInMs` — через сколько выключить KWS. При `null` — KWS работает всё время звонка. `NativeDoubleArrayConsumer.Consumer` — callback из нативного KWS-pipeline в Java. `ConversationKwsStat` — статистика срабатываний KWS отправляется на сервер. Модель версии `ws_0` загружается по ключу `android.mlfeatures.ws_0`. Подробно: `notes/topics/70-keyword-spotter-in-calls.md`.

---

## Дополнения 71

### 71. CALL OneLog события — детальная телеметрия звонков

12+ операций: `INCOMING_CALL_RECEIVED`, `INCOMING_CALL_INIT`, `CALL_RECEIVED_ACCEPT`, `CALL_REMOTE_RINGING`, `START_CALL`, `FINISH_CALL` (с duration/error/is_group), `GROUP_CALL_JOIN`/`GROUP_CALL_JOIN_FAILED`, `SHARE_CALL_LINK`, `ADMIN_CALL_SETTINGS`, `ADMIN_CALL_SETTINGS_TO_USER` (с user_id2), `UNKNOWN_CALLER_ALERT`. Атрибуты: `call_id`, `user_id2`, `camera`, `microphone`, `screenshare`, `recording`, `con_state`. Сервер получает полный лог всех звонков с метаданными. Подробно: `notes/topics/71-call-onelog-events.md`.

---

## Дополнения 72-76

### 72. VIDEO_MESSAGE OneLog

4 операции: `video_message_start_recording`, `video_message_delete`, `video_message_hands_free_mode_on`, `video_message_error`. Атрибуты: `source_id` (chat_id), `message_id`, `local_message_id`. Сервер знает каждое начало записи видеосообщения с chat_id. Подробно: `notes/topics/72-video-message-onelog.md`.

### 73. PERMISSION OneLog — ежедневный отчёт

`DailyAnalyticsWorker` ежедневно отправляет статус 7 разрешений: push, contacts, fsi, gallery (allowed/partial/denied), camera, microphone, geo. `permission_changed_state` — real-time при изменении. Сервер ведёт исторический профиль разрешений. Подробно: `notes/topics/73-permission-onelog-daily.md`.

### 74. BACKGROUND_MODE OneLog

7 операций: `system_curtain_shown/hidden` (открытие notification shade), `work_in_background_permission`, `snack_shown/hidden/click_on`, `carpet_mode_on`. Сервер знает, когда пользователь открывает notification shade и взаимодействует с предложением фонового режима. Подробно: `notes/topics/74-background-mode-onelog.md`.

### 75. SETTINGS + POWER_SAVING OneLog

`MINIAPP_BIOMETRY` с `webappId` — сервер знает, для каких мини-апок включена биометрия. `THEME`/`BACKGROUND`/`TEXT_SIZE` — настройки внешнего вида. `POWER_SAVING`: `show_shade/click_shade_button/close_shade`. Таблица `webapp_biometry` в Room DB. Подробно: `notes/topics/75-settings-power-saving-onelog.md`.

### 76. CONTACT_OR_BLOCK OneLog

`showed` и `clicked` (clickType: `to_contacts`/`block`/`close`) — сервер знает каждое решение пользователя добавить в контакты или заблокировать через infobar UI. В сочетании с синхронизацией контактов — полная картина социального графа. Подробно: `notes/topics/76-contact-or-block-onelog.md`.

---

## Дополнения 77-79

### 77. SHARE_TO_MAX + INVITE_MAX_BANNER OneLog

`SHARE_TO_MAX` — при share intent из внешних приложений сервер получает `chatsInfo` — список всех чатов, в которые был отправлен контент, с типами (DIALOG/PRIVATE_CHANNEL/PUBLIC_CHAT/etc) и ID. `INVITE_MAX_BANNER` — `show`/`click_link` с `session_id`, `screen`, `entryPoint`, `linkType`. Подробно: `notes/topics/77-share-to-max-invite-onelog.md`.

### 78. PUSH OneLog — 12+ операций

`show` (с `chat_id`), `drop` (с причиной), `delivered`, `open_chat`/`open_url` (с `trid`, `eKey`), `m_as_read` (отметка из notification shade), `n_q_rep` (быстрый ответ), `n_canceled`. Сервер получает полный лог взаимодействия с push-уведомлениями. Подробно: `notes/topics/78-push-onelog-events.md`.

### 79. Дополнительные OneLog категории

`AUDIO_TRANSCRIPTION.transcription_result` — `message_id`, `media_id`, `duration`, `waiting_time`, `source_id` (chat_id). `AUTH_QR.LOG` — полный лог QR-аутентификации (6 шагов). `CHANNEL_RECSYS_FOLDER` — взаимодействие с рекомендательными папками. `CONTACT_RENAME_BANNER` — переименование контактов. `PRESENCE.EVENT_MESSAGE_COUNTER`. Подробно: `notes/topics/79-additional-onelog-categories.md`.

---

## Дополнения 80-81

### 80. ACTION/CLICK/SHOW OneLog

`GET_INSTALL_REFERRER` — при первой установке сервер получает источник установки (referrer string, is_update_version). `FCM_ON_DELETED_MESSAGES` — FCM удалил сообщения. `ACTION_CACHE_CLEARED` — очищен кэш. `CLICK.profile_button_click` — нажатие на профиль с `source_meta`. `SHOW.SEARCH_RESPONSE` — показан результат поиска. Подробно: `notes/topics/80-action-click-show-onelog.md`.

### 81. DANGEROUS_URL_ACTIONS, DANGEROUS_FILE_ACTIONS, REGISTRATION, INFORMER, BANNER

`DANGEROUS_URL_ACTIONS` — сервер знает, когда пользователю показывалось предупреждение об опасной ссылке и перешёл ли он по ней (`go`/`close`). `DANGEROUS_FILE_ACTIONS` — показ предупреждения об опасном файле с `source_id` (chat_id), `download_file`/`not_download_file`. `REGISTRATION` — детальный лог регистрации. `INFORMER` — взаимодействие с informer-баннерами. Подробно: `notes/topics/81-dangerous-actions-registration-onelog.md`.

---

## Дополнения 82

### 82. MESSAGE_CLICKABLE_ELEMENT_ACTIONS + CHAT_PROFILE_CLICKABLE_ELEMENT_ACTIONS

`clicked_open_link` с `message_id` и `source_id` — сервер знает, на какую ссылку в каком сообщении в каком чате нажал пользователь. `clicked_call` — нажатие на номер телефона в сообщении. `clicked_copy` — копирование текста. `CHAT_PROFILE_CLICKABLE_ELEMENT_ACTIONS` — клики в профиле чата. Подробно: `notes/topics/82-message-clickable-element-onelog.md`.

---

## Дополнения 83

### 83. ok9.f() — geolocation_send_click, search_click, inline_button_click

`geolocation_send_click` — сервер знает каждое нажатие кнопки отправки геолокации. `search_click` — клик на результат поиска. `inline_button_click` — нажатие inline-кнопок ботов. Подробно: `notes/topics/83-ok9f-geolocation-search-inline.md`.

---

## Дополнения 84

### 84. TRANSCRIBE_MEDIA(202) — серверная транскрипция

WS-опкод 202. Ответ: `transcription` (текст) + `transcriptionStatus` (byte). Содержимое голосовых/видеосообщений уходит на серверы MAX для транскрипции. Server-gated через `enable-audio-messages-transcription`/`enable-video-messages-transcription` PmsKey + `audio-transcription-locales` whitelist. Подробно: `notes/topics/84-transcribe-media-ws-opcode.md`.

---

## Дополнения 85

### 85. NOTIF_* WS-опкоды — 25 server-to-client push

`NOTIF_CONFIG(134)` — real-time PmsKey update без перезапуска. `NOTIF_LOCATION_REQUEST(148)` — второй канал запроса геолокации (помимо FCM). `NOTIF_DRAFT(152)/NOTIF_DRAFT_DISCARD(153)` — черновики синхронизируются с сервером в реальном времени. `NOTIF_BANNERS(292)` — server-pushed баннеры по WS. `NOTIF_TRANSCRIPTION(293)` — результат транскрипции. Подробно: `notes/topics/85-notif-ws-opcodes.md`.

---

## Дополнения 86

### 86. NeuroAvatars — AI-генерация аватаров

Модуль `one.me.login.neuroavatars`: `NeuroAvatarsScreen`, `RegistrationNeuroAvatarsScreen`, `NeuroAvatarPickerBottomSheet`. При регистрации и в настройках пользователю предлагается нейро-аватар. Фотография пользователя вероятно отправляется на серверы MAX для AI-обработки. Подробно: `notes/topics/86-neuroavatars-ai.md`.

---

## Дополнения 87

### 87. LOCATION WS-опкоды — live location и запросы геолокации

`LOCATION_SEND(125)`, `LOCATION_REQUEST(126)`, `LOCATION_STOP(124)`, `NOTIF_LOCATION(147)`, `NOTIF_LOCATION_REQUEST(148)`. `liveLocation=true` — непрерывный поток координат. `NOTIF_LOCATION_REQUEST` — сервер может запросить геолокацию по WS (второй канал помимо FCM). Координаты уходят на MAX-сервер (WS) и Yandex (HTTP) независимо. Подробно: `notes/topics/87-location-ws-opcodes.md`.

---

## Дополнения 88

### 88. Call Rate — оценка качества звонков

`CallRateBottomSheet` с `callId`, `isGroupCall`, `isVideoCall`, `sdkReasons` (технические причины плохого качества от WebRTC SDK). Сервер получает оценку + технические причины. Условия показа server-controlled через `call-rate` PmsKey и `android.rating.limits`. Подробно: `notes/topics/88-call-rate-quality-rating.md`.

---

## Дополнения 89

### 89. video-transcoding-class + one-video-uploader-config

`video-transcoding-class` (#302) — JSON-массив `[1,2,3]` (low/average/high), default `[2,3]`. Сервер контролирует доступные классы качества видео. `one-video-uploader-config` — JSON `{"audio": 0|1|2, "video": 0|1|2}` — режим загрузчика видео. Подробно: `notes/topics/89-video-transcoding-config.md`.

---

## Дополнения 90

### 90. Media upload WS-опкоды + speedy-upload

`PHOTO_UPLOAD(80)`, `STICKER_UPLOAD(81)`, `VIDEO_UPLOAD(82)`, `FILE_UPLOAD(87)`. `speedy-upload` (#275) — «Фейк прогресс для загрузки видео» (server-gated). `file-upload-unsupported-types` (#36) — server-controlled content-фильтрация типов файлов. `file-upload-max-size` (#35) — server-controlled лимит размера. Подробно: `notes/topics/90-media-upload-opcodes.md`.

---

## Дополнения 91

### 91. Дополнительные WS-опкоды

`DEBUG(2)` — debug-команда от сервера. `LOG(5)` — клиент отправляет логи на сервер по WS (второй канал помимо apptracer). `LINK_INFO(89)` — при вставке ссылки клиент запрашивает preview у сервера — сервер видит все ссылки, которые пользователь вставляет в сообщения до отправки. `DRAFT_SAVE(176)` — черновики сохраняются на сервере. `SESSIONS_INFO(96)/SESSIONS_CLOSE(97)` — управление активными сессиями. Подробно: `notes/topics/91-additional-ws-opcodes.md`.

---

## Дополнения 92

### 92. WEB_APP_INIT_DATA, VOTERS_LIST_BY_ANSWER, AUDIO_PLAY WS-опкоды

`WEB_APP_INIT_DATA(160)` — сервер знает о каждом запуске мини-апки. `VOTERS_LIST_BY_ANSWER(305)` — голосование не анонимное, сервер знает кто как проголосовал. `AUDIO_PLAY(301)` — сервер знает, когда пользователь прослушивает голосовые сообщения. 5 опкодов для папок (FOLDERS_GET/UPDATE/REORDER/DELETE). `COMPLAIN/COMPLAIN_REASONS_GET` — жалобы через WS. Подробно: `notes/topics/92-webapp-complaints-folders-polls-ws.md`.

---

## Дополнения 93

### 93. client-conv-id + send-queue-size

`client-conv-id` (#82) — «Включить клиентское создание conversations id» (server-gated P2P conversation ID generation). `send-queue-size` (#265) — server-controlled лимит очереди отправки сообщений (default 30). Подробно: `notes/topics/93-client-conv-id-send-queue.md`.

---

## Дополнения 94

### 94. Battery и Memory мониторинг

`battery-slice-interval` (#15, default 1 мин) — server-controlled интервал сбора срезов батареи. Room-таблица `battery` с `sliceTime`, `payload`. `battery_level_change` отправляется как часть статистики звонков. `memory-slice-interval` (#181) — аналогичный мониторинг памяти. Сервер имеет детальный профиль энергопотребления устройства. Подробно: `notes/topics/94-battery-memory-monitoring.md`.

---

## Дополнения 95

### 95. Calls stats — 40 метрик после каждого звонка

`w32.java` — 40 WebRTC-метрик: CPU (загрузка, score, ядра), Memory (max/avg MB), BatteryLevelChange, AudioLevel (уровень аудио — сервер знает когда пользователь говорит), аудио-качество (jitter/loss/concealment), видео-качество (frames/freezes/bitrate). Полный технический профиль каждого звонка. Подробно: `notes/topics/95-calls-stats-40-metrics.md`.

---

## Дополнения 96

### 96. vchat.* API — 14 методов звонкового API

`vchat.clientStats` — HTTP API для 40 метрик звонка. `vchat.getLogUploadUrl` — сервер выдаёт URL для загрузки логов звонка. `vchat.clientSupportedCodecs` — клиент сообщает поддерживаемые кодеки. `vchat.getExternalIdsByOkIds/getOkIdsByExternalIds` — маппинг OK ID ↔ внешние ID. Статистика через OneLog-инфраструктуру. Подробно: `notes/topics/96-vchat-api-methods.md`.

---

## Дополнения 97

### 97. api.oneme.ru — основной API endpoint и тестовые серверы

`api.oneme.ru:443` — production. Dev-меню содержит: `api-test.oneme.ru`, `api-tg.oneme.ru` (неизвестное назначение), `api-test2.oneme.ru`. Deeplink-обработчик принимает тестовые хосты. `api-tg.oneme.ru` — суффикс `tg` требует дополнительного анализа. Подробно: `notes/topics/97-api-oneme-ru-endpoints.md`.

---

## Дополнения 98

### 98. MyTracker API детали

`tracker-api.vk-analytics.ru` с путями: `v3/`, `mobile/v1`, `mlapi` (антифрод ML), `beta-ml`. Attribution через `mt_deeplink` параметр. Поддерживает proxy-хост. ML-endpoints для сенсорного fingerprint (5 датчиков из topic 35). Подробно: `notes/topics/98-mytracker-api-details.md`.

---

## Дополнения 99

### 99. MyTracker PreInstall — OEM preinstall detection

`PreInstallHandler` читает ресурсы вендорного приложения через `PackageManager` и системные свойства `ro.mytracker.preinstall.path` / `ro.appsflyer.preinstall.path`. Определяет, было ли MAX предустановлено производителем. Referrer отправляется на `tracker-api.vk-analytics.ru`. Однократная проверка (`preinstallRead` flag). Подробно: `notes/topics/99-mytracker-preinstall-oem.md`.

---

## Дополнения 100

### 100. call-chat-members-load-config

`ChatMembersLoadConfig` с полями: `newLoadingContactsLogicEnabled` (bool), `maxLoadCount` (int — сервер контролирует, сколько участников группового звонка видит пользователь), `minInCall` (int). Подробно: `notes/topics/100-call-chat-members-load-config.md`.

---

## Дополнения 101

### 101. anr-config + watchdog-config

`anr-config` (#85) — JSON `{"enabled": bool, "timeout": {"low": ms, "avg": ms, "high": ms}}`. Сервер контролирует ANR-детектор: включён ли и при каком таймауте срабатывает (три уровня по классу устройства). `watchdog-config` (#305) — аналогичный конфиг для watchdog-а. Подробно: `notes/topics/101-anr-watchdog-config.md`.

---

## Дополнения 102-103

### 102. conn-timeouts

`conn-timeouts` (#84) — JSON-объект с таймаутами для разных типов соединений (EnumMap, default 10000ms). Сервер контролирует, как долго клиент ждёт ответа. Подробно: `notes/topics/102-conn-timeouts.md`.

### 103. media-order

`media-order` (#174) — 4 режима: `0`=медиа снизу, `1`=медиа сверху, `2`=медиа сверху только в каналах, `3`=**порядок управляется с бека**. При `3` — сервер динамически контролирует layout сообщений. Подробно: `notes/topics/103-media-order.md`.

---

## Дополнения 104-105

### 104. bad-networ-indicator-config + video-msg-config

`bad-networ-indicator-config` (#14) — JSON с алгоритмом детекции плохой сети: RTT и loss пороги/веса. Сервер контролирует, когда показывать индикатор плохой сети. `video-msg-config` (#297) — конфиг видеосообщений. Подробно: `notes/topics/104-bad-network-video-msg-config.md`.

### 105. chat-history-warm-opts

`chat-history-warm-opts` — int, default `-1` (=All). Сервер контролирует прогрев истории чатов при запуске. Связанные: `chat-history-warm-fail-interval`, `chat-history-notif-msg-strategy`, `chat-history-persist`, `chat-history-login-count`. Все отправляются в WS session config. Подробно: `notes/topics/105-chat-history-warm.md`.

---

## Дополнения 106

### 106. perf-registrar-config + net-stat-config

`PerfRegistrarServerSettings`: `maxAttemptsForPersistentMetric=25`, `rawPersistInterval=15s`, TTL=3 дня. `NetStatConfig(loggableOpcodes={17,18})` — сервер задаёт, для каких WS-опкодов собирать сетевую статистику. Подробно: `notes/topics/106-perf-registrar-net-stat-config.md`.

---

## Дополнения 107

### 107. opcode-stat-config + session.opcode_stats

`opcode-stat-config` (#216) — конфиг сбора статистики WS-опкодов. `session.opcode_stats` — per-session статистика: `opcodeStats`, `anrDetected`, `caughtExceptionCount`, `phonebookSize`, `frescoStats`, `appClockDump`. Сервер знает о нестабильности клиента (ANR, исключения). Подробно: `notes/topics/107-opcode-stat-session-stats.md`.

---

## Дополнения 108

### 108. fresco-executor + system-thread-pool-queue

`fresco-executor` (#122) — «Enable Fresco executor-hack» (server-gated). `system-thread-pool-queue` (#286) — «Disable LinkedTransferQueue34» (server-gated). Оба — performance-хаки для конкретных устройств/версий Android. Подробно: `notes/topics/108-fresco-executor-thread-pool.md`.

---

## Дополнения 109

### 109. Presence PmsKey — полная карта

9 presence-PmsKey: `presence-ttl=300s`, `presence-view-port` (viewport optimization), `presence-external` (запрос presence незнакомцев), `presence-seen-eq` (не-legacy сравнение), `presence-stat` (статистика), `presence-keep-bg-cache`, `notif-typing-presence`, `presence-offline-move-timeout`, `presence-offline-log`. Подробно: `notes/topics/109-presence-pmskey-full.md`.

---

## Дополнения 110

### 110. Audio playback PmsKey

`audio-play-opus` (default=false), `audio-download` (default=false), `audio-download-fallback` (default=**true**), `audio-prefetch` (default=false), `audio-play-cache-ttl`, `audio-peaks-count`, `calc-audio-wave`. Загрузка аудио выключена по умолчанию — сервер включает. Подробно: `notes/topics/110-audio-playback-pmskey.md`.

---

## Дополнения 111

### 111. invite-friends-sheet-frequency

`invite-friends-sheet-frequency` (#143) — server-pushed массив интервалов в днях для показа листа «пригласить друзей». Сервер контролирует growth-механизм. `alreadyInvitedFriends` в `yag.java` — сервер знает, приглашал ли пользователь кого-то. Подробно: `notes/topics/111-invite-friends-frequency.md`.

---

## Дополнения 112

### 112. user-debug-report + userLogReportChatId

`user-debug-report` (#292) — server-pushed режим debug-репортирования. `userLogReportChatId` — **ID чата, в который отправляются логи пользователя** (сервер может направить логи в конкретный чат через WS). `debug-mode` (#3) — server-pushed режим отладки. Подробно: `notes/topics/112-user-debug-report-chat.md`.

---

## Дополнения 113

### 113. invalidate-db-force — серверная инвалидация БД

`invalidate-db-force` (#141) — сервер может принудительно инвалидировать (удалить) локальную БД клиента через версионирование (`configVer > curVer`). Это означает, что **сервер может удалить всю локальную историю сообщений**. `invalidate-db-msg-exception` (#142) — инвалидация при ошибке обработки сообщений. Подробно: `notes/topics/113-invalidate-db-force.md`.

---

## Дополнения 114

### 114. views-count-enabled

`views-count-enabled` (#303) — «Включить просмотры в каналах» (server-gated). При включении сервер знает, какие посты в каналах просматривал пользователь. Подробно: `notes/topics/114-views-count-enabled.md`.

---

## Дополнения 115

### 115. quotes-enabled + media-playlist-enabled

`quotes-enabled` (#247) — «Отображение и отправка markdown quote элемента» (default=false). `media-playlist-enabled` (#175) — медиа-плейлист (default=false). Оба server-gated. Подробно: `notes/topics/115-quotes-media-playlist.md`.

---

## Дополнения 116

### 116. join-requests

`join-requests` (#148) — server-gated функция заявок на вступление в закрытые чаты/каналы. `JoinRequestsScreen` с `chatId`. Подробно: `notes/topics/116-join-requests.md`.

---

## Дополнения 117-118

### 117. Call UI PmsKey

`horizontal-call-mode` (#130, default=false) — горизонтальный UI звонка. `hide-incoming-call-notif` (#129, default=false) — скрытие уведомления при входящем. `ringtone-player-focus` (#258) — 3 режима аудио-фокуса. `calls-android-ssttl` (#117) — TTL общих настроек звонка. Подробно: `notes/topics/117-call-ui-pmskey.md`.

### 118. Media UX PmsKey

`speedy-voice-messages` (#276, default=false) — «Лоудер на отправку голосовых». `open-video-from-start` (#217, default=false). `new-media-upload-ui` (#204, default=false). `new-media-edit-screen` (#203, default=false). `video-fast-seek-enabled` (#296, default=false). `video-speed` (#301). Подробно: `notes/topics/118-media-ux-pmskey.md`.

---

## Дополнения 119-120

### 119. Folders PmsKey

`folders-max-count` (#121, default=30) — максимальное количество папок. `enable-filters-for-folders` (#106, default=false) — фильтры в папках. `channels-suggests-folder` (#69) — папка рекомендованных каналов. Подробно: `notes/topics/119-folders-pmskey.md`.

### 120. Multi-select и messages meta

`multi-select-bars-redesign` (#193, default=false) — «Мультиселект с действиями в боттом баре». `chats-multi-select` (#78, default=false). `log-messages-meta` (#155, default=false) — «Сбор meta info видимых сообщений по клику». Подробно: `notes/topics/120-multiselect-messages-meta.md`.

---

## Дополнения 121

### 121. Notifications и bots PmsKey

`cancel-stale-notifications` (#64, default=false) — «Отмена устаревших нотификаций в notifyAllChats» (server-gated bugfix). `new-intent-fix` (#202, default=false) — «onNewIntent NPE fix» (server-gated bugfix). `bots-channel-adding` (#19, default=false) — добавление ботов в каналы. `bot-start-param` (#18), `bot-complaint-enabled` (#17). Подробно: `notes/topics/121-notifications-bots-bugfix-pmskey.md`.

---

## Дополнения 122

### 122. VPN UI PmsKey — три уровня предупреждений

`show-vpn-snackbar` (#273, default=false), `show-vpn-chat-bottomsheet` (#272, int-режимы), `show-vpn-call-bottomsheet` (#271, int-режимы). Три независимых механизма VPN-предупреждений. Сервер контролирует интенсивность давления на пользователей с VPN. Подробно: `notes/topics/122-vpn-ui-pmskey.md`.

---

## Дополнения 123

### 123. Group calls PmsKey

`gc-from-p2p` (#123, default=false) — «Переход из 1-1 звонка в групповой». `gc-link-pre-settings` (#124, default=false) — «Преднастройки группового звонка по ссылке». `gc-wait-admin` (#125, default=false) — «Комната ожидания администратора». `group-call-part-limit` (#128) — лимит участников. `call-pinch-to-zoom` (#24, default=false). Подробно: `notes/topics/123-group-calls-pmskey.md`.

---

## Дополнения 124

### 124. Calls signaling PmsKey

`outgoing-call-uri` (#223, default=`https://max.ru`) — URI для Android Telecom API. `calls-android-signaling-to` (#124) — JSON `{"use": bool, "cto": 5000, "ird": 2000, "rdsf": 1.0, "mrd": 2000}` — таймауты сигналинга. `early-call-start` (#101, default=false) — «Ранний старт входящего звонка». Подробно: `notes/topics/124-calls-signaling-pmskey.md`.

---

## Дополнения 125

### 125. P2P relay и WebTransport PmsKey

`calls-use-p2p-relay` (#61, default=false) — «Использовать p2p relay» — сервер может перенаправить весь медиа-трафик звонков через relay-серверы. `calls-use-p2p-relay-caps` (#62, default=false) — учитывать capability. `calls-sdk-wt-enabled` (#102, default=false) — WebTransport. Подробно: `notes/topics/125-p2p-relay-webtransport.md`.

---

## Дополнения 126

### 126. Energy saving PmsKey

`energy-saving-bottom-sheet` (#109, default=false) — «Включение шторки энергосбережения» — server-gated bottomsheet с предложением отключить режим экономии батареи. `energy-saving-request-interval` (#110, default=0) — интервал проверки. `call-permissions-interval` (#23, default=0) — интервал проверки разрешений для звонков. Подробно: `notes/topics/126-energy-saving-pmskey.md`.

---

## Дополнения 127

### 127. Calls SDK stats/ICE PmsKey

`calls-sdk-incall-stat` (#109, default=false) — «Отправлять статистику во время звонка» (real-time). `calls-android-direct-ice-restart` (#110, default=false) — «Явный ICE рестарт». `calls-android-no-ice-restart` (#111, default=false) — «Не делать ICE рестарт». `calls-sdk-opus-adapt` (#135, default=false) — «Адаптивная complexity опус». Подробно: `notes/topics/127-calls-sdk-stats-ice-pmskey.md`.

---

## Дополнения 128

### 128. H265 codec и unknown-contact PmsKey

`calls-sdk-h265-prioritized` (#125, default=false) — «Включить поддержку и приоритизировать H265». `calls-android-h265-s` (#116) — «Деградация кодека H265 (%)». `enable-unknown-contact-bottom-sheet` (#107) — 3 режима UI для незнакомых звонящих (0=не показывать/1=Добавить+Заблокировать/2=Всё в порядке+Заблокировать). Подробно: `notes/topics/128-h265-unknown-contact-pmskey.md`.

---

## Дополнения 129

### 129. Opus BWE и calls audio PmsKey

`calls-sdk-ai-opus-bwe` (#105) — AI-based BWE. `calls-sdk-linear-opus-bwe` (#106, default=false) — «Включить Linear Opus BWE». `calls-sdk-log-audio` (#131, default=false) — «Логгировать локальное аудио». Два взаимоисключающих алгоритма BWE. Подробно: `notes/topics/129-opus-bwe-audio-pmskey.md`.

---

## Дополнения 130

### 130. Calls audio pipeline и simulcast PmsKey

`calls-sdk-disable-pipeline` (#130, default=false) — «Отключить аудио пайплайн» (APM). `calls-sdk-dnt-disable-audio` (#134, default=false) — «Не блокировать звук на старте». `calls-android-simulcast-sw-vp8` (#133, default=false) — «Включить sw VP8 simulcast». `calls-android-signaling-ip` (#119) — «Подключаться к сигналингу по IP». Подробно: `notes/topics/130-calls-audio-pipeline-simulcast.md`.

---

## Дополнения 131

### 131. Calls VTV2/LLA/NIDM PmsKey

`calls-android-vtv2` (#112, default=false) — «Использовать видеотрансформер v2». `calls-android-lla` (#113, default=false) — «Использовать LL audio» (Low Latency). `calls-android-nidm` (#114, default=false) — «Разрешить маппинг только собственного ID». Подробно: `notes/topics/131-calls-vtv2-lla-nidm.md`.

---

## Дополнения 132

### 132. Calls fast-join/NS/new-pms PmsKey

`calls-android-fast-join` (#120, default=false) — «Быстрое присоединение через клиентский бекенд». `calls-android-ns` (#121, default=false) — «Включить шумодав» (WebRTC NS). `calls-android-new-pms` (#123, default=false) — «Новый механизм получения звонковых настроек». Подробно: `notes/topics/132-calls-fastjoin-ns-newpms.md`.

---

## Дополнения 133

### 133. Calls early-offer/gen-peerid PmsKey

`calls-android-early-set-offer` (#136, default=false) — «Разрешить раннюю обработку оффера входящего звонка». `calls-android-gen-peerid` (#137, default=false) — «Генерировать peer-id на клиенте». `calls-android-update-endpoint-params` (#132, default=false) — «Параметры webSocket-а заменять, а не дублировать». Подробно: `notes/topics/133-calls-early-offer-genpeerid.md`.

---

## Дополнения 134

### 134. WebRTC Field Trials

8 hardcoded field trials: `WebRTC-Bwe-LossBasedBweV2`, `CallsSDK-Audio-OpusNOLACE`, `WebRTC-AdjustOpusBandwidth`, `CallsSDK-DREDLowBitrate`, `WebRTC-Audio-StableTargetAdaptation`, `CallsSDK-Audio-OpusAdapterMinBitrate:16000`, `WebRTC-Audio-AdaptivePtime`, `CallsSDK-DisableSharedSocket`. Условные (server-controlled): `EarlyStartPlayout`, `EarlyStartRecording`, `AudioProcessingOffOnMute`. Сервер может передать произвольные field trials через `str2`/`strN`. Подробно: `notes/topics/134-webrtc-field-trials.md`.

---

## Дополнения 135

### 135. CallsSDK Opus DRED/FEC flags

`CallsSDK-Audio-OpusFECWithDRED`, `CallsSDK-Audio-OpusDREDByBitrate`, `CallsSDK-DREDLowBitrate`, `CallsSDK-LinearMinBitrate`, `CallsSDK-OpusFileLogs` (запись Opus-логов в файл — могут быть загружены через vchat.getLogUploadUrl), `Calls-SDK-LogDCTraffic` (логирование DataChannel трафика). Управляются через `bonusFieldTrials`. Подробно: `notes/topics/135-callssdk-opus-dred-fec-flags.md`.

---

## Дополнения 136

### 136. ev1 WebRTC session config

25+ параметров WebRTC сессии. Ключевые: `emulatedSignalingError` (NONE/SERVICE_UNAVAILABLE/PARTICIPANT_LIMIT_REACHED — **сервер может принудительно завершить звонок**), `emulatedIceCandidateError` (NONE/REMOTE/LOCAL), `simulcastState` (DISABLED/ONLY_SW_VP8/ALL_SUPPORTED_CODEC), `isAudioCaptureLoggingEnabled`, `bitrateDumpGatheringState`. Подробно: `notes/topics/136-ev1-webrtc-session-config.md`.

---

## Дополнения 137

### 137. android.rating.limits + android.p2prelay.config

`android.rating.limits` — `RateManagerConfig` с 4 компонентами: `rttRateHintConfig`, `lossHintConfig`, `directCandidateTypeHintConfig`, `serverCandidateTypeHintConfig`. Сервер задаёт пороги RTT/loss для оценки качества. `android.p2prelay.config` — конфиг переключения на P2P relay (условия маршрутизации через серверы VK/OK). Подробно: `notes/topics/137-rating-limits-p2prelay-config.md`.

---

## Дополнения 138

### 138. settings.get API

`GetSettings.METHOD_NAME = "settings.get"` — HTTP API для получения 6 RemoteSettings ключей звонкового SDK одним запросом. `RemoteSettingsShared` кэширует и периодически обновляет. Отдельный канал от WS-протокола и PmsKey. Подробно: `notes/topics/138-settings-get-api.md`.

---

## Дополнения 139

### 139. ApiProtocol constants

`p2p_forbidden` — сервер может запретить P2P для конкретного звонка (все медиа через relay). `stun_server`/`turn_server` — server-controlled ICE серверы. `wt_endpoint` — WebTransport endpoint. `onlyAdminCanRecord`/`onlyAdminCanShareMovie` — права в звонке. Подробно: `notes/topics/139-api-protocol-constants.md`.

---

## Дополнения 140

### 140. vchat.getConversationParams — ConversationParams

`ispAsNo` (ASN провайдера), `ispAsOrg` (организация провайдера), `locCc` (country code), `locReg` (регион) — **сервер знает провайдера и геолокацию каждого участника звонка**. `isP2PForbidden` — запрет P2P. `stunTurnServers` — server-controlled ICE серверы. `wsIps` — IP-адреса WS-серверов. `rateCallData` — данные для оценки качества. Подробно: `notes/topics/140-conversation-params.md`.

---

## Дополнения 141

### 141. RateCallData — server-pushed вопросы для оценки звонка

`maxRateForQuestion` (шкала) + `questions[index, title]` — сервер полностью контролирует содержимое диалога оценки звонка. Приходит через `vchat.getConversationParams`. Подробно: `notes/topics/141-rate-call-data.md`.

---

## Дополнения 142

### 142. ConversationParams compact format

`<size>:<base64(LZ4(JSON))>` — компактный формат для push/URL. Ключи: `tkn`, `srcp`, `trne`/`trnu`/`trnp` (**TURN credentials** — username и password для каждого звонка), `stne` (STUN), `wse`/`wsip` (WS), `wte`/`wtip` (WebTransport). Подробно: `notes/topics/142-conversation-params-compact.md`.

---

## Дополнения 143

### 143. vchat.startConversation

Клиент отправляет: `isVideo`, `turnServers`, `conversationId`, `createJoinLink`, `waitForAdmin`, `capabilities` (hex-encoded возможности клиента), `protocolVersion=6` (для VOIP_MULTIPLE_DEVICES), `domainId`, `payload`, `onlyAdminCanShareMovie`. Сервер знает полные возможности клиента при каждом звонке. Подробно: `notes/topics/143-start-conversation-api.md`.

---

## Дополнения 144

### 144. vchat.hangupConversation + vchat.joinConversationByLink

`vchat.hangupConversation`: `conversationId`, `reason` (enum — причина завершения), `anonymToken`. `vchat.joinConversationByLink`: `joinLink`, `isVideo`, `peerId` (long), `anonymToken` (без аккаунта MAX), `capabilities` (hex), `protocolVersion=6`. Подробно: `notes/topics/144-hangup-join-api.md`.

---

## Дополнения 145

### 145. vchat.getOkIdsByExternalIds + vchat.getExternalIdsByOkIds

Двусторонний маппинг: external_id ↔ ok_id. Позволяет связывать пользователей MAX с аккаунтами в других сервисах VK/OK. Используется для идентификации участников звонков из разных сервисов. Подробно: `notes/topics/145-ok-external-ids-mapping.md`.
