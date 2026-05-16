---
tags: [manifest, permissions, components, attack-surface, exported]
status: confirmed
sources:
  - work/apktool_base/AndroidManifest.xml
related:
  - "[[00-INVENTORY]]"
  - "[[07-nfc-hce-webapp]]"
  - "[[08-background-wake]]"
  - "[[12-force-update-killswitch]]"
  - "[[13-deeplinks-idp]]"
---

# Manifest deep-dive: 55 пермов, 26 сервисов, 16 receivers, 8 providers

Полное перечисление компонентов из `AndroidManifest.xml`. Нумерация — для удобства ссылок из других тем.

## 1. Все `<uses-permission>` (37 штатных + 19 кастомных = 56)

Штатные (Android-permissions):

```
ACCESS_COARSE_LOCATION                         ACCESS_FINE_LOCATION
ACCESS_NETWORK_STATE                           ACCESS_WIFI_STATE
BLUETOOTH                                      BLUETOOTH_CONNECT
CAMERA                                         CHANGE_NETWORK_STATE
FOREGROUND_SERVICE                             FOREGROUND_SERVICE_CAMERA
FOREGROUND_SERVICE_DATA_SYNC                   FOREGROUND_SERVICE_MEDIA_PLAYBACK
FOREGROUND_SERVICE_MEDIA_PROJECTION            FOREGROUND_SERVICE_MICROPHONE
FOREGROUND_SERVICE_PHONE_CALL
INTERNET                                       MANAGE_OWN_CALLS
MODIFY_AUDIO_SETTINGS                          NFC
POST_NOTIFICATIONS                             READ_APP_BADGE
READ_CONTACTS                                  READ_EXTERNAL_STORAGE
READ_MEDIA_IMAGES                              READ_MEDIA_VIDEO
READ_MEDIA_VISUAL_USER_SELECTED                RECEIVE_BOOT_COMPLETED
RECORD_AUDIO                                   REQUEST_IGNORE_BATTERY_OPTIMIZATIONS
SYSTEM_ALERT_WINDOW                            USE_BIOMETRIC
USE_FINGERPRINT                                USE_FULL_SCREEN_INTENT
VIBRATE                                        WAKE_LOCK
WRITE_CONTACTS                                 WRITE_EXTERNAL_STORAGE
```

Кастомные / OEM:

```
com.google.android.c2dm.permission.RECEIVE
com.google.android.finsky.permission.BIND_GET_INSTALL_REFERRER_SERVICE
com.google.android.gms.permission.AD_ID                 # доступ к рекламному ID
com.anddoes.launcher.permission.UPDATE_COUNT
com.htc.launcher.permission.READ_SETTINGS
com.htc.launcher.permission.UPDATE_SHORTCUT
com.huawei.android.launcher.permission.CHANGE_BADGE
com.huawei.android.launcher.permission.READ_SETTINGS
com.huawei.android.launcher.permission.WRITE_SETTINGS
com.majeur.launcher.permission.UPDATE_BADGE
com.oppo.launcher.permission.READ_SETTINGS
com.oppo.launcher.permission.WRITE_SETTINGS
com.sec.android.provider.badge.permission.READ
com.sec.android.provider.badge.permission.WRITE
com.sonyericsson.home.permission.BROADCAST_BADGE
com.sonymobile.home.permission.PROVIDER_INSERT_BADGE
me.everything.badger.permission.BADGE_COUNT_READ
me.everything.badger.permission.BADGE_COUNT_WRITE
ru.oneme.app.permission.settings.READ                   # своё разрешение на чтение настроек MAX
```

Большая часть кастомных — про **badge-counter** (число непрочитанных) на разных лаунчерах. Это нормально для мессенджера.

Наиболее опасный комплект:

- `RECORD_AUDIO` + `CAMERA` + `FOREGROUND_SERVICE_MICROPHONE` + `FOREGROUND_SERVICE_CAMERA` + `FOREGROUND_SERVICE_MEDIA_PROJECTION` — звук, камера, запись экрана из foreground-сервиса.
- `MANAGE_OWN_CALLS` + `FOREGROUND_SERVICE_PHONE_CALL` — встраивание в системную звонилку как телефонный аккаунт.
- `RECEIVE_BOOT_COMPLETED` + `REQUEST_IGNORE_BATTERY_OPTIMIZATIONS` + `WAKE_LOCK` + `SYSTEM_ALERT_WINDOW` + `USE_FULL_SCREEN_INTENT` — устойчивое фоновое присутствие, оверлеи, fullscreen-нотификации.
- `READ_CONTACTS` + `WRITE_CONTACTS` — двусторонняя адресная книга (см. [[06-contacts]]).
- `ACCESS_FINE_LOCATION` + `ACCESS_COARSE_LOCATION` — геопозиция (PmsKey `send-location-enabled` управляет, отправлять или нет).
- `NFC` — для HCE мини-апок (см. [[07-nfc-hce-webapp]]).
- `USE_BIOMETRIC` + `USE_FINGERPRINT` — приватный пин-код, разблокировка приложения.

## 2. Manifest-placeholder, не подставленный

```
<uses-permission android:name="android.permission.POST_NOTIFICATIONS"/>
${REQUEST_INSTALL_PACKAGES}
<uses-permission android:name="android.permission.REQUEST_IGNORE_BATTERY_OPTIMIZATIONS"/>
```

Gradle-переменная `${REQUEST_INSTALL_PACKAGES}` сохранилась как литерал. Android-парсер её игнорирует. Факт: при включении флага сборка добавила бы `REQUEST_INSTALL_PACKAGES` — то есть код умеет ставить APK напрямую (вспомогательно к `download.max.ru`-killswitch, см. [[12-force-update-killswitch]]).

## 3. Все services (26 шт.)

| Exported | fgType | Permission | Class |
|---|---|---|---|
| ? | — | — | `one.me.android.concurrent.SingleCoreFeature$ToggleService` |
| ? | — | — | `ru.ok.tamtam.android.services.NotificationTamService` |
| false | `camera \| dataSync \| location \| mediaPlayback \| microphone` | — | `androidx.work.impl.foreground.SystemForegroundService` |
| false | `dataSync` | — | `one.me.background.wake.BackgroundListenService` |
| false | `mediaPlayback` | — | `one.me.android.media.service.OneMeMediaSessionService` |
| false | `mediaProjection` | — | `ru.ok.tamtam.android.calls.MediaProjectionService` |
| false | — | — | `com.google.mlkit.common.internal.MlKitComponentDiscoveryService` |
| **true** | — | `com.google.android.gms.auth.api.signin.permission.REVOCATION_NOTIFICATION` | `com.google.android.gms.auth.api.signin.RevocationBoundService` |
| false | — | — | `com.google.firebase.components.ComponentDiscoveryService` |
| false | — | — | `androidx.work.impl.background.systemalarm.SystemAlarmService` |
| false | — | `android.permission.BIND_JOB_SERVICE` | `ru.ok.android.onelog.UploadService` |
| false | — | — | `com.google.android.datatransport.runtime.backends.TransportBackendDiscovery` |

(Плюс плотный набор внутренних one.me-сервисов и SDK без exported, прокручены ниже.)

Из exported сервисов внешним приложениям доступен только GMS RevocationBoundService — это стандартная Google штука, не специфична для MAX.

`SystemForegroundService` объявлен с **общим** `foregroundServiceType="camera|dataSync|location|mediaPlayback|microphone"` — то есть это «универсальный» foreground-сервис WorkManager, через который любой WorkRequest может работать с одним из перечисленных типов разрешений. У `CallServiceImpl` (см. [[00-INVENTORY]]) отдельный `foregroundServiceType="camera|mediaPlayback|mediaProjection|microphone|phoneCall"` — то есть звонок по умолчанию имеет права на запись экрана (`mediaProjection`).

## 4. Receivers (16 шт., полный список)

Полностью exported только два — оба от стандартных AndroidX (`DiagnosticsReceiver`, `ProfileInstallReceiver`), защищённых пермом `android.permission.DUMP` (только shell/system могут слать).

`com.google.firebase.iid.FirebaseInstanceIdReceiver` — exported=true с пермом `com.google.android.c2dm.permission.SEND` (только GMS может слать) — стандартный FCM.

Без `exported` атрибута (Android выводит из intent-filter — становятся **exported** если фильтр объявлен, для API < 33; и **non-exported** для API >= 33):

- **`one.me.android.calls.CallNotifierBroadcastReceiver`** — без флага. Объявленные действия:
    - `android.nfc.cardemulation.action.HOST_APDU_SERVICE` — это action для `<service>`, у `<receiver>` он не сработает. Похоже на ошибку конфигурации или артефакт.
    - `android.intent.action.BOOT_COMPLETED`
    - `android.intent.action.QUICKBOOT_POWERON`
    - `com.htc.intent.action.QUICKBOOT_POWERON`

  То есть «звонковый нотификатор» — receiver, просыпающийся на старт устройства.

- `one.me.background.wake.BackgroundCheckReceiver` — `MY_PACKAGE_REPLACED`, `BOOT_COMPLETED`. См. [[08-background-wake]].

Остальные — внутренние WorkManager constraint-proxy, ничего необычного.

Сводка тревоги: на BOOT_COMPLETED просыпается **минимум два разных компонента** MAX: `BackgroundCheckReceiver` и `CallNotifierBroadcastReceiver`. Плюс `androidx.work.impl.background.systemalarm.RescheduleReceiver`. Плюс `BackgroundWakeBootReceiver`. То есть после ребута MAX обязательно поднимается несколько раз.

## 5. Providers (8 шт.)

| Exported | Enabled | Authority | Class |
|---|---|---|---|
| false | true | `ru.oneme.app.androidx-startup` | `androidx.startup.InitializationProvider` |
| false | true | `ru.oneme.app.tracer-startup` | `androidx.startup.InitializationProvider` |
| false | true | `ru.oneme.app.provider` | `androidx.core.content.FileProvider` |
| false | **false** | `ru.oneme.app.firebaseinitprovider` | `com.google.firebase.provider.FirebaseInitProvider` |
| false | true | `ru.oneme.app.notifications` | `one.me.android.notifications.NotificationsImagesProvider` |
| ? | **false** | `ru.oneme.app.dps-init-provider` | `ru.trace_flow.dps.internal.DpsInitProvider` |
| false | true | `ru.oneme.app.tracer-startup` | `ru.ok.tracer.startup.InitializationProvider` |
| false | true | `ru.oneme.app.ApplicationProvider` | `ru.ok.android.commons.app.ApplicationProvider` |

Странности:

1. `FirebaseInitProvider` отключен (`enabled=false`). Это значит Firebase **не используется обычным способом**. FCM (push) отдельно через `FirebaseInstanceIdReceiver`+`<service>` из firebase messaging — то есть Firebase используется ТОЛЬКО для push-уведомлений, без всего остального firebase SDK (analytics, crashlytics, performance — отрублены в пользу собственных через apptracer.ru).
2. `DpsInitProvider` отключен (`enabled=false`), но регистрация осталась. По имени — Data Processing Service / Data Producer Service. PmsKey `dps` сервер может включить → провайдер тогда…? Нужно проверить, как `enabled` меняется в рантайме. Динамически — никак. Значит `enabled=false` означает «не используется в этой сборке», но код в APK есть.
3. Дублирующиеся authorities `ru.oneme.app.tracer-startup` (и `androidx.startup.InitializationProvider`, и `ru.ok.tracer.startup.InitializationProvider` объявлены с одной authority) — это редкое, и обычно ошибка. По факту Android установит первое объявление.

## 6. Activities (11 шт.) — exported

| Exported | Class | Notable filters |
|---|---|---|
| true | `one.me.android.MainActivity` | `MAIN`, `SEND`, `SEND_MULTIPLE` |
| (часть) | `one.me.android.LinkInterceptorActivity` | http://max.ru, max://max.ru, autoVerify=true (см. [[13-deeplinks-idp]]) |

Все остальные activity — внутренние (settings, profile, chat, splash и т. д.), exported=false.

## 7. Hardcoded values в манифесте

- `com.google.android.geo.API_KEY = AIzaSyDJbuC3fODS_aR7jcOkoP6qWIsQen9XARI` (Google Maps — для встроенных карт).
- `app_scheme = max`, `app_host = max.ru`.
- `com.facebook.soloader.enabled = false` (отключение SoLoader, см. [[09-native-libs]]).
- `tracer_environment = "release"` или подобное (нужно отдельно вытащить).
- `tracer_mapping_uuid` — задаётся в Gradle при сборке, идёт в `osi.c.uuid` (см. [[17-apptracer-uplink]]).
- Иконка приложения — `@mipmap/ic_launcher_9may` («9 мая»).

## 8. Сводка

В сумме разрешения и компоненты позволяют MAX:

- иметь устойчивое фоновое присутствие (`BOOT_COMPLETED` × 4 receivers, `MY_PACKAGE_REPLACED` × 1, foreground-service `dataSync` без таймера);
- вести запись звука и видео в звонке + запись экрана (`mediaProjection` в `CallServiceImpl`);
- работать как телефонный аккаунт системы (`MANAGE_OWN_CALLS`) — отображать звонки наряду с системной звонилкой;
- читать и писать контакты, читать медиатеку (фото/видео), читать местоположение;
- эмулировать NFC-карту;
- показывать оверлеи поверх других приложений (`SYSTEM_ALERT_WINDOW`).

Все эти возможности — **штатные пермиссии Android**, по which `ru.oneme.app` запрашивает разрешение у пользователя через runtime prompts. Скептический взгляд: при «обязательной предустановке» (см. вступление в README про предустановку с 1 сентября 2025) часть permission-grant-ов может быть приходить уже выданными через bundled-permission-механизм OEM или через MDM-профиль.
