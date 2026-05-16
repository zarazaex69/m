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