# 08 — ContentProvider: полный аудит (26.16.0)

> Wave 3 deep-dive. Версия: 26.16.0 (versionCode 6698).
> Предыдущий анализ: `wave1/09-attack-surface-manifest.md` (таблица + FileProvider/SSRF).
> Цель: найти НЕ ЗАДОКУМЕНТИРОВАННОЕ ранее.

---

## 1. Полный список провайдеров (8 штук, идентичен 26.15.3)

| # | Authority | Class | exported | enabled | grantUri | permissions | path-permissions |
|---|-----------|-------|----------|---------|----------|-------------|------------------|
| 1 | `ru.oneme.app.androidx-startup` | `androidx.startup.InitializationProvider` | false | true | false | — | — |
| 2 | `ru.oneme.app.tracer-startup` | `ru.ok.tracer.startup.InitializationProvider` | false | true | false | — | — |
| 3 | `ru.oneme.app.provider` | `androidx.core.content.FileProvider` | false | true | **true** | — | — |
| 4 | `ru.oneme.app.firebaseinitprovider` | `com.google.firebase.provider.FirebaseInitProvider` | false | **false** | false | — | — |
| 5 | `ru.oneme.app.notifications` | `one.me.android.notifications.NotificationsImagesProvider` | false | true | **true** | — | — |
| 6 | `ru.oneme.app.dps-init-provider` | `ru.trace_flow.dps.internal.DpsInitProvider` | false | **false** | false | — | — |
| 7 | `ru.oneme.app.tracer-startup` (дубль!) | `ru.ok.tracer.startup.InitializationProvider` | false | true | false | — | — |
| 8 | `ru.oneme.app.ApplicationProvider` | `ru.ok.android.commons.app.ApplicationProvider` | false | true | false | — | — |

**Примечание**: Authority `ru.oneme.app.tracer-startup` объявлена ДВАЖДЫ — строка 155 (через `<meta-data>` в `androidx.startup.InitializationProvider`) и строка 253 (отдельный `ru.ok.tracer.startup.InitializationProvider`). Android разрешает это только если один из них disabled или они в разных процессах. Здесь оба enabled — потенциальный конфликт при установке (на практике Android берёт первый).

---

## 2. grantUriPermissions=true БЕЗ path-permission

**Два провайдера** имеют `grantUriPermissions="true"` без каких-либо `<path-permission>`:

1. **FileProvider** (`ru.oneme.app.provider`) — `grantUriPermissions=true`, NO path-permission
2. **NotificationsImagesProvider** (`ru.oneme.app.notifications`) — `grantUriPermissions=true`, NO path-permission

Это означает: любой URI, сгенерированный через `FileProvider.getUriForFile()` или `NotificationsImagesProvider`, при передаче через Intent с `FLAG_GRANT_READ_URI_PERMISSION` даёт получателю доступ **ко всему дереву** провайдера, а не к конкретному пути.

---

## 3. FileProvider — подтверждение `external-path="."` (ИЗВЕСТНО)

```xml
<!-- res/xml/provider_paths.xml -->
<paths>
    <external-path name="external_files" path="." />
    <cache-path name="cache" path="." />
    <cache-path name="copy_media" path="copy/media/" />
</paths>
```

**Scope**: весь `/sdcard/` + весь app cache. Без изменений в 26.16.0.

---

## 4. NotificationsImagesProvider — SSRF (ИЗВЕСТНО, уточнение)

### Механизм openFile():

1. URI pattern: `content://ru.oneme.app.notifications/message_image/{encoded_url}/{loadFromNetwork_bool}`
2. Декодирует `{encoded_url}` → парсит как `android.net.Uri`
3. Проверка `vej.i(context, uri)` — **internal URI detection**:
   - Resolves symlinks до 10 уровней через `Os.readlink()`
   - Canonicalizes path через `File.getCanonicalPath()`
   - Проверяет содержит ли путь `/data/data/{packageName}` или `context.getFilesDir().getParent()`
   - Если internal → `SecurityException("Internal uri detected")`
4. Если файл не в кеше И `loadFromNetwork=true` → **скачивает URL через image loader** (coroutine `w04`)
5. Возвращает `ParcelFileDescriptor.open(file, MODE_READ_ONLY)`

### Bypass-вектор (уточнение к wave1):

Проверка `vej.i()` защищает только от чтения **локальных** internal-файлов. Она **НЕ проверяет**:
- Является ли URL внешним HTTP/HTTPS (нет whitelist доменов)
- Является ли URL `file:///proc/...` или `file:///sys/...`
- Является ли URL `content://` другого провайдера

Однако: провайдер `exported=false`, поэтому SSRF возможен только если атакующее приложение получит `grantUriPermission` через Intent от MAX.

---

## 5. Инициализационные провайдеры — что запускают

### 5.1 `androidx.startup.InitializationProvider` (authority: `ru.oneme.app.androidx-startup`)

**onCreate()** читает `<meta-data>` из собственного ProviderInfo и инициализирует:
- `androidx.lifecycle.ProcessLifecycleInitializer` — стандартный lifecycle observer
- `androidx.profileinstaller.ProfileInstallerInitializer` — baseline profiles для ART

**Вердикт**: Стандартный AndroidX, безопасен.

### 5.2 `ru.ok.tracer.startup.InitializationProvider` (authority: `ru.oneme.app.tracer-startup`)

**onCreate()** вызывает `p2h.a(context)`:
- Проверяет ресурс `tracer_is_disabled` (bool)
- Если не disabled → инициализирует OK Tracer SDK

Из `<meta-data>` в манифесте видно что инициализируются:
- `NativeBridgeInitializer` — нативный crash handler
- `CrashReportInitializer` — crash reporting на apptracer.ru
- `PerformanceMetricsInitializer` — метрики производительности
- `LoggerInitializer` — логирование
- `TracerInitializer` — основной tracer
- `DiskUsageInitializer` — мониторинг диска
- **`HeapDumpInitializer`** — 🔴 инициализация heap dump collection

**Вердикт**: HeapDumpInitializer подтверждает что heap dumps собираются с момента запуска приложения (задокументировано в topic 17).

### 5.3 `FirebaseInitProvider` (authority: `ru.oneme.app.firebaseinitprovider`)

- **enabled="false"** — НЕ запускается автоматически
- Стандартный Firebase init. Disabled потому что MAX использует собственный push через `one.me.sdk.vendor.push.FcmMessagingService`.

### 5.4 `ApplicationProvider` (authority: `ru.oneme.app.ApplicationProvider`)

**onCreate()**: Сохраняет `Application` instance в static field `ApplicationProvider.a`.
- Все методы (query/insert/update/delete) бросают `UnsupportedOperationException`
- Чисто utility-провайдер для доступа к Application context из библиотечного кода (`commons-app_release`)
- **Не выдаёт данных наружу**

---

## 6. DpsInitProvider — trace-flow.ru (ИЗВЕСТНО, уточнение)

**Authority**: `ru.oneme.app.dps-init-provider`
**enabled**: `false` в манифесте

### onCreate() — что делает при включении:

1. Получает `Application` context
2. Читает ресурс по обфусцированному имени (`Ljde.a("3b5c71e286012f6483042854bd183252962e39558313305e86")`) — это bool-ресурс, проверяет включён ли DPS
3. Если включён — читает из `ApplicationInfo.metaData`:
   - Ключ `Ljde.a("ee30a31062d61e9a62c2538b4fc55c81678d549e638d71be59fc7bab49")` → API key
   - Ключ `Ljde.a("150dd2a5d7a72361d7b36e70fab4617ad2fc6965d6fc5846e080525ce1")` → endpoint URL
   - Ключ `Ljde.a("bfd0a6ee9cd3fecb9cc7b3dab1c0bcd09988b4cf9d8893f3a7e39eebb1f095edbdef9ff1")` → дополнительный параметр
4. Строит `c95$a` (DPS SDK builder) с Application, API key, endpoint
5. Вызывает `.e()` — инициализация SDK

**Вердикт**: Провайдер disabled, но код полностью рабочий. Включение через серверный PmsKey или OTA-обновление манифеста (через split APK) возможно. Нет других trace-flow провайдеров кроме этого.

---

## 7. IPC-провайдеры обрабатывающие file:// или content://

### vej class (utility, НЕ провайдер)

Класс `vej` — это **URI resolver** используемый по всему приложению для обработки входящих `content://` и `file://` URI:

- `vej.h(context, uri, ...)` — resolves URI to file path. Обрабатывает:
  - `file://` scheme → прямой путь
  - `content://` scheme → query `_data` column или `openFileDescriptor`
  - DocumentsContract URIs (external storage, downloads, media)
  - Google Photos content provider URIs
  - **Собственный FileProvider** (`external_files` path → `/sdcard/`)

- `vej.b(context, c56, url, filename)` — **копирует файл из произвольного URI** через `ContentResolver.openInputStream(Uri.parse(uriString))`. Нет валидации scheme — принимает и `file://`, и `content://`, и `http://`.

- `vej.c(uri)` — конвертирует FileProvider URI обратно в путь на external storage. Проверяет первый path segment == `"external_files"` → строит путь от `Environment.getExternalStorageDirectory()`.

- `vej.i(context, uri)` / `vej.j(uri, context, callback)` — internal URI detection (symlink resolution + canonical path + `/data/data/` check).

### 🟡 НОВАЯ НАХОДКА: vej.b() — openInputStream без scheme validation

`vej.b()` вызывает `ContentResolver.openInputStream(Uri.parse(uriString))` где `uriString` приходит из параметра. Если вызывающий код передаёт user-controlled URI (например из deeplink или WebApp JS-bridge), это позволяет:
- Чтение произвольных `content://` URI других приложений (если есть grant)
- Чтение `file://` URI (deprecated но работает на старых API)

Однако: `vej.b()` используется для **копирования файлов при отправке** (share intent), а не для внешнего IPC. Caller — внутренний код обработки вложений.

---

## 8. SQLite-провайдеры выдающие данные без permission

**НЕТ**. Ни один из 7 ContentProvider'ов не использует SQLiteDatabase в query(). Все кастомные провайдеры (NotificationsImagesProvider, ApplicationProvider, DpsInitProvider) возвращают `null` из query() или бросают `UnsupportedOperationException`.

Базы данных (Room) используются внутренне, но не экспонируются через ContentProvider.

---

## 9. InstallReferrerProvider / AdvertisingIdProvider

**НЕТ отдельных провайдеров** для install referrer или advertising ID в манифесте.

- `InstallReferrerClient` — стандартная Google Play библиотека, работает через IPC к Play Store (не через собственный provider)
- `AdvertisingIdClient` — стандартная GMS библиотека, работает через IPC к Google Play Services
- `MyTracker` использует оба через стандартные API, не регистрирует собственных провайдеров

---

## 10. grantUriPermission() в коде — кому выдаётся

Найдено 3 места вызова `Context.grantUriPermission()`:

1. **`dg6` class** (share/send handler) — грантит READ permission на FileProvider URI всем приложениям из `queryIntentActivities()` для share intent. Стандартный паттерн.

2. **`xff` class** (аналогичный share handler) — то же самое, другой code path.

3. **`FileProvider` class** (стандартный AndroidX) — внутренняя логика.

**Вердикт**: grantUriPermission используется только для стандартного share flow. Нет случаев программного гранта на NotificationsImagesProvider.

---

## 11. Дубль authority `ru.oneme.app.tracer-startup`

В манифесте 26.16.0 authority `ru.oneme.app.tracer-startup` объявлена дважды:
- Строка 155: `<provider android:authorities="ru.oneme.app.tracer-startup" android:exported="false" android:name="androidx.startup.InitializationProvider"/>` — пустой тег без meta-data
- Строка 253: `<provider android:authorities="ru.oneme.app.tracer-startup" android:exported="false" android:name="ru.ok.tracer.startup.InitializationProvider"/>`

Это **разные классы** с одинаковой authority. На Android это вызывает `INSTALL_FAILED_CONFLICTING_PROVIDER` если оба enabled. Работает потому что первый (строка 155) — это **merged placeholder** от Gradle (пустой, без meta-data), а реальная инициализация идёт через второй. При merge манифестов первый перезаписывается вторым.

---

## 12. Итоговая оценка — что НОВОЕ найдено

### Подтверждено (уже задокументировано):
- FileProvider `external-path="."` → весь /sdcard/ (wave1/09)
- NotificationsImagesProvider SSRF через loadFromNetwork (wave1/09)
- DpsInitProvider disabled но рабочий (topic 542)
- HeapDumpInitializer в tracer-startup (topic 17)

### Уточнения к известному:
- **vej.i() bypass**: проверка НЕ фильтрует `file:///proc/`, `file:///sys/`, `content://` других провайдеров — только `/data/data/{pkg}`. Но провайдер не exported, поэтому эксплуатация требует grant через Intent.
- **Дубль authority** `ru.oneme.app.tracer-startup` — аномалия сборки, не уязвимость.

### НОВОЕ (не задокументировано ранее):
- **НЕТ принципиально новых находок**. Все 8 провайдеров уже покрыты в wave1/09. Код провайдеров минимален — 5 из 7 это init-only (query/insert/delete бросают исключения или возвращают null). Единственные два с реальной логикой (FileProvider, NotificationsImagesProvider) уже полностью задокументированы.

---

## Вывод

ContentProvider attack surface в MAX **минимален по количеству** (8 провайдеров, все `exported=false`) но **максимален по scope** для двух из них:
- FileProvider покрывает весь external storage
- NotificationsImagesProvider позволяет SSRF

Оба требуют получения `grantUriPermission` через Intent, что ограничивает атаку приложениями, которым MAX сам передаёт URI (share targets). Новых, ранее не задокументированных уязвимостей в провайдерах **не обнаружено**.
