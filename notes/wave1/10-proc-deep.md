---
tags: [surveillance, proc-filesystem, sysfs, telemetry, apptracer, mytracker, onelog, cpu-fingerprint, root-detection, process-enumeration]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/uoa.java
  - work/jadx_base/sources/defpackage/gy4.java
  - work/jadx_base/sources/defpackage/k6e.java
  - work/jadx_base/sources/defpackage/z5l.java
  - work/jadx_base/sources/defpackage/lel.java
  - work/jadx_base/sources/defpackage/ui7.java
  - work/jadx_base/sources/defpackage/ij9.java
  - work/jadx_base/sources/defpackage/ilb.java
  - work/jadx_base/sources/defpackage/mpf.java
  - work/jadx_base/sources/defpackage/nei.java
  - work/jadx_base/sources/defpackage/km5.java
  - work/jadx_base/sources/defpackage/w6e.java
  - work/jadx_base/sources/com/my/tracker/core/o/m.java
  - work/jadx_base/sources/com/my/tracker/core/o/f.java
  - work/jadx_base/sources/com/my/tracker/core/TrackerConfig.java
  - work/jadx_base/sources/com/facebook/soloader/SysUtil$LollipopSysdeps.java
  - findings/native/libEnhancementLibShared.strings.txt
  - findings/native/libtracernative.strings.txt
  - findings/native/libjingle_peerconnection_so.strings.txt
related:
  - "[[318-proc-filesystem-reads]]"
  - "[[454-proc-files]]"
  - "[[30-root-detection-telemetry]]"
  - "[[338-root-detection-impl]]"
  - "[[04-telemetry-endpoints]]"
  - "[[17-apptracer-uplink]]"
  - "[[35-mytracker-antifraud]]"
---

# /proc и /sys — глубокий анализ: что собирается и куда уходит

Расширение тем 318/454/30/338. Фокус: **конкретные данные**, **конечные получатели**, **новые находки** о fingerprinting и cross-process information leakage.

---

## 1. Сводная таблица: файл → где читается → куда уходит

| Файл из /proc или /sys | Класс-читатель | Что извлекается | Куда уходит | Критичность |
|---|---|---|---|---|
| `/proc/self/stat` | `k6e.java` (Apptracer perf SDK) | PID, comm, state, **utime/stime/cutime/cstime** (CPU ticks) | `sdk-api.apptracer.ru/api/perf/upload` | 🟡 |
| `/proc/self/stat` | `gy4.java` (BatterySnapshot) | utime/stime/cutime/cstime/starttime/numThreads | `sdk-api.apptracer.ru/api/perf/upload` | 🟡 |
| `/proc/self/statm` | `uoa.java` (MemoryStat) | RSS, shared pages → serialized `xoa` JSON | **OneLog** → `api.oneme.ru` (event type `"memory"`) | 🔴 |
| `/proc/<myPid>/cmdline` | `ij9.java` (Apptracer) | Имя процесса (для session-info) | `sdk-api.apptracer.ru/api/crash/upload` | 🟡 |
| `/proc/<myPid>/cmdline` | `ilb.java` (core utils) | Имя процесса (для логирования) | Внутреннее использование + OneLog | 🟡 |
| `/proc/<myPid>/mounts` | `com.my.tracker.core.o.m` (MyTracker) | **Magisk mount paths** detection | `tracker-api.vk-analytics.ru` (isRooted flag) | 🔴 |
| `/proc/self/fd/<fd>` | `lel.java` | Символические ссылки FD → проверка путей | Внутренняя валидация URI | 🟢 |
| `/proc/self` | `ui7.java` (root check array) | Проверка доступности `/proc/self` | Часть root-detection heuristic | 🟡 |
| `/sys/devices/system/cpu/cpu<N>/cpufreq/cpuinfo_max_freq` | `z5l.java` (DevicePerformanceClass) | **Частота каждого ядра CPU** → device fingerprint | OneLog → `api.oneme.ru` (startup_report) | 🔴 |
| `/sys/devices/system/cpu/kernel_max` | `libEnhancementLibShared.so` | Макс. число CPU ядер | Внутреннее (DSP scheduling) | 🟡 |
| `/sys/devices/system/cpu/cpu%u/cpufreq/cpuinfo_min_freq` | `libEnhancementLibShared.so` | Мин. частота каждого ядра | Внутреннее (DSP scheduling) | 🟡 |
| `/sys/devices/system/cpu/cpu%u/cpufreq/cpuinfo_max_freq` | `libEnhancementLibShared.so` | Макс. частота каждого ядра | Внутреннее (DSP scheduling) | 🟡 |
| `/sys/devices/system/cpu/cpu%u/topology/physical_package_id` | `libEnhancementLibShared.so` | Топология CPU (big.LITTLE) | Внутреннее | 🟡 |
| `/sys/devices/system/cpu/cpu%u/topology/core_siblings_list` | `libEnhancementLibShared.so` | Siblings list | Внутреннее | 🟡 |
| `/sys/devices/system/cpu/possible` | `libtracernative.so`, `libjingle_peerconnection_so.so` | Диапазон CPU | Внутреннее | 🟢 |
| `/sys/devices/system/cpu/present` | `libtracernative.so`, `libjingle_peerconnection_so.so` | Активные CPU | Внутреннее | 🟢 |
| `/proc/cpuinfo` | `libEnhancementLibShared.so`, `libtracernative.so`, `libjingle_peerconnection_so.so` | CPU features, model | Внутреннее (NEON/SSE detection) | 🟢 |

---

## 2. НОВЫЕ КРИТИЧЕСКИЕ НАХОДКИ

### 2.1. CPU Frequency Fingerprint → OneLog → api.oneme.ru

**Файл:** `z5l.java` (DevicePerformanceClass)

```java
RandomAccessFile randomAccessFile = new RandomAccessFile(
    "/sys/devices/system/cpu/cpu" + i4 + "/cpufreq/cpuinfo_max_freq", "r");
String line = randomAccessFile.readLine();
i3 += Integer.parseInt(line) / 1000;  // суммарная частота всех ядер
```

Результат (`DevicePerformanceClass` = HIGH/AVERAGE/LOW) вычисляется из:
- **Частоты каждого ядра CPU** (из `/sys/`)
- Количества ядер
- Объёма RAM
- `memoryClass`

Этот класс используется в `startup_report` event (тип `jm5.b`), который отправляется через `ok9.h()` → **OneLog** → `api.oneme.ru`.

**Почему критично:** Комбинация (число ядер × частота каждого × RAM) — это **hardware fingerprint**, уникально идентифицирующий модель устройства с точностью до конкретного чипсета. В сочетании с `Build.SOC_MODEL` (Android 31+) это полный CPU fingerprint.

### 2.2. /proc/self/statm → Serialized Memory Snapshot → OneLog

**Файл:** `uoa.java`

```java
List listL0 = o0i.L0((CharSequence) q04.E0(nv6.Z(new File("/proc/self/statm"))), 
    new String[]{" "}, 6);
// Извлекает: RSS pages, shared pages → конвертирует в байты через sysconf(_SC_PAGESIZE)
```

Результат сериализуется в `xoa` (MemoryInfo JSON) через `mz8Var.b(xoa.Companion.serializer(), xoaVarC)` и отправляется как `valueStr` в event `"memory"` через `km5.a()` → `ok9.h("DEV", "memory", ...)` → **OneLog** → `api.oneme.ru`.

**Что уходит на сервер:**
- `value` = MemoryInfo.dalvikPss
- `value2` = trimLevel
- `value3` = lowMemory flag
- `value4` = availMem
- `value5` = totalMem
- `value6` = threshold
- `value7` = largeMemoryClass
- `value8` = RSS from /proc/self/statm
- `value9` = shared pages from /proc/self/statm
- `valueStr` = **полный serialized xoa (MemoryInfo)** JSON
- `valueStr2` = **список GC events** (serialized)

**Почему критично:** Серверу отправляется **полный memory profile** процесса, включая RSS/shared из `/proc/self/statm`. Это позволяет серверу отслеживать паттерны использования памяти, что может коррелировать с активностью пользователя (открытые вкладки, количество чатов, медиа в памяти).

### 2.3. MyTracker: /proc/<pid>/mounts → Magisk Detection → vk-analytics.ru

**Файл:** `com/my/tracker/core/o/m.java`

```java
r3 = new BufferedReader(new InputStreamReader(
    new FileInputStream("/proc/" + Process.myPid() + "/mounts")));
String[] magiskPaths = {"/sbin/.magisk/", "/sbin/.core/mirror", 
    "/sbin/.core/img", "/sbin/.core/db-0/magisk.db"};
// Проверяет каждую строку mounts на наличие Magisk-путей
```

Результат (`isRooted = true/false`) отправляется на `tracker-api.vk-analytics.ru` как часть device params.

**Дополнительно MyTracker собирает:**
- Список **всех установленных приложений** (non-system) через `InstalledPackagesProvider.getInstalledPackages()` → `f.java`
- Package name + firstInstallTime каждого приложения
- Hash списка для отслеживания изменений

**Почему критично:** VK-аналитика получает:
1. Факт root/Magisk на устройстве
2. **Полный список установленных приложений** с датами установки
3. Это позволяет профилировать пользователя (VPN-клиенты, мессенджеры конкурентов, банковские приложения)

### 2.4. Apptracer Crash Upload: isRooted + Device Fingerprint → sdk-api.apptracer.ru

**Файл:** `mpf.java`

При каждом crash/non-fatal event на `sdk-api.apptracer.ru/api/crash/upload` отправляется:
```json
{
  "packageName": "ru.oneme.app",
  "versionName": "...",
  "device": "Build.MODEL",
  "deviceId": "...",           // ← persistent device ID
  "vendor": "Build.MANUFACTURER",
  "osVersion": "SDK_INT",
  "inBackground": true/false,
  "connection": "WIFI/MOBILE",
  "isRooted": true/false,      // ← r04.G(context) — root detection
  "properties": {
    "board": "Build.BOARD",
    "brand": "Build.BRAND",
    "cpuABI": "arm64-v8a",
    "cpuCount": "8",           // ← Runtime.availableProcessors()
    "osVersionRelease": "14"
  }
}
```

### 2.5. Process Enumeration через ActivityManager (не /proc напрямую)

**Файл:** `w6e.java`

```java
List<ActivityManager.RunningAppProcessInfo> runningAppProcesses = 
    ((ActivityManager) context.getSystemService("activity")).getRunningAppProcesses();
```

Используется для определения имени текущего процесса (fallback для API < 28). На Android < 28 это возвращает **список всех процессов** приложения, а на старых версиях — потенциально процессы других приложений.

---

## 3. Что НЕ найдено (отсутствует в коде)

| Файл | Статус |
|---|---|
| `/proc/net/tcp`, `/proc/net/tcp6`, `/proc/net/udp` | ❌ Не найдено ни в Java, ни в native |
| `/proc/net/route` | ❌ Не найдено |
| `/proc/net/arp` | ❌ Не найдено |
| `/proc/<pid>/maps` (чужих процессов) | ❌ Не найдено |
| `/proc/<pid>/status` | ❌ Не найдено |
| `/proc/<pid>/cgroup` | ❌ Не найдено |
| `/proc/<pid>/oom_score` | ❌ Не найдено |
| `/proc/<pid>/wchan` | ❌ Не найдено |
| `/sys/class/net/` | ❌ Не найдено |
| `/sys/class/power_supply/` | ❌ Не найдено |
| `/sys/class/thermal/` | ❌ Не найдено |
| Перечисление чужих PID через `/proc/` | ❌ Не найдено (только свой PID) |

**Важно:** Отсутствие `/proc/net/tcp` не означает отсутствие network fingerprinting — MyTracker собирает network state через Android API (`ConnectivityManager`), а не через procfs.

---

## 4. Цепочка данных: /proc → JSON → сервер

```
/proc/self/stat ──→ k6e.java (CPU ticks) ──→ nei.java ──→ sdk-api.apptracer.ru/api/perf/upload
                                           ──→ gy4.java (BatterySnapshot) ──→ same endpoint

/proc/self/statm ──→ uoa.java (RSS/shared) ──→ km5.java ──→ ok9.h("DEV","memory",...) 
                                                          ──→ OneLog ──→ api.oneme.ru

/proc/<myPid>/cmdline ──→ ij9.java ──→ zbi session-info ──→ sdk-api.apptracer.ru/api/crash/upload
                      ──→ ilb.java ──→ internal process name resolution

/proc/<myPid>/mounts ──→ MyTracker m.java ──→ isRooted flag ──→ tracker-api.vk-analytics.ru

/sys/.../cpuinfo_max_freq ──→ z5l.java ──→ DevicePerformanceClass ──→ OneLog startup_report 
                                                                    ──→ api.oneme.ru
```

---

## 5. Итоговая оценка

**Три независимых получателя данных из /proc и /sys:**

1. **sdk-api.apptracer.ru** (OK.ru/VK infra) — CPU ticks, process name, isRooted, device fingerprint
2. **tracker-api.vk-analytics.ru** (MyTracker/VK) — isRooted (Magisk detection), installed apps list
3. **api.oneme.ru** (MAX backend через OneLog) — memory profile из /proc/self/statm, CPU fingerprint из /sys/

**Surveillance-значимость:**
- Сервер MAX получает **hardware fingerprint** (CPU topology + freq + RAM) достаточный для cross-app tracking
- VK-аналитика получает **полный список установленных приложений** + root status
- Apptracer получает **CPU usage patterns** (utime/stime) позволяющие определить активность пользователя
- Все три канала содержат persistent `deviceId` для корреляции

**Отсутствие /proc/net/tcp и перечисления чужих процессов** — положительный момент: MAX не сканирует сетевую активность других приложений и не перечисляет чужие процессы через procfs. Однако MyTracker компенсирует это через `getInstalledPackages()` API.
