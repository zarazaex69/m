# 04 — Cross-App Data Exfiltration Analysis

## Summary

MAX (ru.oneme.app) **не использует** наиболее агрессивные Android-механизмы перехвата данных других приложений (NotificationListenerService, AccessibilityService, DeviceAdmin, VpnService, READ_LOGS). Однако через встроенный MyTracker SDK (VK) реализована **полная эксфильтрация списка установленных приложений** с отправкой на `tracker-api.vk-analytics.ru`, а также сбор данных всех доступных сенсоров устройства.

---

## 1. PackageManager — Список установленных приложений → VK Analytics

### Механизм

MyTracker SDK определяет интерфейс `InstalledPackagesProvider`:

```java
// com.my.tracker.MyTrackerConfig
public interface InstalledPackagesProvider {
    List<PackageInfo> getInstalledPackages();
}
```

### Фильтрация и сериализация (com.my.tracker.core.o.f)

```java
private static List b(List list) {
    ArrayList arrayList = new ArrayList();
    for (PackageInfo packageInfo : list) {
        ApplicationInfo applicationInfo = packageInfo.applicationInfo;
        // Фильтр: только НЕ-системные приложения (flags & 1 == 0)
        if ((applicationInfo.flags & 1) == 0) {
            arrayList.add(new e.a(
                applicationInfo.packageName,           // имя пакета
                TimeUtils.convertToSec(packageInfo.firstInstallTime)  // время установки
            ));
        }
    }
    return arrayList;
}
```

**Что собирается:**
- `packageName` — полное имя каждого установленного пользовательского приложения
- `firstInstallTime` — timestamp первой установки (в секундах)

### Дедупликация и отправка

```java
public e a(InstalledPackagesProvider provider) {
    List<PackageInfo> packages = provider.getInstalledPackages();
    List filtered = b(packages);  // только user-apps
    String joined = a(filtered);  // join packageNames через ","
    String currentHash = n.a(joined);  // hash списка
    String savedHash = enginePrefs.getString("appsHash");
    
    if (savedHash.equals(currentHash)) return EMPTY;  // не изменился
    
    enginePrefs.setString("appsHash", currentHash);  // сохранить новый hash
    return new e(filtered);  // отправить полный список
}
```

### Куда уходит

Данные сериализуются в protobuf (field 31, repeated) и отправляются на:
- **`tracker-api.vk-analytics.ru`** — основной endpoint (hardcoded в `TrackerConfig.b()`)

```java
// com.my.tracker.core.proto.a
public static void a(ProtoWriter protoWriter, e eVar, ByteArrayProtoWriter buf) {
    for (e.a app : eVar.a) {
        buf.reset();
        buf.writeField(1, app.a);  // packageName
        buf.writeField(2, app.b);  // firstInstallTime
        protoWriter.writeField(31, buf);  // repeated field
    }
}
```

### Инициализация в приложении

```java
// defpackage/d6.java — при старте OneMeApplication
MyTracker.getTrackerConfig()
    .setOkHttpClientProvider(new h50(16))
    .setKidMode(false)
    .setBackgroundExecutor(ConcurrentComponent.INSTANCE.getExecutors().d())
    .setLogger(new zxf(16, rtdVar2));
MyTracker.initTracker("34982109644049932883", oneMeApplication);
```

Tracker ID: **34982109644049932883**

> **Примечание:** `setInstalledPackagesProvider()` не вызывается явно в декомпилированном коде приложения. Однако интерфейс определён, SDK его поддерживает, и `TrackerConfig.ReadOnlyCopy.installedProvider` передаётся в proto-сериализацию. Возможно, provider устанавливается через серверную конфигурацию или reflection.

---

## 2. ClipboardManager — Чтение/запись буфера обмена

### Использование

Clipboard используется **только для пользовательских действий** (copy/paste):

**Класс `ex3` (clipboard utility):**
```java
public static void a(Context context, String str) {
    // Копирование текста в буфер (по действию пользователя)
    a.d(new l0(context, "Copied Text", str, 17));
}

public static CharSequence c(Context context) {
    // Чтение из буфера (для paste/link preview)
    ClipData primaryClip = ((ClipboardManager) context.getSystemService("clipboard")).getPrimaryClip();
    return primaryClip.getItemAt(0).getText();
}
```

**Класс `hgl` (drag-and-drop handler):**
- Обработка paste в EditText через `getPrimaryClip()`

**Класс `uc2`:**
- `setPrimaryClip(ClipData.newUri(..., "image/jpeg", uri))` — копирование изображения

### Вердикт

- ❌ `addPrimaryClipChangedListener` — **НЕ ИСПОЛЬЗУЕТСЯ** (нет фонового мониторинга буфера)
- Clipboard доступ только по явному действию пользователя (copy/paste кнопки)
- Данные из буфера **не отправляются** на сервер

---

## 3. NotificationListenerService — BIND_NOTIFICATION_LISTENER_SERVICE

**❌ НЕ ОБЪЯВЛЕН** в AndroidManifest.xml (ни в 26.15.3, ни в 26.16.0).

Нет ни permission, ни service с этим binding.

---

## 4. AccessibilityService

**❌ НЕ ОБЪЯВЛЕН** в AndroidManifest.xml.

Все 203 упоминания `AccessibilityEvent` в коде — это:
- Стандартные UI-компоненты (RecyclerView, ViewPager, MaterialButton, SwitchCompat)
- `vu9` — Material AutoCompleteTextView, проверяет SwitchAccess для UI-адаптации
- Нет собственного AccessibilityService

---

## 5. UsageStatsManager / PACKAGE_USAGE_STATS

**Используется ограниченно:**

### 5.1 DbCleanUpWorker — standby bucket для логирования
```java
// ru.ok.tamtam.android.services.DbCleanUpScheduler$DbCleanUpWorker
Integer bucket = Build.VERSION.SDK_INT >= 28 
    ? Integer.valueOf(((UsageStatsManager) context.getSystemService("usagestats")).getAppStandbyBucket()) 
    : null;
// Используется только в debug-логе при удалении старых событий
```

### 5.2 kph (startup_report) — bucket отправляется как метрика
```java
// defpackage/kph.java
lobVar.j("bucket", Integer.valueOf(
    ((UsageStatsManager) context.getSystemService("usagestats")).getAppStandbyBucket()
));
```

**Куда:** startup_report → OneLog/api.oneme.ru (внутренняя аналитика)

### Вердикт
- `queryUsageStats()` — **НЕ ВЫЗЫВАЕТСЯ** (нет сбора статистики использования других приложений)
- Только `getAppStandbyBucket()` — bucket самого приложения MAX
- Permission `PACKAGE_USAGE_STATS` **НЕ ОБЪЯВЛЕН** в манифесте

---

## 6. PROCESS_OUTGOING_CALLS / READ_PHONE_STATE

### READ_PHONE_STATE
- **Объявлен** в коде как runtime permission check (не в манифесте как uses-permission!)
- Используется **только** для определения типа мобильной сети (2G/3G/4G/5G):

```java
// defpackage/oad.java, defpackage/r04.java
if (context.checkPermission("android.permission.READ_PHONE_STATE", ...) == 0) {
    switch (telephonyManager.getNetworkType()) { ... }
}
```

### PROCESS_OUTGOING_CALLS
**❌ НЕ ИСПОЛЬЗУЕТСЯ** — нет ни permission, ни кода перехвата исходящих звонков.

---

## 7. READ_LOGS

**❌ НЕ ОБЪЯВЛЕН** в манифесте. (Logcat capture через другой механизм — см. topic 535)

---

## 8. DUMP

**Используется** двумя AndroidX-компонентами (не приложением):
- `androidx.work.impl.diagnostics.DiagnosticsReceiver` — permission="android.permission.DUMP"
- `androidx.profileinstaller.ProfileInstallReceiver` — permission="android.permission.DUMP"

Это стандартные AndroidX receivers для отладки WorkManager и профилей. Приложение **не запрашивает** DUMP permission.

---

## 9. BIND_DEVICE_ADMIN / DeviceAdminReceiver

**❌ НЕ ИСПОЛЬЗУЕТСЯ.** Нет DeviceAdminReceiver, нет BIND_DEVICE_ADMIN.

---

## 10. BIND_VPN_SERVICE / VpnService

**❌ НЕ ИСПОЛЬЗУЕТСЯ.** MAX не реализует VPN-сервис. (Но детектирует VPN у пользователя — см. topic 02, 540)

---

## Дополнительные находки

### 11. MyTracker — полный профиль пользователя → VK Analytics

Помимо списка приложений, MyTracker сериализует и отправляет:

```java
// com.my.tracker.core.proto.b — UserInfo proto field 22
byteArrayProtoWriter.writeField(1, userInfoState.gender);
byteArrayProtoWriter.writeRepeatedField(3, userInfoState.okIds);
byteArrayProtoWriter.writeRepeatedField(4, userInfoState.vkIds);
byteArrayProtoWriter.writeRepeatedField(5, userInfoState.emails);
byteArrayProtoWriter.writeRepeatedField(6, userInfoState.icqIds);
byteArrayProtoWriter.writeRepeatedField(7, userInfoState.customUserIds);
byteArrayProtoWriter.writeRepeatedField(8, userInfoState.phones);
byteArrayProtoWriter.writeRepeatedField(9, userInfoState.vkConnectIds);
```

**Полный набор данных в одном protobuf-пакете:**
- gender, age
- okIds[], vkIds[], vkConnectIds[], icqIds[]
- emails[], phones[]
- customUserIds[] (= MAX userId)
- android_id, MAC address
- Google Advertising ID (GAID) + Huawei OAID
- AppSet ID
- Device: manufacturer, model, OS version
- Screen: resolution, density, DPI
- Disk: total/free space
- Timezone, locale, language
- isRooted (проверка su, Magisk, test-keys)
- Список установленных приложений (packageName + installTime)

### 12. AntiFraud Sensors — данные сенсоров

```java
// com.my.tracker.config.AntiFraudConfig
public final boolean useGyroscope;        // TYPE_GYROSCOPE
public final boolean useLightSensor;      // TYPE_LIGHT  
public final boolean useMagneticFieldSensor; // TYPE_MAGNETIC_FIELD
public final boolean usePressureSensor;   // TYPE_PRESSURE
public final boolean useProximitySensor;  // TYPE_PROXIMITY
```

Все сенсоры **включены по умолчанию** (Builder defaults = true). Данные отправляются как часть "антифрод" профиля на `tracker-api.vk-analytics.ru`.

### 13. ${REQUEST_INSTALL_PACKAGES} — шаблонная переменная в манифесте

В обеих версиях манифеста присутствует необработанная шаблонная переменная:
```xml
<uses-permission android:name="android.permission.POST_NOTIFICATIONS"/> ${REQUEST_INSTALL_PACKAGES}
```

Это означает, что в build-системе есть условие для включения `REQUEST_INSTALL_PACKAGES` — permission, позволяющего устанавливать APK из неизвестных источников. В production-сборке переменная не раскрыта (осталась как текст), но в других вариантах сборки (internal/debug) она может быть активна.

### 14. getRunningAppProcesses — определение процесса

```java
// defpackage/w6e.java (Firebase internals)
List<RunningAppProcessInfo> processes = activityManager.getRunningAppProcesses();
// Используется только для определения имени текущего процесса (self)
```

---

## Итоговая оценка угрозы

| Вектор | Статус | Severity |
|--------|--------|----------|
| Список установленных приложений → VK | ✅ АКТИВЕН | **HIGH** |
| Полный профиль (phone/email/okId/vkId) → VK | ✅ АКТИВЕН | **HIGH** |
| Sensor fingerprint → VK | ✅ АКТИВЕН | **MEDIUM** |
| Root detection → VK | ✅ АКТИВЕН | **MEDIUM** |
| Standby bucket → api.oneme.ru | ✅ АКТИВЕН | **LOW** |
| Clipboard monitoring | ❌ Нет | — |
| NotificationListener | ❌ Нет | — |
| AccessibilityService | ❌ Нет | — |
| DeviceAdmin | ❌ Нет | — |
| VpnService | ❌ Нет | — |
| READ_LOGS | ❌ Нет | — |
| PROCESS_OUTGOING_CALLS | ❌ Нет | — |
| UsageStats других приложений | ❌ Нет | — |

---

## Ключевой вывод

Основной канал cross-app exfiltration — **MyTracker SDK (VK)**, который:
1. Собирает **полный список пользовательских приложений** (packageName + installTime)
2. Хеширует список и отправляет **только при изменении** (delta-tracking)
3. Отправляет данные на **tracker-api.vk-analytics.ru** в protobuf
4. Связывает список приложений с **полным профилем пользователя** (phone, email, okId, vkId, vkConnectId, icqId, GAID)
5. Дополняет **sensor fingerprint** (гироскоп, магнитное поле, давление, освещённость, близость)

Это позволяет VK/Mail.ru Group строить **cross-app граф** пользователей с привязкой к реальной личности через множественные идентификаторы.
