# 09 — Attack Surface: AndroidManifest.xml Exported Components

> Версии: 26.15.3 и 26.16.0 (идентичны по exported-компонентам)

---

## 1. Полный список EXPORTED компонентов

### Activities (exported=true)

| Компонент | Permission | Intent-Filters |
|-----------|-----------|----------------|
| `one.me.android.MainActivity` | **НЕТ** | `MAIN/LAUNCHER`, `SEND/*`, `SEND_MULTIPLE/*` |
| `one.me.android.deeplink.LinkInterceptorActivity` | **НЕТ** | `VIEW` http/https/max://max.ru/..* (autoVerify=true) |

### Services (exported=true)

| Компонент | Permission | Intent-Filters |
|-----------|-----------|----------------|
| `one.me.webapp.util.WebAppNfcService` | `BIND_NFC_SERVICE` | `HOST_APDU_SERVICE` |
| `com.google.android.gms.auth.api.signin.RevocationBoundService` | `REVOCATION_NOTIFICATION` | — |
| `androidx.work.impl.background.systemjob.SystemJobService` | `BIND_JOB_SERVICE` | — |

### Receivers (exported=true)

| Компонент | Permission | Intent-Filters |
|-----------|-----------|----------------|
| `ru.ok.tamtam.android.services.BootCompletedReceiver` | **НЕТ** | `BOOT_COMPLETED`, `QUICKBOOT_POWERON` |
| `one.me.background.wake.BackgroundWakeBootReceiver` | **НЕТ** | `MY_PACKAGE_REPLACED`, `BOOT_COMPLETED` |
| `com.google.firebase.iid.FirebaseInstanceIdReceiver` | `c2dm.permission.SEND` | `c2dm.intent.RECEIVE` |
| `androidx.work.impl.diagnostics.DiagnosticsReceiver` | `android.permission.DUMP` | `REQUEST_DIAGNOSTICS` |
| `androidx.profileinstaller.ProfileInstallReceiver` | `android.permission.DUMP` | `INSTALL_PROFILE`, `SKIP_FILE`, `SAVE_PROFILE`, `BENCHMARK_OPERATION` |

### Providers

| Authority | exported | grantUriPermissions | Компонент |
|-----------|----------|---------------------|-----------|
| `ru.oneme.app.provider` | **false** | **true** | `FileProvider` |
| `ru.oneme.app.notifications` | **false** | **true** | `NotificationsImagesProvider` |
| `ru.oneme.app.ApplicationProvider` | false | false | `ApplicationProvider` |
| `ru.oneme.app.androidx-startup` | false | false | `InitializationProvider` |
| `ru.oneme.app.tracer-startup` | false | false | `InitializationProvider` |
| `ru.oneme.app.firebaseinitprovider` | false | false | `FirebaseInitProvider` |
| `ru.oneme.app.dps-init-provider` | false (disabled) | false | `DpsInitProvider` |

---

## 2. Signature Permissions

### `ru.oneme.app.permission.settings.READ` (protectionLevel=signature)

- **Объявлена** в манифесте, но **НЕ ИСПОЛЬЗУЕТСЯ** ни одним компонентом как `android:permission`.
- В коде (jadx) — **нет ссылок** на эту строку.
- **Вывод**: Зарезервирована для будущего IPC между приложениями одного разработчика (VK/OK/MAX). Signature-level означает, что только приложения, подписанные тем же ключом, смогут её использовать. Это потенциальный канал для **cross-app data sharing** между MAX, VK, OK.ru без ведома пользователя.

### `ru.oneme.app.DYNAMIC_RECEIVER_NOT_EXPORTED_PERMISSION` (protectionLevel=signature)

- Стандартный AndroidX-паттерн для защиты динамически регистрируемых receivers.
- Используется в `ct4.java` (utility class).

---

## 3. ContentProviders с grantUriPermissions=true — ТОЧКИ УТЕЧКИ

### 3.1 FileProvider (`ru.oneme.app.provider`)

**provider_paths.xml:**
```xml
<external-path name="external_files" path="." />
<cache-path name="cache" path="." />
<cache-path name="copy_media" path="copy/media/" />
```

**🔴 КРИТИЧЕСКАЯ НАХОДКА**: `external-path path="."` означает **ВЕСЬ внешний storage** (`/sdcard/`) доступен через FileProvider. Любое приложение, получившее URI через `Intent.FLAG_GRANT_READ_URI_PERMISSION`, может прочитать **любой файл** на внешнем хранилище через этот провайдер.

Хотя FileProvider не exported, URI с grant-permission передаются через intents (SEND, SEND_MULTIPLE) из exported MainActivity. Атакующее приложение может:
1. Отправить `ACTION_SEND` в MainActivity
2. Получить обратно URI с grant-permission через result
3. Использовать URI для чтения произвольных файлов

### 3.2 NotificationsImagesProvider (`ru.oneme.app.notifications`)

- **exported=false**, но **grantUriPermissions=true**
- URI pattern: `content://ru.oneme.app.notifications/message_image/{url}/{loadFromNetwork}`
- **Проверка**: Есть `jel.k()` — проверяет что URI не указывает на internal storage (symlink resolution до 10 уровней)
- **Но**: Параметр `loadFromNetwork=true` заставляет провайдер **скачать произвольный URL** и вернуть его как файл!
- **SSRF-вектор**: Если другое приложение получит grant-permission на этот провайдер, оно может заставить MAX скачать произвольный URL (включая internal network endpoints) и вернуть содержимое.

---

## 4. Deep Link Activities — Валидация URI

### LinkInterceptorActivity

**Зарегистрированные схемы:**
- `http://max.ru/...*` (autoVerify=true)
- `https://max.ru/...*` (autoVerify=true)  
- `max://max.ru/*` (custom scheme, без autoVerify)

**Анализ валидации:**

1. **Первый проход** — `I(intent)`: проверяет action через `mv6.g(action)`. Обрабатывает только call-related actions (`action-open-call`, `action-accept-call`, etc.). Если action неизвестен — возвращает `fp1.a` (unknown), и `I()` возвращает false.

2. **Второй проход** — URI передаётся в `MyTracker.handleDeeplink(intent)`:
   - **🔴 MyTracker получает ПОЛНЫЙ URI** включая все query-параметры
   - MyTracker может **переписать URI** (возвращает новую строку)
   - Только `mt_*` параметры фильтруются из результата
   - **Переписанный URI** передаётся в `LinkInterceptorWidget` без дальнейшей валидации

3. **Нет проверки host/path**: URI парсится и передаётся в виджет как есть. Нет whitelist-проверки path-компонента.

**🔴 АТАКА**: Любое приложение может отправить intent:
```
Intent(ACTION_VIEW, Uri.parse("max://max.ru/:auth?externalCallback=1&redirect=https://evil.com"))
```
И MAX обработает его, потенциально перенаправив auth-flow на attacker-controlled URL.

---

## 5. Receivers с DUMP permission

### DiagnosticsReceiver (androidx.work)
- Permission: `android.permission.DUMP` (signature|privileged)
- Action: `androidx.work.diagnostics.REQUEST_DIAGNOSTICS`
- **Что делает**: Выводит диагностику WorkManager (pending jobs, constraints, etc.)
- **Риск**: Низкий — требует DUMP permission (только system/adb)

### ProfileInstallReceiver
- Permission: `android.permission.DUMP`
- Actions: `INSTALL_PROFILE`, `SKIP_FILE`, `SAVE_PROFILE`, `BENCHMARK_OPERATION`
- **Что делает**: Управляет baseline profiles для ART
- **Риск**: Низкий — стандартный AndroidX компонент

---

## 6. BIND_* сервисы

| Сервис | Permission | Назначение |
|--------|-----------|------------|
| `WebAppNfcService` | `BIND_NFC_SERVICE` | HCE — эмуляция NFC-карты для мини-приложений |
| `CallServiceImpl` | `BIND_TELECOM_CONNECTION_SERVICE` | Интеграция с Telecom framework |
| `SystemJobService` | `BIND_JOB_SERVICE` | WorkManager job scheduling |
| `FileUploadService` | `BIND_JOB_SERVICE` | Загрузка файлов (video calls SDK) |
| `externcalls.UploadService` | `BIND_JOB_SERVICE` | OneLog analytics upload |
| `onelog.UploadService` | `BIND_JOB_SERVICE` | OneLog analytics upload |
| `JobInfoSchedulerService` | `BIND_JOB_SERVICE` | Firebase data transport |

---

## 7. WebAppNfcService — HCE без caller validation

**AID**: `F0010203040303` (custom, non-standard)

**Критический анализ processCommandApdu():**
```java
public byte[] processCommandApdu(byte[] bArr, Bundle bundle) {
    // Только проверяет длину >= 4 и INS byte == 0xA4 (SELECT)
    // НЕ проверяет кто вызывает (какой NFC reader)
    byte[] bArr2 = (byte[]) ((jxb) this.b.getValue()).d.get();
    // Возвращает данные из внутреннего буфера
    return bArr2;
}
```

**🔴 НАХОДКА**: Данные для NFC-ответа берутся из `jxb.d` — это `AtomicReference`, заполняемый мини-приложением через JS-bridge. Любой NFC-reader (включая злоумышленника с NFC-считывателем) может получить эти данные без аутентификации. Если мини-приложение загрузило платёжные данные — они утекут при proximity-атаке.

---

## 8. Boot-time поведение (без permission)

### BootCompletedReceiver (exported=true, NO permission)
На `BOOT_COMPLETED`:
1. Получает DI-контейнер через `nuf.G(context)`
2. Вызывает accessor 80 → `lkc` (executor)
3. Запускает `p3(20, ...)` → case 20 в run():
   - `uei.o.getValue().a.A(true)` — включает фоновый режим
   - `vei.c(false)` — отключает что-то
   - `zgi.a()` — инициализирует что-то
   - **`toc.d()`** (accessor 542) — `notifyAllChats` + `h()` — **пробуждает все чаты и синхронизирует**

**🔴 НАХОДКА**: На каждый boot MAX автоматически:
- Включает фоновый режим
- Синхронизирует ВСЕ чаты с сервером
- Перепланирует background wake alarm

Это происходит **без взаимодействия пользователя** и **без проверки caller** (receiver exported без permission).

### BackgroundWakeBootReceiver (exported=true, NO permission)
На `BOOT_COMPLETED` и `MY_PACKAGE_REPLACED`:
- Проверяет `pn0.d()` — включён ли background wake
- Если да — `pn0.e()` — перепланирует alarm для периодического пробуждения

---

## 9. Подозрительные intent-filters

| Action | Компонент | Риск |
|--------|-----------|------|
| `BOOT_COMPLETED` | BootCompletedReceiver, BackgroundWakeBootReceiver | Автозапуск без user consent |
| `QUICKBOOT_POWERON` | BootCompletedReceiver | Vendor-specific boot (HTC) |
| `MY_PACKAGE_REPLACED` | BackgroundWakeBootReceiver | Self-update trigger |
| `HOST_APDU_SERVICE` | WebAppNfcService | NFC card emulation |
| `ru.ok.android.onelog.action.UPLOAD` | 2× UploadService | Telemetry upload trigger |
| `com.google.firebase.MESSAGING_EVENT` | FcmMessagingService (priority normal) + FirebaseMessagingService (priority -500) | Push command processing |

**Отсутствуют** (хорошо): `SCREEN_OFF`, `DEVICE_OWNER_CHANGED`, `Telephony.*` — нет прямого перехвата SMS/звонков через manifest receivers.

---

## 10. Authority провайдеров и данные

| Authority | Данные |
|-----------|--------|
| `ru.oneme.app.provider` | **Весь /sdcard/ + весь cache/** через FileProvider |
| `ru.oneme.app.notifications` | Кэшированные изображения уведомлений + **SSRF через loadFromNetwork** |
| `ru.oneme.app.ApplicationProvider` | Application context (internal, не отдаёт данные) |
| `ru.oneme.app.tracer-startup` | Инициализация Tracer SDK (HeapDump, CrashReport, NativeBridge, DiskUsage) |
| `ru.oneme.app.dps-init-provider` | DPS (disabled) — trace_flow analytics |

---

## 🔴 TOP-5 ATTACK VECTORS (НОВЫЕ)

### 1. FileProvider exposes entire /sdcard/ (path=".")

**provider_paths.xml** содержит `<external-path name="external_files" path="." />` — это означает что FileProvider может сгенерировать URI для **любого файла** на внешнем хранилище. В комбинации с exported MainActivity (принимает `ACTION_SEND`), атакующее приложение может получить grant-URI и прочитать произвольные файлы.

**Severity**: HIGH — data exfiltration через IPC

### 2. NotificationsImagesProvider SSRF (loadFromNetwork=true)

Провайдер принимает произвольный URL в path-сегменте и скачивает его при `loadFromNetwork=true`. Хотя провайдер не exported, URI с grant-permission могут утечь через notification channels или pending intents. Это позволяет использовать устройство жертвы как SSRF-прокси для сканирования внутренней сети.

**Severity**: MEDIUM-HIGH — SSRF через content provider

### 3. LinkInterceptorActivity: No URI path validation + MyTracker URI rewrite

Deep link handler:
- Не валидирует path-компонент URI
- Передаёт URI в MyTracker (третья сторона) который может его **переписать**
- Переписанный URI обрабатывается без повторной валидации
- Custom scheme `max://` не имеет autoVerify — любое приложение может отправить intent

**Severity**: HIGH — auth redirect hijack, phishing через deep links

### 4. NFC HCE без caller authentication

WebAppNfcService отдаёт данные из JS-bridge буфера любому NFC-reader без проверки. Proximity-атака позволяет считать платёжные/идентификационные данные мини-приложения.

**Severity**: MEDIUM — proximity data theft

### 5. Boot receivers без permission → silent full sync

Два exported receiver без permission-защиты автоматически:
- Синхронизируют ВСЕ чаты с сервером
- Включают фоновый режим
- Планируют periodic wake

Любое приложение с `RECEIVE_BOOT_COMPLETED` может отправить broadcast `BOOT_COMPLETED` (до Android 14) и триггернуть полную синхронизацию. Это не прямая уязвимость, но создаёт **гарантированный канал** для серверной стороны: даже если пользователь убил приложение, после reboot MAX автоматически восстановит связь с сервером и синхронизирует всё.

**Severity**: MEDIUM — persistence + guaranteed server reconnection

---

## Дополнительные наблюдения

### Google Maps API Key в манифесте (hardcoded)
```
AIzaSyDJbuC3fODS_aR7jcOkoP6qWIsQen9XARI
```
Не уязвимость сама по себе, но позволяет abuse quota.

### Foreground Service Types — максимальный набор
`SystemForegroundService` имеет типы: `camera|dataSync|location|mediaPlayback|microphone` — это позволяет WorkManager задачам использовать **камеру, микрофон, геолокацию** в фоне.

### HeapDumpInitializer в tracer-startup
Tracer SDK инициализируется через ContentProvider (до Application.onCreate), включая `HeapDumpInitializer` — heap dumps начинают собираться **до** того как пользователь что-либо сделал.

### `${REQUEST_INSTALL_PACKAGES}` — placeholder в манифесте
Строка `${REQUEST_INSTALL_PACKAGES}` не раскрыта — это Gradle placeholder. Означает что permission `REQUEST_INSTALL_PACKAGES` **условно включается** при сборке. Позволяет устанавливать APK без Play Store (для force-update через download.max.ru).

---

## Diff 26.15.3 → 26.16.0

Манифесты **идентичны** по exported-компонентам. Единственные отличия:
- Иконка: `ic_launcher_9may` → `ic_launcher` (убрали 9 мая тему)
- В 26.16.0 убраны `com.android.stamp.*` meta-data (Play Store stamp)
- В 26.16.0 `web_scheme` вынесен в `@string/web_scheme` (было hardcoded `https`)

**Вывод**: Attack surface не изменился между версиями.
