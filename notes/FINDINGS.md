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

---

## Дополнения 146

### 146. vchat.getLogUploadUrl + device fingerprint

`vchat.getLogUploadUrl` — сервер выдаёт URL для загрузки диагностических логов звонков. `och.java` — каждый запрос к звонковому API содержит: `device=Build.MANUFACTURER/Build.MODEL` (модель устройства), `osVersion`, `ispAsOrg`, `locCc`, `locReg`. Сервер знает точную модель телефона каждого участника звонка. Подробно: `notes/topics/146-log-upload-device-fingerprint.md`.

---

## Дополнения 147

### 147. CallNativeAnalyticsApiRequest

`platform=ANDROID`, `appVersion` (версия MAX), `sdkType=ANDROID`, `sdkVersion=0.1.13`, `version=1` + 40 метрик звонка. Отправляется через `vchat.clientStats`. Подробно: `notes/topics/147-call-native-analytics-request.md`.

---

## Дополнения 148

### 148. CallExternalAnalyticsApiRequest

Внешняя аналитика звонков с `application`, `collector`, `platform`. Два независимых канала аналитики: нативный (`vchat.clientStats` с sdkVersion) и внешний (с collector). Подробно: `notes/topics/148-call-external-analytics.md`.

---

## Дополнения 149

### 149. UploadConfig — конфиг загрузки аналитики звонков

`DEFAULT_DISABLE_UPLOAD_IN_CALL=true` — загрузка отключена во время активного звонка. `DEFAULT_MAX_EVENT_COUNT=800` — буфер до 800 событий. `DEFAULT_MAX_FILE_SIZE_KB=15`. `DEFAULT_LOCAL_FILE_COUNT=100`. Подробно: `notes/topics/149-upload-config-calls-analytics.md`.

---

## Дополнения 150

### 150. applyBitrateDumpGatheringConfig

При `android.dump.bitrate.isEnabled=true` — создаётся файл `target_bitrate_dump_<timestamp>` для каждого звонка. `ev1.O = new av1(context)` — bitrate dump активирован. Подробно: `notes/topics/150-apply-bitrate-dump-config.md`.

---

## Дополнения 151

### 151. ConversationImpl callbacks

`onCustomData(JSONObject)` — произвольные JSON-данные через WebRTC DataChannel. `onCallParticipantFingerprint(long)` — DTLS fingerprint tracking (детект MITM). `onRateCall(JSONObject)` — server-initiated rating dialog. `onMeInWaitingRoomChanged(bool)` — waiting room status. Подробно: `notes/topics/151-conversation-impl-callbacks.md`.

---

## Дополнения 152

### 152. AsrOnlineManager — real-time транскрипция звонков

`AsrOnlineManager` — real-time транскрипция речи участников звонков. `AsrOnlineChunk`: `participantId` + `text` (кто что сказал). `enableAsrOnline(bool)` — server-controlled. Три уровня аудио-анализа: KWS (topic 70) + on-device ASR (topic 15) + online ASR. Подробно: `notes/topics/152-asr-online-manager.md`.

---

## Дополнения 153

### 153. Signaling commands — 24 команды

`mute-participant` (заглушить участника), `request-asr` (онлайн-транскрипция с start=bool), `enable-feature-for-roles` (feature gating по ролям), `report-network-stat` (timestamp+bitrate), `report-perf-stat`, `change-simulcast`, `update-display-layout`, `switch-room`, `start-url-sharing`/`stop-url-sharing`, `record-stop`, `get-hand-queue`. Подробно: `notes/topics/153-signaling-commands.md`.

---

## Дополнения 154

### 154. ConversationFeature — фичи звонков с role-based access

`ck1` enum: `ADD_PARTICIPANT`, `RECORD` (запись звонка), `MOVIE_SHARE` (watch together), `ASR_RECORD` (**запись для ASR/транскрипции** — отдельная фича). `enableFeatureForRoles(feature, roles)` — role-based access через `enable-feature-for-roles` сигналинг. Подробно: `notes/topics/154-conversation-features-roles.md`.

---

## Дополнения 155-156

### 155. kv1 call roles

`CREATOR` (0), `ADMIN` (1), `SPEAKER` (2). Используются для role-based access к фичам звонка. Подробно: `notes/topics/155-call-roles.md`.

### 156. MicrophoneManager

`registerAudioSampleCallback(delay, callback)` — несколько компонентов одновременно получают аудио-семплы: KWS (topic 70), ASR online (topic 152), energy calculator. `setMicEnabled(bool)` — включение/выключение микрофона. Подробно: `notes/topics/156-microphone-manager.md`.

---

## Дополнения 157

### 157. WatchTogether + URL Sharing

`add-movie` (movieId/gain/moveToAdminOnHangup), `update-movie`, `remove-movie`. `start-url-sharing`/`stop-url-sharing`. Сервер знает, какие видео и URL шарятся в звонках. Подробно: `notes/topics/157-watch-together-url-sharing.md`.

---

## Дополнения 158

### 158. change-media-settings — принудительное изменение медиа

`MediaSettings`: audio/video/screen_capture/animoji флаги. `handleForceChangeMediaSettings` — **принудительное** изменение медиа-настроек участника сервером/администратором (выключить камеру, микрофон, screen capture). Подробно: `notes/topics/158-change-media-settings.md`.

---

## Дополнения 159

### 159. StereoRoomManager — комнаты, роли и поднятые руки

`requestPromotion`/`promoteParticipant` — механизм модерации вебинаров (поднять руку). `grantAdmin`/`revokeAdmin` — динамическое изменение прав. `handsQueue` — очередь поднятых рук. `switch-room` сигналинг — переключение между комнатами. Подробно: `notes/topics/159-stereo-room-manager.md`.

---

## Дополнения 160

### 160. update-media-modifiers

`mediaModifiers: {denoise: bool, denoiseAnn: bool}`. Сервер может принудительно изменить настройки шумоподавления участника во время звонка. `denoiseAnn` — нейросетевое шумоподавление. Подробно: `notes/topics/160-update-media-modifiers.md`.

---

## Дополнения 161

### 161. change-simulcast

`mediaSource=CAMERA`, `layers[{rid, width, height, fps, bitrateKbps}]`. Сервер может изменить качество видео-потоков во время звонка. Подробно: `notes/topics/161-change-simulcast.md`.

---

## Дополнения 162

### 162. report-perf-stat + update-display-layout

`report-perf-stat`: `{framesReceived, framesDecoded}`. `update-display-layout`: `{layouts: {participantId: "ss"|"sz=WxH:fit=mode"}, snapshot: bool}` — **сервер знает UI layout клиента** (размер, режим отображения каждого участника). Подробно: `notes/topics/162-perf-stat-display-layout.md`.

---

## Дополнения 163

### 163. ConversationStats — 18+ компонентов статистики

`kwsStat` (KWS), `mlDownloadStat` (ML-модели), `iceCandidatePairChangedStat` (ICE routing), `serverTopologyRequestedStat`, `peerConnectionStateChangedStat`, `negotiationErrorStat`, `audioErrorStat`, `acceptCallStat`, `p2pRelayRequestedStat`. Всё отправляется через `vchat.clientStats`. Подробно: `notes/topics/163-conversation-stats.md`.

---

## Дополнения 164

### 164. SignalingTransportStat

Детальная статистика WS-сигналинга: `onConnect`/`onConnected`/`onRestart`/`onMessageReceived`(isPing)/`onCommandSent`/`onFailedByPings`/`onFailedByException`/`onTimeout`. `connectedAtLeastOnceInCall`, `startConnectTime`, `lastMessageReceived`, `firstFailTime`. Подробно: `notes/topics/164-signaling-transport-stat.md`.

---

## Дополнения 165

### 165. IceCandidatePairChangedStat

При каждом изменении ICE pair: `local_address`/`remote_address` (**реальные IP-адреса участников**), `local.sdp`/`remote.sdp` (SDP с типом кандидата/IP/порт), `reason`, `lastDataReceivedMs`. Сервер получает полную историю маршрутизации медиа-трафика. Подробно: `notes/topics/165-ice-candidate-pair-stat.md`.

---

## Дополнения 166

### 166. AcceptCallStat

`call_accepted_incoming` с `isCaller`/`isMe`/`isConcurrent`. Сервер знает: кто принял звонок, на каком устройстве, был ли параллельный звонок. Подробно: `notes/topics/166-accept-call-stat.md`.

---

## Дополнения 167

### 167. P2PRelayRequestedStat

`client_requested_p2p_relay` с `trigger`/`threshold`/`violationsCount`. Сервер знает, когда и почему клиент переключился на relay (в отличие от `p2p_forbidden` — когда сервер запрещает P2P). Подробно: `notes/topics/167-p2p-relay-requested-stat.md`.

---

## Дополнения 168

### 168. ServerTopologyRequestedStat

`client_requested_server_topology` — сервер знает, когда клиент переключился на серверную топологию (медиа через серверы VK/OK). Подробно: `notes/topics/168-server-topology-stat.md`.

---

## Дополнения 169

### 169. NegotiationErrorStat + AudioErrorStat

`NegotiationErrorStat` — SDP при ошибке WebRTC negotiation (полный SDP с кодеками/ICE). `AudioErrorStat` — `audio_error` с тремя полями (тип:код:описание). Подробно: `notes/topics/169-negotiation-audio-error-stat.md`.

---

## Дополнения 170

### 170. IceCandidateGatheringFailedStat + IceCandidateAddFailedStat

`IceCandidateGatheringFailedStat`: `local_address`/`remote_url`/`transport` при ошибке сбора ICE. `IceCandidateAddFailedStat`: `ice_candidate_add_failed` с `remote_url`. Подробно: `notes/topics/170-ice-candidate-error-stats.md`.

---

## Дополнения 171

### 171. IceRestartStat + PeerConnectionChangedStat

`IceRestartStat`: `ice_restart` (без параметров). `PeerConnectionChangedStat`: `connection_state_changed` с `connection_state` (new/connecting/connected/disconnected/failed/closed) и `p2p_relay` (bool). Подробно: `notes/topics/171-ice-restart-peer-connection-stat.md`.

---

## Дополнения 172

### 172. ConversationStartedStat + ConversationConnectedToSignalingStat

`call_start` с `callType`/`warmupStatus` (labels). `signaling_connected` с временем до подключения (мс). Оба — SingleShotStat. Подробно: `notes/topics/172-conversation-started-signaling-stat.md`.

---

## Дополнения 173

### 173. CallFinishStat + CallInitStat

`call_finish`: `reason` (ConversationEndReason), `rate_reasons` (автоматические причины плохого качества), `errorText`. `call_init`: `source` (callType + isAnon). Подробно: `notes/topics/173-call-finish-init-stat.md`.

---

## Дополнения 174

### 174. ConversationPreparedStat

`call_warmup` — время прогрева WebRTC сессии (мс). Подробно: `notes/topics/174-conversation-prepared-stat.md`.

---

## Дополнения 175

### 175. ConversationEndReason — 17 причин завершения

`banned`, `killed`/`killed_without_delete` (принудительное завершение сервером), `obsolete_client` (устаревший клиент), `another_device` (multi-device), `socket_closed`, `timeout`, `hangup`, `missed`, `rejected`, `busy`, `error`, `failed`, `canceled`, `removed`, `initially_closed`, `call_timeout`. Подробно: `notes/topics/175-conversation-end-reason.md`.

---

## Дополнения 176

### 176. RateHint — автоматические причины плохого качества

`rtt_<rttMs>`, `audioloss_<maxLoss>`, `videoloss_<maxLoss>`, candidate type. Пороги server-controlled через `android.rating.limits`. 4 триггера: RTT, loss (audio+video), directCandidateType, serverCandidateType. Подробно: `notes/topics/176-rate-hints.md`.

---

## Дополнения 177

### 177. NetworkStat

`rttMs`/`audioLoss`/`videoLoss`/`activeCandidateType` (host/srflx/relay). Используется для генерации `RateHint` при превышении порогов. Подробно: `notes/topics/177-network-stat.md`.

---

## Дополнения 178

### 178. Conversation.CallType + Conversation.State

`CallType`: Incoming/Outgoing/Join. `State`: None(0)/Preparing(1)/Starting(2)/Connecting(3)/Connected(4)/Finished(5). Подробно: `notes/topics/178-call-type.md`.

---

## Дополнения 179

### 179. Conversation interface — 30+ менеджеров

`AsrManager`, `AsrOnlineManager`, `CameraManager`, `ChatManager`, `ConversationFeatureManager`, `KeywordSpotterManager`, `MicrophoneManager`, `NoiseSuppressionManager`, `RecordManager`, `StereoRoomManager`, `UrlSharingManager`, `WatchTogetherPlayer`, `DisplayLayoutSender`, `DebugManager` (в production!). Подробно: `notes/topics/179-conversation-interface.md`.

---

## Дополнения 180

### 180. RecordManager — запись и стриминг звонков

`startRecord(StartParams)`: `isStream` (стриминг), `movieId`, `albumId`, `sessionRoomId`, `name`, `description`, `groupId`, `privacy`. Интеграция с видео-платформой VK/OK. `stopRecord`. Подробно: `notes/topics/180-record-manager.md`.

---

## Дополнения 181

### 181. NoiseSuppressionManager

4 режима: `serversideBasic`, `serversideAnn`, `clientsidePlatform`, `clientsideAnn`. `filePath` — ML-модель шумоподавления. Fallback параметры при проблемах с производительностью. Подробно: `notes/topics/181-noise-suppression-manager.md`.

---

## Дополнения 182

### 182. ChatManager — чат в звонке

`sendMessage(OutboundMessage: participantId+text)` через WebRTC DataChannel. При P2P relay — трафик через серверы VK/OK. `Calls-SDK-LogDCTraffic` — логирование DataChannel трафика. Подробно: `notes/topics/182-chat-manager.md`.

---

## Дополнения 183

### 183. MediaMuteManager + ParticipantStatesManager

`updateMediaOptionsForAll`/`updateMediaOptionsForParticipant` — администратор меняет медиа-опции для всех/участника. `requestToEnableMediaForParticipant` — запрос включить камеру/микрофон. `ParticipantStateChange`: `participantId`/`isOn`/`timestamp`. Подробно: `notes/topics/183-media-mute-participant-states.md`.

---

## Дополнения 184

### 184. AsrManager — on-device ASR запись

`startRecord(fileName, sessionRoomId)` — ASR-запись в файл. Отдельно от `AsrOnlineManager` (real-time). `ASR_RECORD` фича с role-based access. Подробно: `notes/topics/184-asr-manager.md`.

---

## Дополнения 185

### 185. FeedbackManager — emoji-реакции в звонках

`sendFeedback(key, source)` + `setTimeout(millis)`. Сервер знает, кто и когда отправил реакцию. Подробно: `notes/topics/185-feedback-manager.md`.

---

## Дополнения 186

### 186. ContactCallManager — анонимность в звонках

`iAmAnonymous`/`iWasInitiallyAnonymous`. Анонимные участники через `anonymToken`. Статус анонимности может меняться во время звонка. Подробно: `notes/topics/186-contact-call-manager.md`.

---

## Дополнения 187

### 187. NetworkConnectionManager

`getTopology()` (P2P/server), `registerBadConnectionCallback`, `addNetworkConnectivityListener`. Подробно: `notes/topics/187-network-connection-manager.md`.

---

## Дополнения 188

### 188. CameraManager

`getNumberOfCameras`/`isCameraEnabled`/`isCapturingFromFrontCamera`/`setCameraEnabled`/`switchCamera`. Подробно: `notes/topics/188-camera-manager.md`.

---

## Дополнения 189

### 189. MediaConnectionManager

`onMediaConnected(ConnectedInfo: isFirstConnection)` / `onMediaDisconnected`. `isFirstConnection` — первое ли подключение или переподключение. Подробно: `notes/topics/189-media-connection-manager.md`.

---

## Дополнения 190

### 190. ConversationEventsListener — 30+ событий

`onMicrophoneForciblyMuted` (принудительное заглушение), `onParticipantsDeAnonymized` (**деанонимизация участников**), `onMigratedToServerTopology`, `onCustomData` (DataChannel), `onConversationIdChanged`, `onRateCall`, `onRolesChanged`, `onWaitForAdminEnabled`. Подробно: `notes/topics/190-conversation-events-listener.md`.

---

## Дополнения 191

### 191. ConversationParticipant — 20+ полей

`getAcceptedCallClientType`/`getAcceptedCallPlatform` (платформа при входе), `getCapabilities` (возможности клиента), `getNetworkStatus`, `isTalking`, `isReported`, `isAdmin`/`isCreator`/`isPrimarySpeaker`, `getAudioOptionState`/`getVideoOptionState`/`getScreenshareOptionState`. Подробно: `notes/topics/191-conversation-participant.md`.

---

## Дополнения 192

### 192. ClientCapabilities — 15 битов возможностей

Hex-encoded: `BIT_SCREEN_TRACK_PRODUCER`(0), `BIT_VIDEO_TRACKS`(1), `BIT_WAITING_HALL`(2), `BIT_SCREEN_TRACK_CONSUMER`(4), `BIT_ADMIN_MUTE_NOTIFY`(5), `BIT_WATCH_MOVIE`(6), `BIT_SESSION_ROOMS`(8), `BIT_VMOJI`(9), `BIT_AUDIENCE_MODE`(11), `BIT_ADD_PARTICIPANT`(15), `BIT_USE_P2P_RELAY`(16), `BIT_WAIT_FOR_ADMIN`(17). Подробно: `notes/topics/192-client-capabilities.md`.

---

## Дополнения 193

### 193. CallInfo — полная информация о звонке

`endpoint`/`wsIps`/`wtEndpoint`/`wtIps`/`turnServer`/`stunServer`/`isP2PForbidden`/`deviceIndex`. Сервер полностью контролирует маршрутизацию медиа-трафика. Подробно: `notes/topics/193-call-info.md`.

---

## Дополнения 194

### 194. StartConversationDelegate.Params

`conversationId`/`calleeIds` (список вызываемых)/`chatId`/`isVideo`/`internalParams`. Подробно: `notes/topics/194-start-conversation-delegate.md`.

---

## Дополнения 195

### 195. ConversationFactory — 6 типов звонков

`startCall`/`createConf`/`answerCall`/`joinCall`/`joinAnonByLink`/`joinByLink`. `ConversationBuilder` — 18+ параметров. Каждый звонок инициализируется с `RemoteSettings`/`experiments`/`fieldTrials`. Подробно: `notes/topics/195-conversation-factory.md`.

---

## Дополнения 196

### 196. DebugManager + MediaDumpManager — дамп медиа в production

`requestMediaDump(durationSec, audio, video)` — **удалённый** запрос дампа аудио+видео. 6 точек перехвата аудио-pipeline: `IN_ENTER_PROCESSING`/`IN_AFTER_NS`/`IN_AFTER_ANIMOJI`/`IN_EXIT_PROCESSING`/`OUT_ENTER_PROCESSING`/`OUT_EXIT_PROCESSING`. `enableFullAudioDump(path)` — полный дамп в файл. Всё в production-сборке. Подробно: `notes/topics/196-debug-media-dump-manager.md`.

---

## Дополнения 197

### 197. MLFeaturesManager + ConversationFeatureManager

`MLFeaturesManagerImpl` — оркестратор `KwsFeatureDelegate`+`NSFeatureDelegate` с загрузкой моделей через `DownloadService`+`RemoteSettings`. `ConversationFeatureManager` — серверный контроль фич: `enableFeatureForAll`/`enableFeatureForRoles(roles)`. Подробно: `notes/topics/197-ml-features-conversation-feature-manager.md`.

---

## Дополнения 198

### 198. SessionRoomsManager + StereoRoomManager + RateManager

`SessionRoomsManager` — breakout rooms: `assignParticipantsToRooms` (принудительное распределение). `StereoRoomManager` — сцена/аудитория: `promoteParticipant`/`unpromoteParticipant`/`grantAdmin`/`getHandsQueue`. `RateManager` — `getShouldRateConversation()` на основе серверных `RateHint`. Подробно: `notes/topics/198-session-stereo-rate-manager.md`.

---

## Дополнения 199

### 199. CallsAudioManager + ScreenCaptureManager + VideoRenderManager

`CallsAudioManager` — `onMutedForever`, proximity tracking при громкоговорителе, `State(IDLE/DIALING/RINGING/CONVERSATION)`. `ScreenCaptureManager` — `setAudioCaptureEnabled(true)` захватывает системный аудио при screenshare. `VideoRenderManager` — рендереры видео участников. Подробно: `notes/topics/199-calls-audio-screen-video-render.md`.

---

## Дополнения 200

### 200. vchat API — полный список

11 методов: `vchat.startConversation`/`joinConversation`/`joinConversationByLink`/`hangupConversation`/`getConversationParams`/`createJoinLink`/`removeJoinLink`/`clientSupportedCodecs`/`getExternalIdsByOkIds`/`getOkIdByExternalId`/`getOkIdsByExternalIds`. Подробно: `notes/topics/200-vchat-api-full-list.md`.

---

## Дополнения 201

### 201. Calls SDK — архитектурная сводка (ИТОГ)

24 менеджера, 24 сигналинг-команды, 18+ stat-структур. Ключевые индикаторы: нет E2E, ASR on-device+server-side, KWS «не слышу», `requestMediaDump` (удалённый дамп аудио+видео в prod), `onParticipantsDeAnonymized`, `onMicrophoneForciblyMuted`, device fingerprint (ISP+geo) в каждом запросе, `ClientCapabilities` hex-encoded, WebRTC field trials с сервера. Подробно: `notes/topics/201-calls-sdk-architecture-summary.md`.

---

## Дополнения 202

### 202. OneLog — механизм загрузки

`Collector` (файловый буфер `onelog/<collector>/append+upload`) → `Uploader` → `OneLogApiRequest`. В каждом запросе: `application=ru.oneme.app:26153:26.15.3`, `platform=android:phone:<Android version>`. `UploadService` — фоновый `IntentService`. Подробно: `notes/topics/202-onelog-upload-mechanism.md`.

---

## Дополнения 203

### 203. Live Location — геолокация

`LocationData`: `latitude`/`longitude`/`altitude`/`accuracy(epu)`/`bearing(hdn)`/`speed(spd)`. `livePeriod` — живая геолокация. FCM push `LocationRequest` — **сервер инициирует получение геолокации** без действий пользователя (`LocationManager.getLastKnownLocation(gps/network)`). Подробно: `notes/topics/203-live-location.md`.

---

## Дополнения 204

### 204. Opus FileWriter — нативная запись аудио

`nativeAudioStartRecord(path, sampleRate, channels)` → `nativeAudioWriteFrame(ByteBuffer)` → `close`. Используется в `AsrManager` (запись для ASR) и `MediaDumpManager` (дамп pipeline). Подробно: `notes/topics/204-opus-file-writer.md`.

---

## Дополнения 205

### 205. Tasks.proto — 60+ фоновых задач

`LocationRequest`(liveLocation/messageId/requestId), `CritLog`, `SuspendBot`, `UpdateFireTimeProtoTask`, `SyncChatHistory`/`WarmChatHistory`, `MsgSend`/`MsgEdit`/`MsgDelete`, `FileUpload`/`FileDownload`. Подробно: `notes/topics/205-tasks-proto.md`.

---

## Дополнения 206

### 206. Protos.java — 50+ protobuf nano схем

`Location`: `latitude`/`longitude`/`altitude`/`accuracy`/`bearing`/`speed`/`livePeriod`/`startTime`/`endTime`/`ttl`/`zoom`/`corrupted`. `VideoConversation`: `conversationId`/`joinLink`/`previewParticipantIds`/`approxParticipantCount`/`type(BY_LINK/FROM_CHAT)`. `LogEvent`: `event`/`params(bytes)`/`sessionId`. Подробно: `notes/topics/206-protos-schemas.md`.

---

## Дополнения 207

### 207. CritLog — 30+ типов критических событий

`AUDIO_TRANSCRIPTION`(message_id/media_id/duration/waiting_time), `DANGEROUS_FILE_ACTIONS`/`DANGEROUS_URL_ACTIONS`, `MINIAPP_BIOMETRY`, `GET_INSTALL_REFERRER`, `ADMIN_CALL_SETTINGS_TO_USER`, `WEBAPP_ACTION`/`WEBAPP_BRIDGE`, `PERMISSION`, `PUSH`, `REGISTRATION`. Подробно: `notes/topics/207-critlog-events.md`.

---

## Дополнения 208

### 208. WebApp телеметрия — WEBAPP_ACTION + WEBAPP_BRIDGE

`WEBAPP_ACTION`: OPEN/CLOSE/REFRESH/MINIAPP_TAKE_PHOTO + botId/webAppName/entryPoint/sourceType. `WEBAPP_BRIDGE`: **каждый JS-bridge вызов** логируется с sessionId/botId/webAppName/success/method/code. Подробно: `notes/topics/208-webapp-telemetry.md`.

---

## Дополнения 209

### 209. GET_INSTALL_REFERRER — источник установки

`getInstallerPackageName()` → нормализация → CritLog `ACTION`/`GET_INSTALL_REFERRER`. Поля: `value` (имя пакета установщика), `is_update_version`. Отправляется один раз при первом запуске новой версии. Подробно: `notes/topics/209-install-referrer.md`.

---

## Дополнения 210

### 210. SEARCH_RESPONSE — телеметрия поиска

`inputQuery` — **поисковый запрос отправляется на сервер**. `counters`: RECENTS/ALL_CONTACTS/LOCAL_SEARCH. Подробно: `notes/topics/210-search-response-telemetry.md`.

---

## Дополнения 211

### 211. PRESENCE/EVENT_MESSAGE_COUNTER + MESSAGE_CLICKABLE_ELEMENT_ACTIONS

`EVENT_MESSAGE_COUNTER`: 8 счётчиков (online/offline × contact/stranger × opened/closed). `MESSAGE_CLICKABLE_ELEMENT_ACTIONS`: clicked_copy/clicked_open_link/clicked_call/clicked_update_app + messageId/source_id. Подробно: `notes/topics/211-presence-message-click-telemetry.md`.

---

## Дополнения 212

### 212. CHANNEL_RECSYS_FOLDER — рекомендательная система

`channel_folder_open`/`channel_folder_click`/`channel_folder_follow`/`channel_folder_delete` + `channel_id`/`channel_position`. Данные для обучения рекомендательного алгоритма. Подробно: `notes/topics/212-channel-recsys-telemetry.md`.

---

## Дополнения 213

### 213. VIDEO_STATS + AUDIO_STATS

`VIDEO_STATS`: action_play/first_bytes/content_error/close_at_empty_buffer/empty_buffer + vid/vsid/cdn_host/quality/connection_type. `AUDIO_STATS`: first_bytes. Подробно: `notes/topics/213-video-audio-stats-telemetry.md`.

---

## Дополнения 214

### 214. WS Session Config — расширенные поля

`proxy`/`proxy-domains`/`isVpn`/`location`(код страны по гео)/`reg-country-code`(код страны регистрации)/`lang`/`callsSeed`/`app-update-type`. Сервер знает VPN, прокси и страну. Подробно: `notes/topics/214-ws-session-config-extended.md`.

---

## Дополнения 215

### 215. Схема БД сообщений

35+ полей: `ttl` (самоудаление), `live_until` (живая геолокация), `delayed_attrs_time_to_fire` (отложенная отправка), `channel_views`/`channel_forwards`, `msg_link_*` (forward chain). Подробно: `notes/topics/215-messages-db-schema.md`.

---

## Дополнения 216

### 216. Локальная БД — 35+ таблиц

`webapp_biometry` (биометрия мини-приложений), `fcm_notifications_history`, `stat_events`, `battery`, `presence`, `tasks`, `organizations`, `complain_reasons`. Подробно: `notes/topics/216-local-db-tables.md`.

---

## Дополнения 217

### 217. WebApp Biometry — биометрия в мини-приложениях

`webapp_biometry`: user_id/bot_id/**token**/access_requested/access_granted. 8 JS-bridge методов: `WebAppBiometryAuthRequest`/`WebAppBiometryUpdateTokenRequest`/`WebAppBiometryAccessRequest`. Токен хранится в БД. Подробно: `notes/topics/217-webapp-biometry.md`.

---

## Дополнения 218

### 218. FCM + stat_events DB схемы

`fcm_notifications_analytics`: 5 временных меток push lifecycle (sent_time/fcm_sent_time/received_time/created_time/time). `stat_events`: BLOB-буфер статистики. `fcm_notifications`: text/sender/url/bmd. Подробно: `notes/topics/218-fcm-stat-db-schemas.md`.

---

## Дополнения 219

### 219. battery + organizations DB

`battery`: sliceTime/utime/stime/batteryCapacity/instantAmperage — детальная статистика энергопотребления. `organizations`: id/name/parentId/folderTemplateId/updateTime. Подробно: `notes/topics/219-battery-organizations-db.md`.

---

## Дополнения 220

### 220. QUIC TLS — отключена проверка сертификата

`qse` — null `X509TrustManager` (`checkServerTrusted` пустой). Используется для QUIC/WebTransport. Приложение само выводит `SECURITY WARNING: INSECURE configuration! Server certificate validation is disabled; QUIC connections may be subject to man-in-the-middle attacks!` — но всё равно использует. Подробно: `notes/topics/220-quic-tls-disabled.md`.

---

## Дополнения 221

### 221. network_security_config.xml — cleartext HTTP

Явно разрешён cleartext HTTP для 6 доменов: `mobileid.megafon.ru`, `idgw.mobileid.mts.ru`, `hhe.mts.ru`, `he-mc.tele2.ru`, `he-mc.t2.ru`, `balance.beeline.ru`. Подтверждение [[01-mobile-id-cleartext]]. Подробно: `notes/topics/221-network-security-config.md`.

---

## Дополнения 222

### 222. Deep link схемы

`max://`, `https://max.ru`, `yandexmaps://`, `yandexnavi://`, `dgis://`, `petalmaps://` (Huawei). Поддержка Petal Maps — ориентация на российский рынок без Google. Подробно: `notes/topics/222-deeplink-schemes.md`.

---

## Дополнения 223

### 223. Device performance telemetry

`bucket`(App Standby Bucket — насколько активно используется), `memory`/`large_memory`, `exit_reason`(`getHistoricalProcessExitReasons` — причина завершения: ANR/OOM/crash/kill), `img_cache`/`img_err`. Подробно: `notes/topics/223-device-performance-telemetry.md`.

---

## Дополнения 224

### 224. yag SharedPreferences — 60+ полей

`okToken`/`okTokenRefreshTs`, `pushToken`/`pushDeviceType`, `currentProxyList`/`pushProxyList`/`lastSuccessProxy`, `firstLoginTime`/`lastLoginTime`, `isDebugHostRotationEnabled`/`isDebugUaDnsEmulationEnabled`/`isIceCandidateEmulationEnabled` (debug-флаги в prod). Подробно: `notes/topics/224-yag-shared-prefs-full.md`.

---

## Дополнения 225

### 225. auth.anonymLogin

`device_id`/`version=2`/`client_version=android_8`/`client_type=SDK_ANDROID`. Анонимная авторизация для calls SDK — анонимность относительная, device_id передаётся. Подробно: `notes/topics/225-auth-anonym-login.md`.

---

## Дополнения 226

### 226. log.externalLog — API загрузки OneLog

POST+gzip. Параметры: `collector`/`application`(ru.oneme.app:26153:26.15.3)/`platform`(android:phone:ver)/`items`. Базовый класс для `CallAnalyticsApiRequest`. Подробно: `notes/topics/226-log-external-log-api.md`.

---

## Дополнения 227

### 227. DNS over HTTPS — обнаружение API endpoint

`https://dns.google.com/resolve?name=api._endpoint.ok.ru.&type=16` — TXT-запись DNS для динамического обнаружения API endpoint. Обход DNS-блокировок через Google DNS. Подробно: `notes/topics/227-dns-over-https-endpoint-discovery.md`.

---

## Дополнения 228

### 228. libtracernative.so — экспорты Apptracer

24 экспорта: `tracer_aurora_collect_minidump_from_cachedir` (**Aurora OS** — российская ОС), `tracer_supports_arbitrary_dump`, `tracer_set_api_endpoint`, `tracer_disable_upload`, `tracer_upload`/`tracer_upload_async`/`tracer_upload_crashes`. Подробно: `notes/topics/228-libtracernative-exports.md`.

---

## Дополнения 229

### 229. libEnhancementLibShared.so — нативные компоненты

ASR: `c_enh_asr_predict`/`calcConfidence`/`fillPunct`. KWS: `BCResNetKWS::score`/`extractFeatures`/`computeProbs`. Denoise: `c_enh_denoise_accept_pcm`/`finalize`. **`SpeakerRecognitionEngine`+`SpeakerRecognitionVerifier`** — идентификация личности по голосу. Подробно: `notes/topics/229-lib-enhancement-exports.md`.

---

## Дополнения 230

### 230. libEnhancementLibShared — build paths

Build path: `/home/good/mainframer/webrtc4/src/.../one-ann-audio-processing`. Зависимости: TFLite + Kaldi. `webrtc4` — 4-я версия кастомного WebRTC VK. `one-ann-audio-processing` — внутреннее название модуля. Подробно: `notes/topics/230-lib-enhancement-build-info.md`.

---

## Дополнения 231

### 231. MyTrackerParams — пользовательские параметры

`age`/`gender`/`email`/`phone`/`okId`/`vkId`/`vkConnectId`/`icqId`. MyTracker связывает пользователя MAX с аккаунтами OK.ru/VK/ICQ. Подробно: `notes/topics/231-mytracker-params.md`.

---

## Дополнения 232

### 232. MyTracker AntiFraudConfig + InstalledPackagesProvider

`AntiFraudConfig`: 5 сенсоров (гироскоп/магнитное поле/давление/освещённость/proximity). `InstalledPackagesProvider.getInstalledPackages()` — **список установленных приложений** передаётся в MyTracker. `getApkPreinstallParams` — параметры предустановки. Подробно: `notes/topics/232-mytracker-config-antifraud.md`.

---

## Дополнения 233

### 233. DigitalID — URL-параметры

`&digitalId=<id>&oid=<userId>&photo=<base64>` — фото пользователя передаётся в GET-параметре URL при переходе с DigitalID. Подробно: `notes/topics/233-digital-id-url-params.md`.

---

## Дополнения 234

### 234. ri9 SharedPreferences — 50+ полей

`phoneNumber`/`locationCountryCode`/`gostLicenseCheckEnabled`/`allowLogSensitiveData` (разрешить логирование чувствительных данных)/`isDisableWebAppSsl`/`leakCanaryEnabledStateFlow`/`tenorAnonId` (Tenor GIF). Подробно: `notes/topics/234-ri9-shared-prefs.md`.

---

## Дополнения 235

### 235. qp6 дополнительные PmsKey

`gostEnvironmentCheckFlags`, `isWebRtcLoggingEnabled`/`isCallsAudioLoggingEnabled` (сервер включает детальное логирование), `isLogVisibleMessagesMetaEnabled`/`isLogChatMetaEnabled` (логирование метаданных сообщений), `liveStreamsEnabled`/`liveStreamsUrlPrefix`, `callsTrafficMarkers`, `batterySliceIntervalMs`, `useWebAppPhoneHash`. Подробно: `notes/topics/235-qp6-additional-pmskeys.md`.

---

## Дополнения 236

### 236. WebApp Phone Hash

Мини-приложения получают `phone`+`hash`+`authDate` через JS-bridge. Серверный флаг `useWebAppPhoneHash` управляет передачей. Подробно: `notes/topics/236-webapp-phone-hash.md`.

---

## Дополнения 237

### 237. CallsSignalingTimeouts

`enabled`/`connectTimeout`(5s)/`initialReconnectDelay`(2s)/`reconnectDelayScaleFactor`(1.0)/`maxReconnectDelay`(2s). Серверно-управляемые через PmsKey. Подробно: `notes/topics/237-calls-signaling-timeouts.md`.

---

## Дополнения 238

### 238. chats + uploads DB схемы

`chats`: id/server_id/data(BLOB)/favourite_index/sort_time/cid. `uploads`: 15 полей включая thumbhash_base64. Подробно: `notes/topics/238-chats-uploads-db-schemas.md`.

---

## Дополнения 239

### 239. informer_banner DB

14 полей: click_time/show_time/close_time/show_count — полный lifecycle взаимодействия с баннером. Подробно: `notes/topics/239-informer-banner-db.md`.

---

## Дополнения 240

### 240. metrics DB — distributed tracing

`traceId`/`metricName`/`spanAndPropertiesDump`(BLOB)/`attempt`/`isMarkedAsFailed`. Distributed tracing с retry. Подробно: `notes/topics/240-metrics-db-schema.md`.

---

## Дополнения 241

### 241. tasks + recent DB схемы

`tasks`: id/type/status/fails_count/data(BLOB protobuf)/created_time. `recent`: recent_type/sticker_id/emoji/gif(BLOB)/gif_id. Подробно: `notes/topics/241-tasks-recent-db-schemas.md`.

---

## Дополнения 242

### 242. notifications_tracker_messages DB

`chat_id`/`message_id`/`time`/`fcm`/`drop_reason` — трекинг причин отброса уведомлений. Подробно: `notes/topics/242-notifications-tracker-db.md`.

---

## Дополнения 243

### 243. chat_folder DB

14 полей: filters/filterSubjects/widgets/templateId/sourceId. Подробно: `notes/topics/243-chat-folder-db.md`.

---

## Дополнения 244

### 244. log-sensitive / log-full — серверное управление логированием

`log-sensitive=true` в ответе LOGIN — **сервер включает логирование токена авторизации** в открытом виде. `log-full=true` — полное логирование контактов. PmsKey `logsensitive`. Подробно: `notes/topics/244-log-sensitive-server-flag.md`.

---

## Дополнения 245

### 245. LOGIN WS ответ

Поля: config/drafts/presence/contacts/messages/profile/updates/time/calls/chats + log-full/log-sensitive. `LOGIN.Response`: profile/token/chatMarker/videoChatHistory/resetAt/contactInfos/config. Подробно: `notes/topics/245-login-ws-response.md`.

---

## Дополнения 246

### 246. WS Session Fingerprint — расширенные поля

`enable-audio-messages-transcription`/`enable-video-messages-transcription` (сервер управляет транскрипцией), `calls-sdk-log-audio`, `ab-status`, `one-video-player`/`one-video-uploader-config`. Итого 40+ полей. Подробно: `notes/topics/246-ws-session-fingerprint-extended.md`.

---

## Дополнения 247

### 247. UserSettings — полный список

26+ ключей: `audio.transcription.enabled`, `app.family.protection.status`, `app.privacy.phone.number.privacy`, `app.privacy.inactive.ttl`, `app.privacy.safe_mode`, `app.privacy.search_by_phone`, `app.notification.*`. Подробно: `notes/topics/247-user-settings-full.md`.

---

## Дополнения 248

### 248. Task Types — 46 типов

`TYPE_LOCATION_REQUEST`/`TYPE_LOCATION_STOP`, `TYPE_STAT_CRIT_EVENT`, `TYPE_SUSPEND_BOT`, `TYPE_WARM_CHAT_HISTORY`, `TYPE_CHAT_MARK_BATCH`/`TYPE_CHAT_DELETE_BATCH`. Подробно: `notes/topics/248-task-types.md`.

---

## Дополнения 249

### 249. MsgSend + ChatPersonalConfig + ChatGroupMark

`MsgSend`: chatId/messageId/userId/traceId/lastKnownDraftTime/notify. `ChatPersonalConfig`: hideNonContactBar. `ChatGroupMark`: flagType/enabled. Подробно: `notes/topics/249-msg-send-chat-tasks.md`.

---

## Дополнения 250

### 250. Config + Complain Tasks

`Config`: `isPushToken`/`isUserSettings`/`userSettings(Map<String,String>)`/`syncChatIds`. `Complain`: `ids[]`/`reasonId`/`typeId`/`details`. Подробно: `notes/topics/250-config-complain-tasks.md`.

---

## Дополнения 251

### 251. ContactUpdate + ContactVerify Tasks

`ContactUpdate`: contactId/action/newName/oldName/lastName/oldLastName. `ContactVerify`: contactId/confirm/localName. Подробно: `notes/topics/251-contact-tasks.md`.

---

## Дополнения 252

### 252. Profile + ChatCreate Tasks

`Profile`: firstName/lastName/description/link/photoId/photoToken/avatarType/crop. `ChatCreate`: chatId/chatType/groupId/subjectId/subjectType/startPayload. Подробно: `notes/topics/252-profile-chat-create-tasks.md`.

---

## Дополнения 253

### 253. ChatUpdate + ChatMembersUpdate Tasks

`ChatUpdate`: description/photoToken/pinMessageId/notifyPin/theme. `ChatMembersUpdate`: userIds[]/operation/chatMemberType/showHistory. Подробно: `notes/topics/253-chat-update-members-tasks.md`.

---

## Дополнения 254

### 254. Crash/Log Upload

Multipart: stackTrace/uploadBean(type/format/severity/tags)/threadDump(threads.gzip)/logs(logs.gzip)/drops(drops.json). Полные логи и дамп потоков при краше. Подробно: `notes/topics/254-crash-log-upload.md`.

---

## Дополнения 255

### 255. Protos.java — дополнительные схемы

`BotsInfo`: hasBots/suspendedBot. `ChannelInfo`: admins[]/membersCount/signAdmin. `SelfProfile`: restrictions(Map<Integer,RestrictionsInfo>). `RestrictionsInfo`: expiration. Подробно: `notes/topics/255-protos-additional.md`.

---

## Дополнения 256

### 256. Protos.Chat — 40+ полей

`messagesTtlSec`/`lastSearchClickTime`/`pendingJoinRequestsCount`/`invitedBy`/`admins[]`/`owner`/`restrictions`/`chatFoldersIds[]`/`draft`/`liveStreamUpdateTime`. Подробно: `notes/topics/256-protos-chat-schema.md`.

---

## Дополнения 257

### 257. Protos.Attach — 17 типов вложений

17 типов: PHOTO/VIDEO/AUDIO/STICKER/SHARE/APP/CALL/MUSIC/FILE/CONTACT/PRESENT/INLINE_KEYBOARD/LOCATION/DAILY_MEDIA/WIDGET/POLL. Поля: `sensitive`/`sensitiveContentUnlocked`/`isProcessingOnServer`/`processingOnServerStatus`. Подробно: `notes/topics/257-protos-attach-schema.md`.

---

## Дополнения 258

### 258. MessageElement + ReactionData + PendingUploadData

`MessageElement`: 12 типов (USER_MENTION/GROUP_MENTION/STRONG/LINK/CODE/QUOTE/ANIMOJI) + entityId/from/length. `ReactionData`: reaction/type(EMOJI/STICKER). `PendingUploadData`: pendingMap. Подробно: `notes/topics/258-protos-message-elements.md`.

---

## Дополнения 259

### 259. Protos.Call + CallHistoryState

`Call`: callType(AUDIO/VIDEO)/hangupType(HANGUP/CANCELED/REJECTED/MISSED)/duration/contactIds[]. `CallHistoryState`: backwardMarker/forwardMarker/MissedMessagesItem.ids[]. Подробно: `notes/topics/259-protos-call-history.md`.

---

## Дополнения 260

### 260. Protos.Contact — схема контакта

`serverPhone`/`gender`/`accountStatus`/`registrationTime`/`lastSearchClickTime`/`organizationIds[]`. `ContactName`: CUSTOM/DEVICE/ONEME. Подробно: `notes/topics/260-protos-contact-schema.md`.

---

## Дополнения 261

### 261. Protos.Control — системные сообщения

12 типов: NEW/ADD/REMOVE/LEAVE/TITLE/ICON/HELLO/SYSTEM/JOIN_BY_LINK/PIN/BOT_STARTED. Поля: event/chatType/userId/userIds[]/pinnedMessageId/showHistory. Подробно: `notes/topics/261-protos-control-message.md`.

---

## Дополнения 262

### 262. Protos.Present + ConfirmPresent — подарки

`Present`: presentId/senderId/receiverId/status(NEW/RECEIVED/ACCEPTED/DECLINED/ACCEPTING). `ConfirmPresent`: accept/asPrivate. Подробно: `notes/topics/262-protos-present-gift.md`.

---

## Дополнения 263

### 263. Protos.Video + Quality

`Video`: videoId/duration/live/startTime/transcriptionStatus/thumbhashData. `Quality`: bitrate/height/width/isOriginal. Подробно: `notes/topics/263-protos-video-schema.md`.

---

## Дополнения 264

### 264. Protos.Audio — аудио-сообщения

`audioId`/`duration`/`startTime`/`transcriptionStatus`(UNKNOWN/PROCESSING/SUCCESS/FAILED/NOT_SUPPORTED/MEDIA_NOT_READY)/`wave`(waveform bytes). Подробно: `notes/topics/264-protos-audio-schema.md`.

---

## Дополнения 265

### 265. Protos.Poll + Photo + File

`Poll.AnswerStats`: userId/timestamp (кто и когда голосовал). `Poll.State.voterPreviewIds[]`. `Photo`: photoId/height/width/gif/thumbhashData. `File`: fileId/size. Подробно: `notes/topics/265-protos-poll-photo-file.md`.

---

## Дополнения 266

### 266. Protos.ChatOptions — 13 флагов

`onlyAdminCanCall`/`contentLevelChat`/`joinRequest`/`membersCanSeePrivateLink`/`signAdmin`/`aPlusChannel`/`official`/`serviceChat`. Подробно: `notes/topics/266-protos-chat-options.md`.

---

## Дополнения 267

### 267. ChatSettings + ChatReactionsSettings

`ChatSettings`: dontDisturbUntil/favoriteIndex/hideLiveLocationPanel/hideLiveLocationPanelBeforeTime/lastNotifMark. `ChatReactionsSettings`: count/included/isActive/isFull. Подробно: `notes/topics/267-protos-chat-settings-reactions.md`.

---

## Дополнения 268

### 268. GroupChatInfo + GroupOptions

`GroupChatInfo`: groupId/isAnswered/isImportant/isMember/isModerator/messagingPermissions(DISABLED/MEMBERS/ALL). `GroupOptions`: groupPremium. Подробно: `notes/topics/268-protos-group-chat-info.md`.

---

## Дополнения 269

### 269. Protos.Sticker

`stickerId`/`setId`/`stickerType`(STATIC/LIVE/POSTCARD/LOTTIE)/`authorType`(SYSTEM/USER)/`audio`. `AnimationProperties`: duration/fps/framesCount/replayDelay. Подробно: `notes/topics/269-protos-sticker.md`.

---

## Дополнения 270

### 270. DraftSave + MsgView + AssetsAdd + SyncChatHistory

`MsgView`: chatId/messageIds[]/registerView/time — **сервер знает, какие сообщения просмотрены**. `DraftSave`: chatId/draft(bytes) — черновики синхронизируются с сервером. Подробно: `notes/topics/270-tasks-draft-msgview-assets.md`.

---

## Дополнения 271

### 271. MsgEdit + MsgReact Tasks

`MsgEdit`: text/editAttaches/**oldText**/oldAttaches/oldElements/oldStatus — **сервер получает старое содержимое при редактировании**. `MsgReact`: reaction/reactionType. Подробно: `notes/topics/271-tasks-msg-edit-react.md`.

---

## Дополнения 272

### 272. MsgDelete Task

`messagesId[]`/`forMe`/`complaint` (удаление + жалоба в одной операции)/`notDeleteMessageFromDb`. Подробно: `notes/topics/272-tasks-msg-delete.md`.

---

## Дополнения 273

### 273. VideoPlay + ChatClear + ChatDelete Tasks

`VideoPlay`: videoId/place/saveToGallery/startDownload. `ChatClear`/`ChatDelete`: forAll/lastEventTime. Подробно: `notes/topics/273-tasks-video-chat-ops.md`.

---

## Дополнения 274

### 274. FileDownloadCmd + MsgSharePreview + ChatPinSetVisibility

`FileDownloadCmd`: fileId/fileName/chatId/messageId. `MsgSharePreview`: messageId/text. `ChatPinSetVisibility`: chatServerId/show. Подробно: `notes/topics/274-tasks-file-share-pin.md`.

---

## Дополнения 275

### 275. Batch Tasks

`ChatMarkBatch`: chatIds[]/maxMark. `DeleteChatsBatch`: chatIds[]. `ChatsList`: chatsSync/count/marker. `DraftDiscard`: chatId. Подробно: `notes/topics/275-tasks-batch-ops.md`.

---

## Дополнения 276

### 276. UpdateFireTimeProtoTask + ChangeProfileOrChatPhoto

`UpdateFireTimeProtoTask`: chatId/messageId/fireTime/notifySender (отложенные сообщения). `ChangeProfileOrChatPhoto`: chatId/file/lastModified/crop. Подробно: `notes/topics/276-tasks-fire-time-photo.md`.

---

## Дополнения 277

### 277. VideoConvert + VideoUpload + WarmChatHistory

`VideoConvert`: srcPath/dstPath/startPosition/endPosition/quality. `VideoUpload`: file/audio. `WarmChatHistory`: chatIds[]. Подробно: `notes/topics/277-tasks-video-warm-history.md`.

---

## Дополнения 278

### 278. PhotoUpload + FileUpload Tasks

`PhotoUpload`: file/chatId/crop/profile. `FileUpload`: file/originalFile/fileName/fileId/attachType/lastUpdatedFile/lastUpdatedOriginalFile. Подробно: `notes/topics/278-tasks-photo-file-upload.md`.

---

## Дополнения 279

### 279. FileDownload + PrepareFileUpload Tasks

`FileDownload`: fileId/fileName/url/place/notCopyVideoToGallery. `PrepareFileUpload`: uri/fileName/messageId. Подробно: `notes/topics/279-tasks-file-download-prepare.md`.

---

## Дополнения 280

### 280. MsgSendCallback + MsgDeleteRange Tasks

`MsgSendCallback`: messageId/callbackId/**payload**/buttonType/buttonPosition(row/column). `MsgDeleteRange`: chatId/startTime/endTime. Подробно: `notes/topics/280-tasks-callback-delete-range.md`.

---

## Дополнения 281

### 281. Assets Tasks + SuspendBot

`AssetsAdd/Remove/ListModify/Move`: управление ассетами (стикеры/GIF). `SuspendBot`: botId/chatId/suspend. Подробно: `notes/topics/281-tasks-assets-suspend-bot.md`.

---

## Дополнения 282

### 282. ChatMark + ChatComplain Tasks

`ChatMark`: chatId/messageId/mark/setAsUnread/isReadReaction (сервер знает, до какого сообщения прочитан чат). `ChatComplain`: chatId/complaint. Подробно: `notes/topics/282-tasks-chat-mark-complain.md`.

---

## Дополнения 283

### 283. LocationStop + MsgCancelReaction + ChangeChatPhoto

`LocationStop`: chatId/messageId. `MsgCancelReaction`: chatId/messageId. `ChangeChatPhoto`: chatId/file/lastModified/crop. Подробно: `notes/topics/283-tasks-location-reaction-photo.md`.

---

## Дополнения 284

### 284. ExternalVideoSend + CongratsStatus Tasks

`ExternalVideoSend`: chatId/messageId/externalUrl. `CongratsStatus`: userId/holidayId/status. Подробно: `notes/topics/284-tasks-external-video-congrats.md`.

---

## Дополнения 285

### 285. ChatHide + ChatGroupMark + ConfirmPresent

`ChatHide`: chatId. `ChatGroupMark`: chatId/flagType/enabled. `ConfirmPresent`: presentId/accept/asPrivate. Подробно: `notes/topics/285-tasks-chat-hide-group-present.md`.

---

## Дополнения 286

### 286. WS Opcodes — дополнительные

`SESSIONS_INFO`/`SESSIONS_CLOSE`, `PROFILE_DELETE`/`PROFILE_DELETE_TIME`, `WEB_APP_INIT_DATA`, `PHONE_BIND_REQUEST`/`CONFIRM`/`WEBAPP_SHARE`, `FOLDERS_GET/UPDATE/REORDER/DELETE/NOTIF_FOLDERS`, `NOTIF_MSG_REACTIONS_CHANGED`/`NOTIF_MSG_YOU_REACTED`. Подробно: `notes/topics/286-ws-opcodes-additional.md`.

---

## Дополнения 287

### 287. WEB_APP_INIT_DATA

`hash`/`user`/`query_id`/`url`/`trackId`/`email`/`chatReactionsSettings`/`organizations`. **Мини-приложение получает email, организации и UserSettings пользователя**. Подробно: `notes/topics/287-web-app-init-data.md`.

---

## Дополнения 288

### 288. OK_TOKEN WS

`token_refresh_ts`/`token_lifetime_ts`. Сервер обновляет OK-токен авторизации через WS. Подробно: `notes/topics/288-ok-token-ws.md`.

---

## Дополнения 289

### 289. PHONE_WEBAPP_SHARE WS

`phone`/`hash`/`contact`. Номер телефона передаётся мини-приложению через WS. Подробно: `notes/topics/289-phone-webapp-share-ws.md`.

---

## Дополнения 290

### 290. NOTIF_MESSAGE WS

`chat_id`/`message`/`chat`/`mark`/`unread`/`ttl`/`url`/`prevMessageId`/`invisible`. Подробно: `notes/topics/290-notif-message-ws.md`.

---

## Дополнения 291

### 291. NOTIF_MARK + NOTIF_ATTACH WS

`NOTIF_MARK`: chat_id/mark/unread/userId (read receipts — сервер уведомляет, кто прочитал). `NOTIF_ATTACH`: fileId/audioId/videoId/error. Подробно: `notes/topics/291-notif-mark-attach-ws.md`.

---

## Дополнения 292

### 292. NOTIF_MSG_DELETE + NOTIF_FOLDERS WS

`NOTIF_MSG_DELETE`: chat_id/messageIds[]/messages/chat/ttl. `NOTIF_FOLDERS`: folders/banners/showTime/updateTime. Подробно: `notes/topics/292-notif-msg-delete-folders-ws.md`.

---

## Дополнения 293

### 293. NOTIF_MSG_DELAYED + BOT_INFO + COMPLAIN_REASONS_GET WS

`NOTIF_MSG_DELAYED`: messageIds[]/chat_id/lastDelayedUpdateTime/updateTypeId. `BOT_INFO`: commands/contact/startMessage. `COMPLAIN_REASONS_GET`: complainSync/complains. Подробно: `notes/topics/293-notif-delayed-bot-complain-ws.md`.

---

## Дополнения 294

### 294. EXTERNAL_CALLBACK + PHONE_BIND_REQUEST WS

`EXTERNAL_CALLBACK`: botId/startParam/chats. `PHONE_BIND_REQUEST`: blockingDuration/trackId/codeLength. Подробно: `notes/topics/294-external-callback-phone-bind-ws.md`.

---

## Дополнения 295

### 295. NOTIF_CALL_START WS

`conversation_id`/`rejectedParticipants`/`internalCallerParams`. Подробно: `notes/topics/295-notif-call-start-ws.md`.

---

## Дополнения 296

### 296. WS Opcodes — финальный список (148-160)

`AUTH_QR_APPROVE`(QR-авторизация), `VOTERS_LIST_BY_ANSWER`(кто как голосовал), `TRANSCRIBE_MEDIA`/`NOTIF_TRANSCRIPTION`, `MSG_DELIVERY`, `CHAT_SUGGEST`, `ORG_INFO`, `CHAT_LIVESTREAM_INFO`. Итого 160 опкодов. Подробно: `notes/topics/296-ws-opcodes-final.md`.

---

## Дополнения 297

### 297. TRANSCRIBE_MEDIA + NOTIF_TRANSCRIPTION WS

`TRANSCRIBE_MEDIA`: transcription/transcriptionStatus. `NOTIF_TRANSCRIPTION`: messageId/chat_id/**transcription**/mediaId/transcriptionStatus. **Серверная транскрипция** — сервер получает аудио/видео и возвращает текст. Подробно: `notes/topics/297-transcribe-media-ws.md`.

---

## Дополнения 298

### 298. AUDIO_PLAY + SEND_VOTE WS

`AUDIO_PLAY`: opus/mp3/m4a/callName/callerId/messagesReactions. `SEND_VOTE`: trackId/chat/state. Подробно: `notes/topics/298-audio-play-send-vote-ws.md`.

---

## Дополнения 299

### 299. VOTERS_LIST_BY_ANSWER + ORG_INFO + CHAT_LIVESTREAM_INFO WS

`VOTERS_LIST_BY_ANSWER`: voteCount/voters/marker. `ORG_INFO`: organizations/email/user/hash. `CHAT_LIVESTREAM_INFO`: liveStreams. Подробно: `notes/topics/299-voters-org-livestream-ws.md`.

---

## Дополнения 300

### 300. LINK_INFO + AUTH_QR_APPROVE WS

`LINK_INFO`: stickerSet/startPayload/chat/user/group/message/videoConference/banners. `AUTH_QR_APPROVE`: tokenAttrs. Подробно: `notes/topics/300-link-info-auth-qr-ws.md`.

---

## Дополнения 301

### 301. FOLDERS_GET + GET_POLL_UPDATES + NOTIF_PROFILE WS

`FOLDERS_GET`: allFilterExcludeFolders/folderSync/folders/foldersOrder. `GET_POLL_UPDATES`: polls. `NOTIF_PROFILE`: profile. Подробно: `notes/topics/301-folders-polls-profile-ws.md`.

---

## Дополнения 302

### 302. NOTIF_BANNERS WS

`showTime`/`banners`/`updateTime` — все обязательные. Сервер управляет расписанием показа баннеров. Подробно: `notes/topics/302-notif-banners-ws.md`.

---

## Дополнения 303

### 303. UpdateMessageEvent

`chatId`/`messageId`/`reactionsChanged`. Используется в NOTIF_MSG_REACTIONS_CHANGED и NOTIF_MSG_YOU_REACTED. Подробно: `notes/topics/303-update-message-event.md`.

---

## Дополнения 304

### 304. NOTIF_TYPING WS

`type`(VIDEO_MSG/AUDIO_MSG)/`isTyping`. Сервер знает, что именно записывает пользователь (текст/аудио/видео). Подробно: `notes/topics/304-notif-typing-ws.md`.

---

## Дополнения 305

### 305. NOTIF_CONTACT WS

`ContactUpdateAction`: ADD/UPDATE/REMOVE/BLOCK/UNBLOCK. Подробно: `notes/topics/305-notif-contact-ws.md`.

---

## Дополнения 306

### 306. NOTIF_CONTACT_SORT WS

`ids[]`(порядок контактов)/`phones[]`(порядок телефонов). Сервер синхронизирует порядок контактов между устройствами. Подробно: `notes/topics/306-notif-contact-sort-ws.md`.

---

## Дополнения 307

### 307. InboundCall PUSH телеметрия

`p_op`/`chat_id`/`call_id`/`show_source`/`ttime`/`dtime`(задержка доставки)/`fcmdtime`(задержка FCM). Подробно: `notes/topics/307-inbound-call-push-telemetry.md`.

---

## Дополнения 308

### 308. nm4 — парсер контактов и presence

`phonebook_id`/`contact_id`/`phone`/`phone_key`/`server_phone`/`email`/`first_name`/`last_name`/`avatar_path` + presence(`seen`/`status`). Подробно: `notes/topics/308-nm4-contact-presence-parser.md`.

---

## Дополнения 309

### 309. nm4 — дополнительные схемы

Чаты: id/server_id/data. Загрузки видео: quality/trim positions. FCM: text/sender/push_id/bmd. Подробно: `notes/topics/309-nm4-additional-schemas.md`.

---

## Дополнения 310

### 310. presence DB

`contactServerId`/`seen`(время последнего визита)/`status`. Локальный кэш статусов присутствия. Подробно: `notes/topics/310-presence-db-schema.md`.

---

## Дополнения 311

### 311. contacts + phones DB

`contacts`: id/server_id/data(BLOB protobuf). `phones`: phonebook_id/contact_id/phone/phone_key/server_phone/email/first_name/last_name/avatar_path. Подробно: `notes/topics/311-contacts-phones-db.md`.

---

## Дополнения 312

### 312. stickers + sticker_sets + profile DB

`stickers`: 16 полей (url/mp4_url/lottie_url/audio/tags). `sticker_sets`: name/icon_url/author_id/link. `profile`: id/server_id/profile(BLOB). Подробно: `notes/topics/312-stickers-profile-db.md`.

---

## Дополнения 313

### 313. animoji + animoji_set + organizations DB

`animoji`: id/emoji/lottie_url/lottie_play_url/set_id. `animoji_set`: name/icon_url/icon_lottie_url/animoji_ids. `organizations`: +iconUrl. Подробно: `notes/topics/313-animoji-organizations-db.md`.

---

## Дополнения 314

### 314. folder_and_chats + message_uploads + notifications_read_marks DB

`folder_and_chats`: chatId/folderId. `message_uploads`: path/upload_type/video_quality/trim. `notifications_read_marks`: chat_id/mark. Подробно: `notes/topics/314-folder-chats-uploads-marks-db.md`.

---

## Дополнения 315

### 315. comments + complain_reasons DB

`comments`: 28 полей (аналог messages + parent_chat_server_id/parent_message_server_id). `complain_reasons`: id/type_id/complain_reasons(BLOB). Подробно: `notes/topics/315-comments-complain-reasons-db.md`.

---

## Дополнения 316

### 316. BatterySnapshot

`slice`/`cpuTicks`(u/s/cu/cs)/`batteryPercent`/`mobileNet`(rx/tx/idle)/`wifiNet`(rx/tx/idle)/`processes`. Подробно: `notes/topics/316-battery-snapshot.md`.

---

## Дополнения 317

### 317. EnrichedBatterySnapshot

`snapshot`(BatterySnapshot)/`seqId`/`visibility`(FG/BG). Подробно: `notes/topics/317-enriched-battery-snapshot.md`.

---

## Дополнения 318

### 318. /proc filesystem reads

`/proc/self/stat`(CPU ticks)/`/proc/self/statm`(память)/`/proc/<pid>/cmdline`(имя процесса)/`/proc/self/fd/`(файловые дескрипторы). Подробно: `notes/topics/318-proc-filesystem-reads.md`.

---

## Дополнения 319

### 319. TrafficStats — сетевая статистика по UID

`getUidRxBytes`/`getUidTxBytes`(uid) — входящий/исходящий трафик MAX по UID. Используется в BatterySnapshot. Подробно: `notes/topics/319-traffic-stats-uid.md`.

---

## Дополнения 320

### 320. HealthStats API

`SystemHealthManager.takeMyUidSnapshot()`: mobile(rx/tx/idle)/wifi(rx/tx/idle). Fallback на TrafficStats. Подробно: `notes/topics/320-health-stats-api.md`.

---

## Дополнения 321

### 321. Apptracer perf/upload

`https://sdk-api.apptracer.ru/api/perf/upload?crashToken=<token>` — загрузка BatterySnapshot (CPU/battery/network) на Apptracer. Подробно: `notes/topics/321-apptracer-perf-upload.md`.

---

## Дополнения 322

### 322. Apptracer sample/initUpload

`https://sdk-api.apptracer.ru/api/sample/initUpload?sampleToken=<token>` — загрузка произвольных файлов (heap dumps, CPU profiles) с feature/sampleSize/attr1/attr2/tag. Подробно: `notes/topics/322-apptracer-sample-upload.md`.

---

## Дополнения 323

### 323. Apptracer — все endpoints

`api/crash/trackSession`(device_id/sessions/drops) + `api/perf/upload`(samples) + `api/sample/initUpload`+`api/sample/upload`. Подробно: `notes/topics/323-apptracer-all-endpoints.md`.

---

## Дополнения 324

### 324. DiskUsageWorker

`int_data`(applicationInfo.dataDir)/`ext_data`(externalFilesDir)/`total_size`. Рекурсивный список файлов (до 20). Подробно: `notes/topics/324-disk-usage-worker.md`.

---

## Дополнения 325

### 325. Apptracer SDK — компоненты

`CrashReport`+`HeapDump`+`PerformanceMetrics`+`DiskUsage`+`SamplingProfiler`+`NativeBridge`+`SampleUpload`. Подробно: `notes/topics/325-apptracer-sdk-components.md`.

---

## Дополнения 326

### 326. NativeBridge — JNI мост

`nativeInstallBridge(qb5.Z)` — устанавливает JNI мост. `NativeBridge`: log(msg)/setKey(key, value). Подробно: `notes/topics/326-native-bridge-tracer.md`.

---

## Дополнения 327

### 327. MyTracker — endpoints

`tracker-api.vk-analytics.ru/v3/`(main) + `ip4.`(IPv4) + `ts./mobile/v1`(timestamp) + `mlapi.`(ML) + `beta-ml.`(Beta ML). Подробно: `notes/topics/327-mytracker-endpoints.md`.

---

## Дополнения 328

### 328. MyTracker инициализация

`initTracker("34982109644049932883", app)` + `setCustomUserId(userId)` + `setKidMode(false)`. Tracker ID MAX: `34982109644049932883`. Подробно: `notes/topics/328-mytracker-init.md`.

---

## Дополнения 329

### 329. MyTracker — рекламные идентификаторы

Google GAID + Huawei OAID + android_id + mac. Подробно: `notes/topics/329-mytracker-advertising-ids.md`.

---

## Дополнения 330

### 330. MyTracker DeviceParamsDataProvider

Build.DEVICE/MANUFACTURER/MODEL/VERSION + screen(width/height/density) + timezone + freeSpace + currentModeType + hasSystemFeature(touchscreen). Подробно: `notes/topics/330-mytracker-device-params.md`.

---

## Дополнения 331

### 331. MyTrackerUserLifecycle

`trackLoginEvent(userId, null)` + `trackRegistrationEvent(userId, null)` + `trackInviteEvent()`. Подробно: `notes/topics/331-mytracker-user-lifecycle.md`.

---

## Дополнения 332

### 332. MyTracker ActivityLifecycle

`onActivityStarted(timePoint)`/`onActivityStopped(timePoint)`. Основа для подсчёта времени сессии. Подробно: `notes/topics/332-mytracker-activity-lifecycle.md`.

---

## Дополнения 333

### 333. MyTracker Install Referrer

`installReferrer`/`installBeginTimestamp`/`referrerClickTimestamp`/`installer`. Google + Huawei. Подробно: `notes/topics/333-mytracker-install-referrer.md`.

---

## Дополнения 334

### 334. MyTracker Event Types

`session`(3L/11)/`launch`(2L/13)/`referrer`(1L/12)/`install_referrer`(5L/17)/`custom`(33L/41). Подробно: `notes/topics/334-mytracker-event-types.md`.

---

## Дополнения 335

### 335. MyTracker ExternalProtoWriters

`REMOTE_CONFIG_STRING`(1)/`LOCATION_INFO`(2)/`NETWORK_INFO`(3)/`CELL_AND_WIFI_INFO`(4). Подробно: `notes/topics/335-mytracker-external-proto.md`.

---

## Дополнения 336

### 336. ilb.java — расширенные данные устройства

`board`/`brand`/`cpuABI`(Build.SUPPORTED_ABIS)/`cpuCount`/`networkOperatorName`/`installer`. Подробно: `notes/topics/336-ilb-device-data.md`.

---

## Дополнения 337

### 337. Crash Report Device Info

`environment`/`buildUuid`/`sessionUuid`/`device_id`/`inBackground`/`connection`/`isRooted`. Подробно: `notes/topics/337-crash-report-device-info.md`.

---

## Дополнения 338

### 338. Root Detection

Build.PRODUCT(sdk/google_sdk) + Build.TAGS(test-keys) + /system/app/Superuser.apk + /system/xbin/su. Эмулятор не считается рутированным. Подробно: `notes/topics/338-root-detection-impl.md`.

---

## Дополнения 339

### 339. Minidump crash handler

`installMinidumpWriterImpl(getCacheDir/tracer/minidump)` via libtracernative. Подробно: `notes/topics/339-minidump-crash-handler.md`.

---

## Дополнения 340

### 340. CrashReportInitializer — ApplicationExitInfo

`getReason()`/`getProcessName()`/`getTimestamp()`/`getTraceInputStream()`(ANR trace). ANR trace загружается на Apptracer. Подробно: `notes/topics/340-crash-report-exit-info.md`.

---

## Дополнения 341

### 341. Apptracer Crash Types

10 типов: CRASH/NON_FATAL/FATAL/ERROR/WARNING/NOTICE/INFO/DEBUG/MINIDUMP/ANR. Путь: getCacheDir/tracer/crashes. Подробно: `notes/topics/341-apptracer-crash-types.md`.

---

## Дополнения 342

### 342. PmsKey — новые флаги

`mytracker-enabled`/`tracer-non-fatal-crashed-enabled`/`wm-*`(WorkManager)/`calc-audio-wave`/`battery-slice-interval`/`ymap`/`views-count-enabled`. Подробно: `notes/topics/342-pmskey-additional-new.md`.

---

## Дополнения 343

### 343. rtd SharedPreferences — 40+ флагов

`userLogReportChatId`/`isMytrackerEnabled`/`statSessionBackgroundThreshold`/`retryTranscriptionAttempt`/`vpnChatBottomsheetEnabled`/`vpnCallBottomsheetEnabled`. Подробно: `notes/topics/343-rtd-server-flags.md`.

---

## Дополнения 344

### 344. PmsKey debug/special

`user-debug-report`/`fake-chats`/`fake-in-app-review`/`blocked-users`/`anr-config`/`watchdog-config`/`debug-mode`/`min-log-level`. Подробно: `notes/topics/344-pmskey-debug-special.md`.

---

## Дополнения 345

### 345. EXIF GPS метаданные

`GPSLatitude`/`GPSLongitude`/`GPSAltitude`/`GPSTimeStamp` + `CameraOwnerName`/`BodySerialNumber`/`LensSerialNumber`. Подробно: `notes/topics/345-exif-gps-metadata.md`.

---

## Дополнения 346

### 346. Fused Location Provider

`latitude`/`longitude`/`altitude`/`accuracy`/`bearing`/`speed` → sj9(LocationData). Google Play Services. Подробно: `notes/topics/346-fused-location-provider.md`.

---

## Дополнения 347

### 347. Firebase Messaging Service

`RECEIVE`/`NEW_TOKEN`. Поля: `google.message_id`/`google.product_id`/`error`. Подробно: `notes/topics/347-firebase-messaging-service.md`.

---

## Дополнения 348

### 348. Firebase CctTransportBackend

POST+gzip+`X-Goog-Api-Key`+`datatransport/3.1.9`. URL задаётся динамически. Подробно: `notes/topics/348-firebase-cct-transport.md`.

---

## Дополнения 349

### 349. Firebase Installations API

`https://firebaseinstallations.googleapis.com/v1/<path>`. FID — уникальный ID установки. Подробно: `notes/topics/349-firebase-installations-api.md`.

---

## Дополнения 350

### 350. Firebase Installations — данные

`fid`/`appId`/`authVersion(FIS_v2)`/`sdkVersion(a:18.0.0)`. Данные уходят в Google. Подробно: `notes/topics/350-firebase-installations-data.md`.

---

## Дополнения 351

### 351. Firebase конфигурация

`google_api_key=AIzaSyABuDYeeDXIOrKTXLkUj30Ii143ofPe63Q`, `google_app_id=1:659634599081:android:9605285443b661167225b8`, `gcm_defaultSenderId=659634599081`, `project_id=max-messenger-app`. Подробно: `notes/topics/351-firebase-config-keys.md`.

---

## Дополнения 352

### 352. Apptracer App Token

`tracer_app_token=t6QnlHov0Gq1UBGYG9GPqZu0EiVMZ922FKvwyAEASa90` — crashToken для всех Apptracer API запросов. Подробно: `notes/topics/352-apptracer-app-token.md`.

---

## Дополнения 353

### 353. strings.xml — дополнительные конфиги

`tracer_mapping_uuid=47afde00-4ab5-11f1-9a3d-0c152d90928f`. Юридические ссылки: `legal.max.ru/pp`/`legal.max.ru/ps`. Contact MIME: `vnd.android.cursor.item/vnd.ru.ok.tamtam.android.profile`. Подробно: `notes/topics/353-strings-additional-config.md`.

---

## Дополнения 354

### 354. y-map PmsKey — Яндекс.Карты конфигурация

`tile`/`geocoder`/`static`/`logoLight`/`logoDark`. Яндекс.Карты API ключ приходит с сервера. Геокодер: `geocode-maps.yandex.ru/v1?...&apikey=<server_key>`. Подробно: `notes/topics/354-yandex-maps-config.md`.

---

## Дополнения 355

### 355. Deeplink маршруты — полный список

`:auth`/`:share-self-out`/`joincall/<id>`/`join/<id>`/`stickerset/<id>`/`c/<chatId>/<msgId>`/`:folder`/`:current`. Подробно: `notes/topics/355-deeplink-routes-full.md`.

---

## Дополнения 356

### 356. api.oneme.ru — серверы

`api.oneme.ru`(prod) + `api-test.oneme.ru` + `api-tg.oneme.ru` + `api-test2.oneme.ru`. Подробно: `notes/topics/356-api-oneme-ru-servers.md`.

---

## Дополнения 357

### 357. HostReachabilityChecker — полный список

`gstatic.com`/`mtalk.googl.com`/`calls.okcdn.ru`/`gosuslugi.ru`/`pushtrs.push.hicloud.com`. Хосты закодированы как int-массивы. Подробно: `notes/topics/357-host-reachability-full.md`.

---

## Дополнения 358

### 358. api.ok.ru URI builder

`https://api.ok.ru` + `ok://api/api/<method>` URI scheme. Calls SDK использует OK.ru API. Подробно: `notes/topics/358-api-ok-ru-uri.md`.

---

## Дополнения 359

### 359. z8f.a — декодированные строки

IP-определение: `api.ipify.org`/`checkip.amazonaws.com`/`ifconfig.me`/`ip.mail.ru`/`ipv4-internet.yandex.net`/`ipv6-internet.yandex.net`. Также `trace-flow.ru`. Подробно: `notes/topics/359-z8f-decoded-strings.md`.

---

## Дополнения 360

### 360. z8f.a — дополнительные строки

DPS: `dps_config.bin`/`dps_dont_report.bin`/`dps_log_entries.bin`. HostReachability JSON: appVersion/clientTs/connectionType/deviceId/hosts/ip/operator/status/uid/vpn. Подробно: `notes/topics/360-z8f-decoded-additional.md`.

---

## Дополнения 361

### 361. HostReachability bzk конфигурация

`reportHosts=[trace-flow.ru]` + `selfIpDomains=[yandex/ifconfig.me/ipify/aws/mail.ru]` + `timeout=10000` + `maxSnapshots=50`. Подробно: `notes/topics/361-host-reachability-bzk-config.md`.

---

## Дополнения 362

### 362. z8f DPS метаданные

`ru.trace_flow.dps.API_KEY`/`CLIENT_VERSION`/`USER_ID`. HostReachability: reachabilityHosts/reportHosts/selfIpDetectionDomains/sampleRate/snapshotLifetimeMs/timeoutMs/maxSnapshots. Подробно: `notes/topics/362-z8f-dps-metadata.md`.

---

## Дополнения 363

### 363. Google Maps API Key

`com.google.android.geo.API_KEY=AIzaSyDJbuC3fODS_aR7jcOkoP6qWIsQen9XARI`. Подробно: `notes/topics/363-google-maps-api-key.md`.

---

## Дополнения 364

### 364. AndroidManifest meta-data

7 Apptracer инициализаторов + `com.google.android.geo.API_KEY` + `com.facebook.soloader.enabled=false` + `STAMP_TYPE_DISTRIBUTION_APK`. Подробно: `notes/topics/364-manifest-metadata-full.md`.

---

## Дополнения 365

### 365. Facebook Fresco

GIF/WebP анимации + 5 потоков (FrescoIoBoundExecutor/FrescoDecodeExecutor/FrescoBackgroundExecutor/FrescoLightWeightBackgroundExecutor/FrescoAnimationWorker) + SoLoader + stat.fresco. Подробно: `notes/topics/365-facebook-fresco.md`.

---

## Дополнения 366

### 366. ZstdUtil

`nativeDecompress(bytes)` — только декомпрессия. Данные сжимаются на сервере. Подробно: `notes/topics/366-zstd-util.md`.

---

## Дополнения 367

### 367. LinkInterceptorActivity — MyTracker deeplink

`URI → MyTracker.handleDeeplink(intent)` — все deeplink-и проходят через MyTracker. Подробно: `notes/topics/367-link-interceptor-mytracker.md`.

---

## Дополнения 368

### 368. WebAppHttpClient.WebAppHasVpnException

VPN обнаружен в WebApp HTTP клиенте. Мини-приложения блокируются при VPN. Подробно: `notes/topics/368-webapp-vpn-exception.md`.

---

## Дополнения 369

### 369. DownloadFileFromWebAppWorker

`fileName`/`fileUrl`. ForegroundWorker для загрузки файлов из мини-приложений. Подробно: `notes/topics/369-download-file-from-webapp.md`.

---

## Дополнения 370

### 370. tamtam.api исключения

`NonceException`/`SessionSendLimitException`/`UnknownOpcodeException`/`MaxRetryCountExceededException`/`InvalidParsePresenceException`. Подробно: `notes/topics/370-tamtam-api-exceptions.md`.

---

## Дополнения 371

### 371. MediaProjectionService

`startForeground(12, notification)`. Foreground Service для захвата экрана в звонках. Подробно: `notes/topics/371-media-projection-service.md`.

---

## Дополнения 372

### 372. NotificationTamService + BootCompletedReceiver

`directReply`/`pushId`/`eventKey`. `BOOT_COMPLETED` → `BackgroundWake`. Подробно: `notes/topics/372-notification-service-boot-receiver.md`.

---

## Дополнения 373

### 373. OneLog телеметрия

`log.externalLog`. Поля: collector/operation/uid/network/type/count/datum/group/custom. Application: `packageName:versionCode:versionName`. Platform: `android:phone/tablet:OS_version`. Подробно: `notes/topics/373-onelog-telemetry.md`.

---

## Дополнения 374

### 374. LZ4 + Zstd

`nativeDecompress(ByteBuffer)` + `nativeDecompress(byte[])`. Оба — только декомпрессия. Данные сжимаются на сервере. Подробно: `notes/topics/374-compression-lz4-zstd.md`.

---

## Дополнения 375

### 375. CallAnalyticsSender

`send(event)`/`setIdle`/`forceSend`. `DEFAULT_DISABLE_UPLOAD_IN_CALL=true`. Поля: collector/application/sdk_type/sdk_version/items. Подробно: `notes/topics/375-call-analytics-sender.md`.

---

## Дополнения 376

### 376. AsrOnlineManager — серверный ASR

`enableAsrOnline()` → SignalingProvider. При серверной топологии включается **автоматически**. Результат: `AsrOnlineChunk(participantId, text)` — транскрипция с атрибуцией по участникам. Подробно: `notes/topics/376-asr-online-manager.md`.

---

## Дополнения 377

### 377. RecordManager

`record-start {movieId, name, privacy="PUBLIC", isStream}`. Запись сохраняется на сервере. `privacy="PUBLIC"` по умолчанию. Поддерживается стриминг. Подробно: `notes/topics/377-record-manager.md`.

---

## Дополнения 378

### 378. P2PRelay + SessionRooms

`P2pRelaySwitchTrigger`: RTT >= threshold (с сервера) N раз → relay. `SessionRoomsManager`: joinRoom/leaveRoom/requestAttention + admin: assignParticipants/moveParticipant/activateRooms. Подробно: `notes/topics/378-p2prelay-sessionroom.md`.

---

## Дополнения 379

### 379. WatchTogether

`play/pause/resume/stop/setPosition`. `MovieState(participantId, position, isPlaying, volume, isMuted)`. Транспорт: WebTransport с сжатием. Подробно: `notes/topics/379-watch-together.md`.

---

## Дополнения 380

### 380. MLFeaturesManager

KWS (`ws_0`) + NS (`ns_1`). Конфиг с сервера: `{url, checksum, enabled}`. Remote keys: `android.mlfeatures.ws_0` / `android.mlfeatures.ns_1`. Файлы: `.tflite` + `.cfg`. Сервер контролирует URL модели. Подробно: `notes/topics/380-ml-features-manager.md`.

---

## Дополнения 381

### 381. WaitingRoom + UrlSharing + Feedback + StereoRoom

`get-waiting-hall`. `UrlSharingInfo(url, initiatorId)`. `Feedback(key, source=USER_CLICK/GESTURES/UNKNOWN)`. `StereoRoom`: promoteParticipant/grantAdmin/revokeAdmin/handsQueue. Подробно: `notes/topics/381-waiting-room-urlsharing-feedback-stereo.md`.

---

## Дополнения 382

### 382. ConversationStats

18 компонентов. `bad_call_detected_by_audio_spotter(confidence)` — KWS сработал. `call_finish(reason, rate_reasons)`. `call_start(callType, warmupStatus)`. `client_requested_server_topology`. `ml_ready_to_use(modelId, durationMs)`. Подробно: `notes/topics/382-conversation-stats.md`.

---

## Дополнения 383

### 383. externcalls.sdk.net

`DownloadService`: HTTP + MD5 validation. `NetworkStat(rttMs, audioLoss, videoLoss, activeCandidateType)`. `StatMonitor.observeStat()` → поток NetworkStat. Подробно: `notes/topics/383-externcalls-sdk-net.md`.

---

## Дополнения 384

### 384. externcalls.sdk.config

`RemoteSettings`: android.wordspotter.config/android.dump.bitrate/android.p2prelay.config. `ConversationParams`: isP2PForbidden/ispAsNo/ispAsOrg/locCc/locReg. Подробно: `notes/topics/384-externcalls-sdk-config.md`.

---

## Дополнения 385

### 385. MyTracker детали

10 модулей (RemoteConfig/Antifraud/MiniApps/...). `MyTrackerParams`: age/gender/email/phone/okId/vkId/vkConnectId/icqId. `AntiFraudConfig`: все датчики включены. `installedPackagesProvider`. Huawei HMS Install Referrer. Подробно: `notes/topics/385-mytracker-details.md`.

---

## Дополнения 386

### 386. ShortcutBadger

`applyCount(ctx, n)` — счётчик на иконке. 13 лаунчеров. Стандартная OSS библиотека. Подробно: `notes/topics/386-shortcut-badger.md`.

---

## 387. ИТОГОВЫЙ СВОДНЫЙ АНАЛИЗ

Полный анализ MAX 26.15.3. 386 тем. 12 категорий: идентификация/ASR/запись/серверный контроль/профилирование/push/VPN-блокировка. Подробно: `notes/topics/387-final-summary.md`.

---

## Дополнения 388

### 388. OkApiServiceInternal

13 методов. `sendSupportedCodecsStatistics`. `getExternalIdsByOkIds`/`getOkIdsByExternalIds` (деанонимизация). `requestUploadUrl`. `hangupConversation(reason)`. Подробно: `notes/topics/388-ok-api-service-internal.md`.

---

## Дополнения 389

### 389. vchat API запросы

`vchat.startConversation`: capabilities(hex)/turnServers/waitForAdmin. `vchat.joinConversation` → `{endpoint, wtEndpoint, p2pForbidden}`. `vchat.clientSupportedCodecs` → fingerprinting. `vchat.hangupConversation(reason)`. Подробно: `notes/topics/389-vchat-api-requests.md`.

---

## Дополнения 390

### 390. ClientCapabilities

15 битов, hex-encoded. Default: SCREEN_TRACK_PRODUCER/VIDEO_TRACKS/WAITING_HALL. Передаётся при каждом `startConversation`/`joinConversation`. Подробно: `notes/topics/390-client-capabilities.md`.

---

## Дополнения 391

### 391. ConversationEventsListener

40+ callbacks. `onParticipantsDeAnonymized`/`onMicrophoneForciblyMuted`/`onMigratedToServerTopology`/`onOpponentFingerprintChanged`/`onCustomData(JSONObject)`. Подробно: `notes/topics/391-conversation-events-listener.md`.

---

## Дополнения 392

### 392. ConversationParticipant

`capabilities`/`networkStatus`/`isTalking`/`deAnonymize`. `getAcceptedCallClientType()`/`getAcceptedCallPlatform()`. Подробно: `notes/topics/392-conversation-participant.md`.

---

## Дополнения 393

### 393. RateManager

`RateHint(reason)` по RTT/loss/candidateType. `RateCallData(maxRate, questions)` с сервера. Конфиг: `android.rating.limits`. Подробно: `notes/topics/393-rate-manager.md`.

---

## Дополнения 394

### 394. Audio SDK

`KeywordSpotterManager`: `setKeywordSpotterParams(isEnabled, filePath)`, `KeywordSpotterConfig(turnOffInMs)`. `MicrophoneManager`: `registerAudioSampleCallback(delay, callback)` — сырые аудио-сэмплы. Подробно: `notes/topics/394-audio-sdk-kws-mic-proximity.md`.

---

## Дополнения 395

### 395. KeywordSpotterManagerImpl

`android.wordspotter.config` → `{turn_off_in_ms}`. `setKeywordSpotterParams(isEnabled, filePath)` → нативный KWS → `onKeyword(confidence)` → `bad_call_detected_by_audio_spotter` на сервер. Подробно: `notes/topics/395-keyword-spotter-impl.md`.

---

## Дополнения 396

### 396. NoiseSuppressionManagerImpl

`serversideBasic`/`serversideAnn` — NS на сервере (аудио передаётся на сервер). `clientsideAnn(filePath)` — нейросетевое NS на устройстве. Подробно: `notes/topics/396-noise-suppression-impl.md`.

---

## Дополнения 397

### 397. Video SDK

`ScreenCaptureManager`: setScreenCaptureEnabled/setAudioCaptureEnabled. `CameraManager`: setCameraEnabled/switchCamera. `DisplayLayoutSender.sendDisplayLayouts(items)` — сервер знает расположение видео-окон. Подробно: `notes/topics/397-video-sdk-screen-camera.md`.

---

## Дополнения 398

### 398. MediaMuteManager

`updateMediaOptionsForParticipant`/`updateMediaOptionsForAll` — принудительное изменение медиа-опций. `requestToEnableMediaForParticipant`/`requestToEnableMediaForAll`. Подробно: `notes/topics/398-media-mute-manager.md`.

---

## Дополнения 399

### 399. AsrManager

`asr-start {fileName}`/`asr-stop`. `AsrInfo(initiator, movieId)`. Отдельно от серверного `AsrOnlineManager`. Подробно: `notes/topics/399-asr-manager-record.md`.

---

## Дополнения 400

### 400. ChatManager + ContactCallManager

`InboundMessage(senderId, text, isDirect)`. `OutboundMessage(participantId, text)`. `ContactCallManager`: getIAmAnonymous/getIWasInitiallyAnonymous. Подробно: `notes/topics/400-chat-manager-contacts.md`.

---

## Дополнения 401

### 401. MediaDumpManager

`requestMediaDump(duration, audio, video)` → signaling `collect-debug-dump {audio, video, duration}`. 6 источников аудио. **В production сборке**. Подробно: `notes/topics/401-media-dump-manager.md`.

---

## Дополнения 402

### 402. DebugManager

`enableFullAudioDump(path)`, `registerStatListener(period)`, `setVideoSettingsOverride(maxDimension)`. **В production сборке**. Подробно: `notes/topics/402-debug-manager.md`.

---

## Дополнения 403

### 403. ConversationFeatureManager

4 фичи: ADD_PARTICIPANT/RECORD/MOVIE_SHARE/ASR_RECORD. `enableFeatureForRoles(feature, roles)` через signaling. Подробно: `notes/topics/403-conversation-feature-manager.md`.

---

## Дополнения 404

### 404. AdvertisingIdClient

`getAdvertisingIdInfo(ctx)` → `Info(id=GAID, isLimitAdTrackingEnabled)`. Логирует `limit_ad_tracking`/`ad_id_size`/`time_spent`. Подробно: `notes/topics/404-advertising-id-client.md`.

---

## Дополнения 405

### 405. InstallReferrer

`installReferrer`/`installBeginTimestamp`/`referrerClickTimestamp`. Передаётся в MyTracker для атрибуции. Подробно: `notes/topics/405-install-referrer.md`.

---

## Дополнения 406

### 406. MyTrackerUserLifecycle

`trackLoginEvent(userId, type)` / `trackRegistrationEvent(userId, type)` / `trackInviteEvent()`. `MultipleInstallReceiver`: перехват `INSTALL_REFERRER`. Подробно: `notes/topics/406-mytracker-user-lifecycle.md`.

---

## Дополнения 407

### 407. ShareData

9 типов: TEXT/IMAGES/VIDEOS/FILES/VCARD/MESSAGES/CONTACT/STICKER_SET. Поля: text/images/videos/files/vcard/ids. Подробно: `notes/topics/407-share-data.md`.

---

## Дополнения 408

### 408. PmsKey debug/logging/transcription

`enable-audio-messages-transcription`/`enable-video-messages-transcription` — транскрипция сообщений. `log-sensitive`/`log-messages-meta`/`log-chat-meta`. `calls-fakeboss-incoming-call-enabled`. `battery-slice-interval`. Подробно: `notes/topics/408-pmskey-debug-logging-transcription.md`.

---

## Дополнения 409

### 409. Транскрипция аудио/видео сообщений

`Protos.Audio.transcription`/`Protos.Video.transcription`. `audio.transcription.enabled = true` по умолчанию. PmsKey: `enable-audio-messages-transcription`/`enable-video-messages-transcription`. Подробно: `notes/topics/409-audio-video-transcription.md`.

---

## Дополнения 410

### 410. UserSettings

`audioTranscriptionEnabled`/`hiddenOnline`/`dontDustirbUntil`/`safeMode`/`inactiveTTL`. Синхронизируется с сервером. Подробно: `notes/topics/410-user-settings-map.md`.

---

## Дополнения 411

### 411. UserSettings (sgj) полный

27 полей. Ключевые: `hiddenOnline`/`phoneNumberPrivacy`/`safeMode`/`safeModeNoPin`/`audioTranscriptionEnabled`/`contentLevelAccess`/`familyProtection`. Подробно: `notes/topics/411-user-settings-full.md`.

---

## Дополнения 412

### 412. Protos.Location

`latitude`/`longitude`/`altitude`/`accuracy`/`bearing`/`livePeriod`/`deviceId`. `Protos.LocationInfo`: +`speed`/`time`. Подробно: `notes/topics/412-protos-location.md`.

---

## Дополнения 413

### 413. Conversation interface

23 менеджера. `sendData(participant, JSON)`/`muteAll()`/`grantRoles()`/`removeParticipant(ban)`/`queryChatHistory()`. Подробно: `notes/topics/413-conversation-interface.md`.

---

## Дополнения 414

### 414. ConversationFactory

SDK v0.1.13. 6 методов создания звонков. `setEmulatedNegotiationErrorType`/`setBackendRenderVmoji`/`setLoadKwsBySdkEnabled`. `joinAnonByLink` — анонимный вход. Подробно: `notes/topics/414-conversation-factory.md`.

---

## Дополнения 415

### 415. zu5 (DPS клиент)

`getNetworkOperator()+getNetworkOperatorName()` при каждой отправке. `deviceIdSupplier`/`userIdSupplier`. 4 потока. Подробно: `notes/topics/415-dps-client-zu5.md`.

---

## Дополнения 416

### 416. vm5 + cgj (Device Info)

`android_id` (Settings.Secure). `cgj`: appVersion/osVersion/locale/deviceLocale/deviceName/screen/timeZone/SUPPORTED_ABIS. Подробно: `notes/topics/416-device-info-vm5-cgj.md`.

---

## Дополнения 417

### 417. MyTracker GAID + OAID

`GoogleAdInfoDataProvider`: GAID + isLimitAdTrackingEnabled. `HuaweiAdInfoDataProvider`: OAID + isLimitAdTrackingEnabled. Подробно: `notes/topics/417-mytracker-gaid-oaid.md`.

---

## Дополнения 418

### 418. MyTracker AppsDataProvider

Список не-системных приложений (packageName + firstInstallTime). Хэш `appsHash` — отправляется при изменении. Подробно: `notes/topics/418-mytracker-installed-packages.md`.

---

## Дополнения 419

### 419. WebApp JS-события (34)

Критические: `WebAppBiometryRequestAuth`/`WebAppNfcEmulateNfcTag`/`WebAppSecureStorageSaveKey`/`WebAppVerifyMobileId`/`WebAppSetupScreenCaptureBehavior`/`WebAppUrlInterceptor`. Подробно: `notes/topics/419-webapp-js-events.md`.

---

## Дополнения 420

### 420. WebApp Storage + Biometry

`SecureStorage`(128/4000 байт) + `DeviceStorage`. `BiometryRequestAuth`/`BiometryRequestAccess`/`BiometryUpdateToken`. Подробно: `notes/topics/420-webapp-storage-biometry.md`.

---

## Дополнения 421

### 421. WebAppRequestPhone

`WebAppRequestPhoneResponse(requestId, phone, hash, authDate)`. Мини-приложение получает номер телефона с хэшем для верификации. Подробно: `notes/topics/421-webapp-request-phone.md`.

---

## Дополнения 422

### 422. WebApp ScreenCapture + CodeReader

`WebAppSetupScreenCaptureBehavior(isEnabled)` → `FLAG_SECURE`. `WebAppOpenCodeReader` → `response(requestId, value)`. Подробно: `notes/topics/422-webapp-screen-capture-qr.md`.

---

## Дополнения 423

### 423. WebApp Share + Haptic

`WebAppShare(url, title, text)`. `WebAppHapticFeedback`: IMPACT/NOTIFICATION/SELECTION. Подробно: `notes/topics/423-webapp-share-haptic.md`.

---

## Дополнения 424

### 424. WebAppNfcService

`HostApduService`. `processCommandApdu(apdu)` → данные от мини-приложения. Логирует все APDU в hex. Эмулирует NFC-карту через HCE. Подробно: `notes/topics/424-webapp-nfc-service.md`.

---

## Дополнения 425

### 425. WebApp UI Control

`ClosingBehavior(needConfirmation)`. `BackButton(isVisible)`. `OpenLink(url)`. `OpenMaxLink(url)`. `ChangeScreenBrightness(maxBrightness)`. Подробно: `notes/topics/425-webapp-ui-control.md`.

---

## Дополнения 426

### 426. webapp_biometry DB

`(id, user_id, bot_id, token, access_requested, access_granted)`. Биометрический токен хранится локально, привязан к user_id + bot_id. Подробно: `notes/topics/426-webapp-biometry-db.md`.

---

## Дополнения 427

### 427. Дополнительные таблицы БД

`battery(sliceTime/utime/stime/batteryCapacity/instantAmperage)`. `metrics(traceId/metricName/spanAndPropertiesDump)`. `fcm_notifications_analytics`. `stat_events`. 38 таблиц всего. Подробно: `notes/topics/427-db-additional-tables.md`.

---

## Дополнения 428

### 428. vw0 Battery+CPU collector

`utime`/`stime`/`batteryCapacity`/`instantAmperage` + сетевой трафик (`mrx`/`mtx`/`wrx`/`wtx`). Интервал: `battery-slice-interval` (PmsKey). Подробно: `notes/topics/428-battery-cpu-collector.md`.

---

## Дополнения 429

### 429. fcm_notifications DB

`fcm_notifications`: chat_id/message_id/type/sender/text/push_id/event_key. `fcm_notifications_analytics`: push_id/analytics_status/fcm_sent_time/received_time/content_length. Подробно: `notes/topics/429-fcm-notifications-db.md`.

---

## Дополнения 430

### 430. DB presence + phones + messages

`presence(contactServerId, seen, status)`. `phones(phone, phone_key, server_phone, email, first_name, last_name)`. `messages.text` — в открытом виде в SQLite. Подробно: `notes/topics/430-db-presence-phones-messages.md`.

---

## Дополнения 431

### 431. DB organizations + informer_banner + tasks

`informer_banner(url/priority/click_time/show_time/close_time)`. `notifications_tracker_messages(fcm/drop_reason)`. `tasks(type/status/fails_count/data)`. Подробно: `notes/topics/431-db-organizations-informer-tasks.md`.

---

## Дополнения 432

### 432. DB uploads + video

`uploads(upload_url/upload_progress/total_bytes/photo_token/thumbhash_base64)`. `draft_uploads(video_quality/start_trim/end_trim/mute)`. `video_conversions`. Подробно: `notes/topics/432-db-uploads-video.md`.

---

## Дополнения 433

### 433. DevMenu детали

`ServerHostBottomSheet` ("Адрес сервера") + `ServerPortBottomSheet`. `FeatureToggles` (источник "SERVER"). `LogsViewer`. `TestCrash`. **В production сборке**. Подробно: `notes/topics/433-devmenu-details.md`.

---

## Дополнения 434

### 434. change-media-settings

`{isVideoEnabled, isAudioEnabled, isScreenSharingEnabled, isAnimojiEnabled, isFastScreenSharingEnabled, isAudioSharingEnabled}`. Подробно: `notes/topics/434-change-media-settings.md`.

---

## Дополнения 435

### 435. WebRTC Experiments (ev1)

30+ флагов. `isAudioCaptureLoggingEnabled`/`bitrateDumpGatheringState`/`isBackendRenderVmojiEnabled`/`isCorruptWsEndpointEnabled`. Подробно: `notes/topics/435-webrtc-experiments.md`.

---

## Дополнения 436

### 436. WebAppContactData

`WebAppContactData(displayName, avatarUrl, abbreviationModel)`. Мини-приложение получает данные профиля пользователя. Подробно: `notes/topics/436-webapp-contact-data.md`.

---

## Дополнения 437

### 437. TwoFA

6 экранов: Settings/Creation/Onboarding/CheckPass/StartRestore/ProfileDeletion. `twofa_settings_track_id_key` для аналитики. Подробно: `notes/topics/437-twofa-screens.md`.

---

## Дополнения 438

### 438. SafeMode

Скрытие профиля + звонки только от контактов + чаты только со знакомыми + фильтр контента. `safeMode`/`safeModeNoPin` в UserSettings. Подробно: `notes/topics/438-safe-mode.md`.

---

## Дополнения 439

### 439. QR Auth

`SettingsDevicesScreen` + `QrAuthHintBottomSheet` + `:qr-scanner?mode=2`. `AuthQrUseCase`/`GetQrCodeUseCase`. Подробно: `notes/topics/439-qr-auth.md`.

---

## Дополнения 440

### 440. VpnConnectedWarningBottomSheet

"Отключите VPN" / "Чтобы пользоваться MAX". Серверно-управляемое предупреждение. Подробно: `notes/topics/440-vpn-warning-bottomsheet.md`.

---

## Дополнения 441

### 441. NeuroAvatarsScreen

Выбор/съёмка фото → генерация нейро-аватара на сервере. Используется при регистрации и редактировании профиля. Подробно: `notes/topics/441-neuro-avatars.md`.

---

## Дополнения 442

### 442. BackgroundListenService

`startForeground(9001)`, "MAX работает в фоне". `system_curtain_shown/hidden`. `BackgroundWakeBootReceiver`: перезапуск при обновлении. Подробно: `notes/topics/442-background-listen-service.md`.

---

## Дополнения 443

### 443. CallsSdkInitializer

`calculateMeta(SHA-256 of .so files)` + `initializeSessionSeed(seed, deviceId)` (native). Подробно: `notes/topics/443-calls-sdk-initializer.md`.

---

## Дополнения 444

### 444. CallAdminSettingsScreen

`ADMIN_CALL_SETTINGS{screen, camera, microphone, screenshare, recording, waiting}`. `CallDebugMenuScreen` в production. Подробно: `notes/topics/444-call-admin-settings.md`.

---

## Дополнения 445

### 445. CallRateBottomSheet + StartRecordBottomSheet + VpnPanelWidget

`CallRateBottomSheet(call_id, is_group_call, is_video_call, sdk_reasons)`. `StartRecordBottomSheet`: имя записи (max 250 символов). `VpnPanelWidget`. Подробно: `notes/topics/445-call-rate-record-vpn.md`.

---

## Дополнения 446

### 446. CallServiceImpl

`ConnectionService` + `WakeLock("max:calls_prx")` + уведомления(incomingCalls/activeCalls). Команды: start/restart/restart_for_screen_sharing. Подробно: `notes/topics/446-call-service-impl.md`.

---

## Дополнения 447

### 447. RknBottomSheet

"Это зарегистрированный канал" / "Он есть в перечне Роскомнадзора". Маркировка каналов РКН. Подробно: `notes/topics/447-rkn-bottomsheet.md`.

---

## Дополнения 448

### 448. WarningLinkBottomSheet

"Перейти по ссылке?" + URL + "Перейти"/"Отменить". Аналитика переходов. Подробно: `notes/topics/448-warning-link-bottomsheet.md`.

---

## Дополнения 449

### 449. FileDownloadWarningBottomSheet

`(chat_id, message_id, attach_id, file_id, file_name, file_url, file_size)`. Предупреждение при скачивании потенциально опасного файла. Подробно: `notes/topics/449-file-download-warning.md`.

---

## Дополнения 450

### 450. FakeInAppReviewBottomSheet

Собственный диалог оценки приложения (не Google Play In-App Review). Оценки собираются напрямую MAX. Подробно: `notes/topics/450-fake-inapp-review.md`.

---

## Дополнения 451

### 451. ForceUpdateScreen

"Ваша версия MAX устарела" / "Обновить". Killswitch — полная блокировка приложения. Подробно: `notes/topics/451-force-update-screen.md`.

---

## Дополнения 452

### 452. AccountInitializer

13 параллельных задач. `SslIntegrity` + `DPS(HIGH)` + `BackgroundWakeFeatureInit`. `getUserId()` + `android_id`. Подробно: `notes/topics/452-account-initializer.md`.

---

## Дополнения 453

### 453. Location screens

`PickLocationScreen(chatId, lat, lon, zoom)`. `ShowLocationScreen(chatId, senderId, msgId, lat, lon, zoom)`. Аналитика: `geolocation_send_click(source_id, source_type)`. Подробно: `notes/topics/453-location-screens.md`.

---

## Дополнения 454

### 454. /proc файлы

`/proc/self/stat` → PID/comm/state/utime/stime. Пути: `/proc/self` + `/data/data/ru.oneme.app`. Подробно: `notes/topics/454-proc-files.md`.

---

## Дополнения 455

### 455. kyk (DPS HTTP клиент)

`TrafficStats.setThreadStatsTag`. `{snapshots: [{id, ...}]}`. Параметры: `timeoutMs`/`dontReportUntil`. Подробно: `notes/topics/455-dps-http-client-kyk.md`.

---

## Дополнения 456

### 456. SSL certificate pinning

SHA-256 fingerprint. `InvalidSslIntegrityException`/`InvalidSslSessionException`. `SslIntegrity` задача при запуске. Подробно: `notes/topics/456-ssl-certificate-pinning.md`.

---

## Дополнения 457

### 457. Root + Network detection

`r04.G()`: root detection (su/Superuser.apk/test-keys). `r04.z()`: network type (NONE/WIFI/ETHERNET/BLUETOOTH/VPN/CELLULAR). Подробно: `notes/topics/457-root-network-detection.md`.

---

## Дополнения 458

### 458. f58 Network operator collection

`connection_type`/`vpn` + `getNetworkOperator():getNetworkOperatorName()` в каждом запросе к API. Подробно: `notes/topics/458-network-operator-collection.md`.

---

## Дополнения 459

### 459. yk2 Device data

`Build.FINGERPRINT` + `mcc_mnc(getSimOperator())` + `tz-offset` + `sdk-version` + `net-type`/`mobile-subtype` + `application_build`. Подробно: `notes/topics/459-device-data-yk2.md`.

---

## Дополнения 460

### 460. yag SharedPreferences

50+ ключей. Ключевые: `device.id`/`user.fcmToken`/`user.okToken`/`server.timeDelta`/`app.first.login.time`/`user.callSession`. Подробно: `notes/topics/460-yag-shared-prefs.md`.

---

## Дополнения 461

### 461. isg LOGIN response

LOGIN ответ → proxy/proxydomains/lang. LOGIN запрос → deviceId/pushDeviceType/buildNumber/timezone. Подробно: `notes/topics/461-login-response-proxy.md`.

---

## Дополнения 462

### 462. NotifConfigLogic

Step2(proxy/proxydomains) → Step3(debug-mode/user-debug-report/safeMode) → Step5(chats settings). Подробно: `notes/topics/462-notif-config-logic.md`.

---

## Дополнения 463

### 463. qp6 RTD (полный список PmsKey)

50+ PmsKey. Критические: `enableaudiomessagestranscription`/`callssdklogaudio`/`logmessagesmeta`/`logchatmeta`/`callsfakebossincomingcallenabled`/`batterysliceinterval`. Подробно: `notes/topics/463-qp6-rtd-pmskey-full.md`.

---

## Дополнения 464

### 464. WatchdogConfig

`stuckThreshold`/`hangThreshold`/`saveStacktrace` с сервера. `keepBackgroundSocket` PmsKey. Подробно: `notes/topics/464-watchdog-config.md`.

---

## Дополнения 465

### 465. DevNullServerConfig

8 флагов: `isOpcodeStatEnabled`/`isMemoryStatEnabled`/`isBatteryStatEnabled`/`isChatHistoryStatEnabled`/`isUploadHangCheckEnabled`. Подробно: `notes/topics/465-devnull-server-config.md`.
