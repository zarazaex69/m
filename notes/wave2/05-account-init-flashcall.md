---
tags: [initialization, startup, account, flash-call, login, sms-retriever, dps, mytracker, background-wake, host-reachability, contacts-sync, surveillance, wave2]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/android/initialization/AccountInitializer.java
  - work_26.16.0/apktool_base/smali_classes2/one/me/login/confirm/ConfirmPhoneScreen.smali
  - work_26.16.0/apktool_base/smali_classes3/y27.smali (SmsRetriever handler)
  - work_26.16.0/apktool_base/AndroidManifest.xml
  - work_26.16.0/apktool_base/res/values/strings.xml
  - work/jadx_base/sources/one/me/login/confirm/ConfirmPhoneScreen.java
related:
  - "[[452-account-initializer]]"
  - "[[24-host-reachability-probe]]"
  - "[[08-background-wake]]"
  - "[[415-dps-client-zu5]]"
  - "[[04-telemetry-endpoints]]"
  - "[[06-contacts]]"
  - "[[530-version-26.16.0-diff]]"
---

# AccountInitializer pipeline + Flash-call login (26.16.0)

## 1. AccountInitializer — полный pipeline задач при логине

`AccountInitializer` (`one.me.android.initialization.AccountInitializer`) запускает **все** фоновые задачи при авторизации через ForkJoinPool. Две фазы: `e()` (критические) и `f()` (отложенные).

### Фаза 1: `e()` — критические задачи (блокируют UI)

| # | Задача | Что делает | Подвох |
|---|--------|-----------|--------|
| 1 | **Scout** | базовая инициализация (DI-граф) | — |
| 2 | **AppTracerCrashService** | запуск Apptracer crash-reporter | телеметрия |
| 3 | **Logger** | инициализация логгера | — |
| 4 | **IoPoolSize** | настройка IO thread pool | — |
| 5 | **Invalidate DB** | инвалидация/миграция БД | — |
| 6 | **Protobuf** | инициализация protobuf | — |
| 7 | **FrescoStartup** | инициализация Fresco (изображения) | — |
| 8 | **LibraryUpgrade** | миграция библиотек | — |
| 9 | **Account** | инициализация аккаунта | — |
| 10 | **AnrWatcher** | ANR-мониторинг | — |
| 11 | **SetupRx** | настройка RxJava | — |
| 12 | **RxJavaPlugins.setErrorHandler** | обработчик ошибок | — |
| 13 | **Chroma.init** | инициализация темы | — |
| 14 | **Fresco** | полная инициализация Fresco | — |
| 15 | **Theme background warmup** | прогрев фона темы | — |
| 16 | **Chroma.dynamicChange** | динамическая тема | — |
| 17 | **DynamicFont** | шрифты | — |
| 18 | **NativeMedia** | нативные медиа-кодеки | — |
| 19 | **EmojiProvider** | эмодзи | — |
| 20 | **Animoji warmup** | прогрев анимодзи | — |
| 21 | **VisibilityController** | контроллер видимости | — |
| 22 | **ProxyChangeListener** | слушатель смены прокси | сетевой мониторинг |
| 23 | **InitialDataStorage.Banners** | загрузка баннеров | — |
| 24 | **InitialDataStorage.Chats** | загрузка чатов из БД | — |
| 25 | **InitialDataStorage.Folders** | загрузка папок из БД | — |
| 26 | **Presences** | загрузка presence-статусов | — |
| 27 | **LegacyChats** | legacy-чаты | — |
| 28 | **DevicePerformanceClass** | определение класса устройства | — |
| 29 | **ServerPayloadCatchMode** | режим перехвата серверных payload | — |
| 30 | **Connect** | установка WS-соединения | — |
| 31 | **ForceUpdateLogic.clearForceUpdateVersionIfNeed** | killswitch-логика | серверный контроль |
| 32 | **ServiceTaskCheckProcessingTasks** | проверка фоновых задач | — |
| 33 | **Legacy.ContactsLoader** | **загрузка контактов** | синхронизация |
| 34 | **Legacy.CallsHistoryLoader** | загрузка истории звонков | — |
| 35 | **Legacy.MessageControllerConsumer** | контроллер сообщений | — |
| 36 | **Legacy.ActivityLifecycleCallbacks** | lifecycle callbacks | — |
| 37 | **RestoreMessageUploads** | восстановление загрузок | — |
| 38 | **Legacy.Phonebook** | **синхронизация телефонной книги** | отправка контактов |
| 39 | **Legacy.SystemServicesManager** | системные сервисы | — |
| 40 | **Legacy.ShortcutsHelper** | ярлыки | — |
| 41 | **PermissionStats** | **статистика пермишнов** | телеметрия |
| 42 | **Legacy.PhoneNumberUtil** | утилита номеров | — |
| 43 | **Legacy.StartupListeners** | startup listeners | — |
| 44 | **Shortcuts and badge warmup** | прогрев бейджей | — |
| 45 | **InAppReviewUncaughtExceptionHandler** | обработчик исключений | — |
| 46 | **HeartbeatScheduler** | планировщик heartbeat | keep-alive |
| 47 | **DbCleanUpScheduler** | очистка БД | — |
| 48 | **Db.NotMainThreadListener** | DB listener | — |
| 49 | **Mytracker** | **инициализация MyTracker** | полный профиль + датчики |
| 50 | **SslIntegrity** | **проверка SSL-целостности** | anti-MITM / anti-debug |
| 51 | **MemoryTrimmableRegistry** | управление памятью | — |
| 52 | **ConcurrencyFeatures** | фичи конкурентности | — |
| 53 | **BackgroundWakeFeatureInit** | **инициализация фоновой работы** | wake-lock / background |
| 54 | **NotificationPermissionObserver** | наблюдатель пермишна нотификаций | — |
| 55 | **Dps** | **инициализация DPS (Data Protection Service)** | серверный контроль |

### Фаза 2: `f()` — отложенные задачи (после UI)

| # | Задача | Что делает | Подвох |
|---|--------|-----------|--------|
| 1 | **AppClockUpdater** | синхронизация часов с сервером | — |
| 2 | **GalleryPrefetch** | предзагрузка галереи | — |
| 3 | **Legacy.TimeChangeReceiver** | слушатель смены времени | — |
| 4 | **Legacy.Theme.ScheduleNightModeCheckIfNeed** | ночная тема | — |
| 5 | **SendInstallInfo** | **отправка информации об установке** | телеметрия |
| 6 | **Legacy.DailyAnalytics** | **ежедневная аналитика** | телеметрия |
| 7 | **NotificationTrackerCleanupScheduler** | очистка трекера нотификаций | — |
| 8 | **MessageCommentsCleanupScheduler** | очистка комментариев | — |
| 9 | **Stickers warmup** | прогрев стикеров | — |
| 10 | **HostReachabilityTask** | **проверка достижимости хостов** | серверный probe |
| 11 | **MlKit** | инициализация ML Kit | — |
| 12 | **unsafe-files migration** | миграция файлов | — |
| 13 | **Fresco:renderscript** | RenderScript для Fresco | — |
| 14 | **Fresco:NativeFilters** | нативные фильтры | — |
| 15 | **MemoryRegistrar** | регистрация памяти | — |
| 16 | **RingtoneMoveFromCacheScheduler** | миграция рингтонов | — |
| 17 | **BatteryRegistrar** | регистрация батареи | — |

### DPS инициализация (метод `a()`)

```java
// Инициализация DPS с ключом "ply5hDvhupghrHVA5rqQD1ypiXAxbmE4A68ZzBa8ioc="
// getUserId() → yag.s() (ID пользователя)
// android_id → yag.m0[45]
// Приоритет: xm5.HIGH
// Конфиг: lkc.g("dps", 0, 2, true, true, 1, 2)
accountInitializer.dps = aVarA.I(xm5Var == xm5.HIGH).N(new p6(accountInitializer)).e();
```

DPS инициализируется **условно** — только если серверный флаг `qp6.S2[97]` == true.

## 2. Задачи с surveillance-потенциалом, запускаемые при логине

### Немедленно (фаза 1):
1. **AppTracerCrashService** → crash + heap-dump uploader на `sdk-api.apptracer.ru`
2. **Legacy.ContactsLoader** + **Legacy.Phonebook** → двусторонняя синхронизация контактов
3. **Mytracker** → полный профиль (age/gender/email/phone/okId/vkId), список приложений, датчики
4. **SslIntegrity** → проверка SSL-пиннинга (anti-debug/anti-MITM)
5. **BackgroundWakeFeatureInit** → инициализация фоновых wake-задач
6. **DPS** → Data Protection Service с серверным управлением
7. **PermissionStats** → отправка статистики пермишнов на сервер
8. **Connect** → WS-соединение (159 опкодов, серверный контроль)

### В первые минуты (фаза 2):
9. **HostReachabilityTask** → TCP-probe к серверному списку хостов + отправка IP/VPN/оператора
10. **SendInstallInfo** → информация об установке
11. **Legacy.DailyAnalytics** → ежедневная аналитика

### Итого при каждом логине:
- Контакты синхронизируются с сервером
- MyTracker получает полный профиль + список приложений + данные датчиков
- Apptracer готов к heap-dump по команде сервера
- HostReachability пробует серверный список хостов и отправляет результат + IP + VPN-статус
- DPS инициализируется с высоким приоритетом (если серверный флаг включён)

---

## 3. Flash-call логин в 26.16.0

### Что изменилось

**26.15.3:**
> «Отправили код на %s»
> «Если не получили СМС, проверьте чат %s в MAX»

**26.16.0:**
> «Звоним на %s»
> «Введите последние 6 цифр входящего номера. Если звонка нет, проверьте СМС и чат %s в MAX»

### Механизм

Flash-call в MAX — это **чисто серверная** операция:
1. Сервер инициирует звонок на номер пользователя
2. Пользователь видит входящий звонок на экране телефона
3. Пользователь **вручную** вводит последние 6 цифр номера звонящего в поле ввода
4. Приложение отправляет введённый код на сервер для верификации

### Что НЕ делает flash-call в MAX

- ❌ **НЕ** регистрирует BroadcastReceiver для `android.intent.action.PHONE_STATE`
- ❌ **НЕ** слушает входящие звонки через PhoneStateListener/TelephonyCallback
- ❌ **НЕ** читает CallerID автоматически
- ❌ **НЕ** извлекает код из номера программно
- ❌ **НЕ** использует `READ_CALL_LOG` или `READ_PHONE_STATE`
- ❌ **НЕ** использует `CallScreeningService`

### Пермишны

**Для flash-call НЕ требуются дополнительные пермишны.** В манифесте 26.16.0 отсутствуют:
- `READ_PHONE_STATE`
- `READ_CALL_LOG`
- `PROCESS_OUTGOING_CALLS`
- `RECEIVE_SMS` (для SMS-fallback используется Google SmsRetriever API, не требующий пермишна)

Единственные phone-related пермишны в манифесте:
- `MANAGE_OWN_CALLS` — для управления собственными VoIP-звонками MAX
- `FOREGROUND_SERVICE_PHONE_CALL` — для foreground service при VoIP

### SMS-fallback: SmsRetriever API

Класс `y27` (в 26.16.0) реализует Google Play Services SmsRetriever:
- Регистрирует BroadcastReceiver для `com.google.android.gms.auth.api.phone.SMS_RETRIEVED`
- При получении SMS извлекает код regex `[0-9]{N}` (где N = `codeLength`, обычно 6)
- Автоматически подставляет код в поле ввода
- **Не требует пермишна** `RECEIVE_SMS` — SmsRetriever API работает только для SMS с app-specific hash

### Есть ли подвох (backdoor через flash-call)?

**НЕТ.** Flash-call в MAX — это самая безопасная реализация из возможных:

1. **Нет listener'а входящих звонков** — приложение не регистрирует никаких receiver'ов для `PHONE_STATE`
2. **Нет автоматического чтения CallerID** — пользователь вводит код вручную
3. **Нет дополнительных пермишнов** — flash-call не требует ничего сверх того, что уже есть
4. **Нет persistent listener'а** — после логина никакой компонент не продолжает слушать звонки
5. **Fallback на SMS** — если звонок не пришёл, пользователь может получить SMS-код

Единственный `PhoneStateListener` в APK (`xna.smali`) — это `TelephonyCallback$DisplayInfoListener`, который слушает **только** `onDisplayInfoChanged` и `onServiceStateChanged` для определения типа сети (5G/NR). Он **не** слушает `CALL_STATE`.

### Диагноз

Flash-call в MAX 26.16.0 — это **оптимизация стоимости** (звонок дешевле SMS для оператора), а не security-изменение и не backdoor. Реализация полностью пассивная со стороны клиента — сервер звонит, пользователь вводит цифры вручную. Никакого программного перехвата входящих звонков нет.

---

## 4. Сводка

### AccountInitializer:
- **55 задач** в фазе 1 (критические) + **17 задач** в фазе 2 (отложенные) = **72 задачи** при каждом запуске
- Из них **11 задач** с surveillance/telemetry потенциалом
- DPS инициализируется условно (серверный флаг)
- HostReachabilityTask запускается в отложенной фазе (первые минуты после логина)
- Контакты + MyTracker + Apptracer — немедленно при логине

### Flash-call:
- **Чисто UI-изменение** — сменили текст с «Отправили код» на «Звоним»
- **Нет backdoor** — нет listener'а входящих звонков, нет автоматического чтения CallerID
- **Нет новых пермишнов** — `READ_PHONE_STATE` / `READ_CALL_LOG` отсутствуют
- **SmsRetriever** (Google API) используется как fallback для автоматического чтения SMS-кода
- Пользователь вводит 6 цифр вручную — самая privacy-friendly реализация flash-call
