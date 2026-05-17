# Hidden APIs, Reflection & Dynamic Code Loading в MAX

**Статус:** новая находка  
**Версия:** 26.15.3 (подтверждено в 26.16.0)  
**Критичность:** HIGH — обфусцированный фреймворк обхода hidden API + скрытый SDK «trace-flow.ru» (DPS) с IP-фингерпринтингом и VPN-детекцией

---

## TL;DR

MAX содержит:
1. **Обфусцированный фреймворк обхода Android Hidden API restrictions** (`e1l` / `hph` / `z8f`) — все имена классов и методов закодированы XOR-шифром через `z8f.a()`, чтобы избежать обнаружения статическим анализом.
2. **Скрытый SDK «trace-flow.ru» (DPS)** — ContentProvider-based SDK, который при старте приложения проводит сетевую разведку (DNS resolve, TCP connect, TLS handshake) к серверно-задаваемым хостам и отправляет результаты + VPN-статус + оператора на `https://trace-flow.ru/api/v1/report`.
3. **Custom PathClassLoader (`v7l`)** — подменяет стандартный ClassLoader для Google DynamiteModule, приоритизируя собственные классы над системными.
4. **SoLoader с Runtime.nativeLoad() reflection** — обходит стандартный System.loadLibrary() для загрузки .so из произвольных путей.
5. **ProcessBuilder/Runtime.exec() для logcat capture** — захват полного системного лога через shell-команды.

---

## 1. Обфусцированный Hidden API Bypass Framework

### Архитектура

| Класс | Роль |
|-------|------|
| `z8f.a(String)` | XOR-декодер строк (ключ в первых 8 hex-символах, XOR по 4-байтному циклу) |
| `e1l` | Lazy-инициализация скрытых Class/Method через `jai` (Lazy) |
| `hph` | Factory-switch на 28 case'ов — каждый загружает скрытый класс или метод |
| `qzk` | Использует hidden API для детальной сетевой разведки |
| `y0l` | Использует hidden API для VPN-детекции через NetworkCapabilities |

### Декодированные скрытые API

```
e1l.a  → Class.forName("java.net.NetworkInterface")
e1l.b  → Class.forName("android.net.NetworkCapabilities")  [HIDDEN API]
e1l.c  → Class.forName("android.net.ConnectivityManager")  [HIDDEN API]
e1l.d  → Class.forName("android.net.Network")

Методы (через reflection):
  - NetworkInterface.getName()
  - NetworkInterface.isUp()
  - NetworkInterface.getNetworkInterfaces()
  - NetworkCapabilities.hasTransport(int)
  - NetworkCapabilities.hasCapability(int)
  - NetworkCapabilities.getLinkDownstreamBandwidthKbps()
  - ConnectivityManager.getActiveNetwork()
  - ConnectivityManager.getNetworkCapabilities(Network)
  - ConnectivityManager.getNetworkInfo(Network)
  - ConnectivityManager.getActiveNetworkInfo()
```

### VPN-детекция через reflection (qzk.java)

Класс `qzk` перечисляет **все** сетевые интерфейсы через reflection и ищет:
- `tun` — стандартный VPN-туннель
- `ppp` — PPP-соединение
- `tap` — TAP-интерфейс
- `ipsec` — IPSec-туннель

Это **обходит** стандартный `ConnectivityManager.getNetworkCapabilities()` API и работает даже на устройствах, где VPN не объявляет `TRANSPORT_VPN`.

### Файлы

- `defpackage/z8f.java:880` — декодер
- `defpackage/e1l.java` — lazy class/method holders
- `defpackage/hph.java` — factory (28 cases)
- `defpackage/qzk.java` — сетевая разведка через hidden API
- `defpackage/y0l.java` — VPN-детекция через hidden API

---

## 2. SDK «trace-flow.ru» (DPS) — Скрытый IP-фингерпринтинг

### Что это

Полноценный SDK (`dpslib`), инициализируемый через ContentProvider `ru.trace_flow.dps.internal.DpsInitProvider` при старте приложения. Все строки обфусцированы через `z8f.a()`.

### Что делает

1. **Получает конфигурацию** с `https://trace-flow.ru` (серверно-задаваемый хост из `bzk.a`)
2. **Проводит 3 типа проверок** к каждому хосту из конфига:
   - `wzk(0)` — DNS resolve (`InetAddress.getAllByName()`)
   - `wzk(1)` — TCP connect на порт 443
   - `wzk(2)` — TLS handshake (SSLSocket)
3. **Собирает метаданные:**
   - Оператор: `TelephonyManager.getNetworkOperator() + ":" + getNetworkOperatorName()`
   - Тип сети (2G/3G/4G/5G) через hidden API `getLinkDownstreamBandwidthKbps()`
   - VPN-статус через `y0l.a()` (hidden API)
   - Foreground/background состояние
4. **Отправляет отчёт** на `https://<host>/api/v1/report?ver=<N>` в JSON:
   ```json
   {"snapshots":[{"id":"<uuid>", ...}]}
   ```

### Конфигурация по умолчанию (hardcoded)

```
Хост отчёта:     https://trace-flow.ru
IP-resolve URLs:  https://ipv4-internet.yandex.net/api/v0/ip
                  https://ipv6-internet.yandex.net/api/v0/ip
                  https://ifconfig.me/ip
                  https://api.ipify.org
                  https://checkip.amazonaws.com
                  https://ip.mail.ru/
IP regex:         \b(?:[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}|[0-9a-fA-F:]+:[0-9a-fA-F:]+)\b
API host:         api.oneme.ru
Timeout:          10000ms
Max snapshots:    50
Snapshot TTL:     86400000ms (24h)
Sample rate:      1.0 (100%)
```

### Серверно-обновляемая конфигурация

Конфиг скачивается с сервера (`kyk.java`) и кэшируется в `dps_config.bin`. Сервер может:
- Менять список хостов для проверки
- Менять timeout/sample rate
- Задавать `dontReportUntil` — временно отключать отчёты

### Файлы

- `ru/trace_flow/dps/internal/DpsInitProvider.java` — ContentProvider-инициализатор
- `defpackage/zu5.java` — основной класс DPS SDK
- `defpackage/kyk.java` — загрузка конфига с сервера
- `defpackage/kzk.java` — кэш конфига (`dps_config.bin`)
- `defpackage/v0l.java` — хранение логов (`dps_log_entries.bin`)
- `defpackage/wzk.java` — 3 типа сетевых проверок (DNS/TCP/TLS)
- `defpackage/bzk.java` — конфиг-объект
- `defpackage/u4k.java` — wall clock provider

### Зачем это нужно

DPS — это **система обнаружения блокировок и VPN**. Она:
1. Определяет, какие хосты доступны с устройства пользователя
2. Определяет, использует ли пользователь VPN
3. Определяет реальный IP пользователя через 6 внешних сервисов
4. Отправляет всё это на `trace-flow.ru` с привязкой к оператору

Это позволяет серверу знать:
- Обходит ли пользователь блокировки
- Какой у него реальный IP (даже если он за VPN — через оператора)
- Какие ресурсы доступны/заблокированы в его сети

---

## 3. Custom PathClassLoader (v7l) — Class Loading Override

```java
// defpackage/v7l.java
public final class v7l extends PathClassLoader {
    @Override
    public final Class loadClass(String str, boolean z) {
        if (!str.startsWith("java.") && !str.startsWith("android.")) {
            try {
                return findClass(str);  // ПРИОРИТЕТ СВОИМ КЛАССАМ
            } catch (ClassNotFoundException unused) {}
        }
        return super.loadClass(str, z);
    }
}
```

Используется в `l06.java` (Google DynamiteModule) для загрузки модулей с **приоритетом собственных классов** над системными. Это позволяет подменять поведение Google Play Services модулей.

### Файлы
- `defpackage/v7l.java` — custom ClassLoader
- `defpackage/l06.java:299` — создание и использование

---

## 4. SoLoader с Runtime.nativeLoad() Reflection

```java
// defpackage/kai.java — getNativeLoadRuntimeMethod()
public static Method getNativeLoadRuntimeMethod() {
    if (Build.VERSION.SDK_INT > 27) return null;
    Method declaredMethod = Runtime.class.getDeclaredMethod(
        "nativeLoad", String.class, ClassLoader.class, String.class);
    declaredMethod.setAccessible(true);
    return declaredMethod;
}
```

```java
// defpackage/bj6.java:810 — System.load(str) с переменным путём
public void Z(int i, String str) {
    if (((Method) this.d) == null) {
        System.load(str);  // ЗАГРУЗКА .so ИЗ ПРОИЗВОЛЬНОГО ПУТИ
        return;
    }
    // Или через reflection: Runtime.nativeLoad(path, classLoader, ldPath)
    ((Method) this.d).invoke((Runtime) this.b, str, SoLoader.class.getClassLoader(), str2);
}
```

Это позволяет загружать нативные библиотеки из **любого пути на файловой системе**, включая скачанные с сервера.

### Файлы
- `defpackage/kai.java` — reflection на Runtime.nativeLoad()
- `defpackage/bj6.java:810` — System.load(variable_path)
- `net/jpountz/lz4/LZ4JNI.java:59` — System.load(file.getAbsolutePath())

---

## 5. ProcessBuilder / Runtime.exec() — Logcat Capture

```java
// defpackage/g85.java:66
new ProcessBuilder(new String[0])
    .command("logcat", "-f", path, "-b", "all", "-v", "long", "-t", "4096")
    .redirectErrorStream(true).start().waitFor();

// defpackage/g85.java:136 — непрерывный logcat с ротацией
new ProcessBuilder(new String[0])
    .command("logcat", "-f", path, "-r", "8196", "-n", "4", "-b", "all", "-v", "long")
    .redirectErrorStream(true).start();

// defpackage/hl9.java:44
Runtime.getRuntime().exec(new String[]{"logcat", "-v", "tag", "-T", timestamp});
```

Захватывает **весь системный лог** (включая логи других приложений на pre-Android 11) с ротацией файлов. Результат отправляется через Apptracer.

---

## 6. ActivityThread Reflection (ba.java)

Доступ к внутренним полям Activity:
- `Activity.mMainThread` — ActivityThread instance
- `Activity.mToken` — IBinder token
- `ActivityThread.performStopActivity()` — принудительная остановка Activity
- `ActivityThread.requestRelaunchActivity()` — принудительный перезапуск

Используется в `aa.java`, `tl7.java`, `p3.java` для управления жизненным циклом Activity в обход стандартного API.

---

## 7. android.os.SystemProperties Reflection

4 места доступа к скрытому API `SystemProperties.get()`:
- `bhj.java:192` — чтение произвольных system properties
- `zgj.java:508` — чтение произвольных system properties
- `hph.java:58` — проверка `ro.miui.ui.version.code` (MIUI detection)
- `com/my/tracker/core/utils/SystemUtils.java:10` — MyTracker SDK

---

## 8. Tracer Dynamic Class Loading

```java
// defpackage/kti.java:38-39
if (str.startsWith("ru.ok.tracer.startup.Initializer@")) {
    String className = bundle.getString(str);
    arrayList.add(Class.forName(className));  // ДИНАМИЧЕСКАЯ ЗАГРУЗКА
}
```

Apptracer загружает классы-инициализаторы **по имени из AndroidManifest metadata**. Имена классов задаются в манифесте — теоретически могут быть изменены через серверный update APK.

---

## 9. Статистика Reflection

| Паттерн | Количество файлов |
|---------|-------------------|
| `Class.forName()` | 71 |
| `setAccessible(true)` | 57 |
| `getDeclaredMethod()` | 38 |
| `getDeclaredField()` | 40+ |
| `sun.misc.Unsafe` | 16 |
| `ProcessBuilder` / `Runtime.exec()` | 3 |
| `System.load(variable)` | 2 |
| `android.os.SystemProperties` | 4 |

---

## 10. Что НЕ найдено

- ❌ `ServiceManager.getService()` — не используется
- ❌ `IPhoneSubInfo` / `ITelephony` / `IPackageManager` — не используется напрямую
- ❌ `InMemoryDexClassLoader` — не используется
- ❌ `HiddenApiBypass` / `FreeReflection` (как библиотека) — вместо этого **собственный** обфусцированный фреймворк
- ❌ Загрузка DEX с сети — не обнаружено (но SoLoader может грузить .so из произвольных путей)

---

## Выводы

1. **DPS SDK (trace-flow.ru)** — это полноценная система сетевой разведки, замаскированная обфускацией строк. Она определяет VPN, реальный IP, доступность хостов и оператора — и отправляет всё на внешний сервер. Конфигурация обновляется с сервера.

2. **Hidden API bypass** реализован не через известные библиотеки (которые легко обнаружить), а через **собственный обфусцированный фреймворк** с XOR-кодированием всех строк. Это сознательное сокрытие от анализа.

3. **SoLoader** может загружать нативные библиотеки из произвольных путей через reflection на `Runtime.nativeLoad()` — потенциальный вектор для загрузки обновлённого нативного кода без обновления APK.

4. **Logcat capture** через ProcessBuilder захватывает весь системный лог и отправляет через Apptracer — на pre-Android 11 это включает логи ВСЕХ приложений.
