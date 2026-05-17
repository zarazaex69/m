---
tags: [exec, shell, process, root-detection, logcat, native, wave2]
status: complete
sources: [jadx_base/sources, findings/native/*.strings.txt]
related: [[15-on-device-asr-kws-diarization]], [[17-apptracer-uplink]], [[05-dev-menu-in-prod]]
---

# 08 — Runtime.exec / ProcessBuilder / Native Shell — полный реестр

> Волна 2: верификация. Поиск по всем Java-источникам и нативным строкам.

## Сводка

| Категория | Кол-во уникальных call-sites |
|-----------|------------------------------|
| `Runtime.getRuntime().exec()` — shell-команды | **2** |
| `new ProcessBuilder` — shell-команды | **2** (1 файл) |
| Root-detection (file-exists + exec `which su`) | **2** (2 файла) |
| `/proc/self/*` чтение (не exec) | **5** (4 файла) |
| `__system_property_get` (native, не exec) | **3** либы |
| Реальный `system()`/`popen()`/`execv*` в native | **0** |

**Вывод:** приложение запускает ровно **2 внешних процесса** — оба `logcat`. Плюс MyTracker выполняет `which su` для root-detection. Никаких произвольных shell-команд, скриптов, `su`, `pm install`, `dumpsys`, `netstat`, `iptables` — **не обнаружено**.

---

## Таблица всех находок

### 1. Runtime.getRuntime().exec() — реальное выполнение команд

| # | Файл | Строка | Команда | Аргументы | Вызывающий контекст |
|---|------|--------|---------|-----------|---------------------|
| 1 | `defpackage/hl9.java:44` | `Runtime.getRuntime().exec(new String[]{"logcat", "-v", "tag", "-T", <timestamp>})` | `logcat` | Фиксированные: `-v tag -T <дата>` | `il9` → `hl9` (coroutine). Читает stdout logcat построчно, передаёт в callback `il9.c`. Используется в `IntegrityLogsViewerScreen` (DevMenu). Процесс хранится в `il9.d`, уничтожается в `np8.java`. |
| 2 | `com/my/tracker/core/o/m.java:130` | `runtime.exec(strArr2[i2])` | `which su` | Фиксированные: `"/system/xbin/which su"`, `"/system/bin/which su"`, `"which su"` | MyTracker SDK root-detection. Проверяет наличие `su` binary. Результат — boolean `isRooted`. |

### 2. ProcessBuilder — реальное выполнение команд

| # | Файл | Строка | Команда | Аргументы | Вызывающий контекст |
|---|------|--------|---------|-----------|---------------------|
| 3 | `defpackage/g85.java:66` | `new ProcessBuilder().command(r04.L("logcat", "-f", <path>, "-b", "all", "-v", "long", "-t", "4096"))` | `logcat` | `-f <path>/all.log -b all -v long -t 4096` (дамп последних 4096 строк в файл) | Метод `a()` — одноразовый дамп logcat в файл. Путь из `jai` (lazy-инициализированный `logcat_logs` dir). |
| 4 | `defpackage/g85.java:136` | `new ProcessBuilder().command(r04.L("logcat", "-f", <path>, "-r", "8196", "-n", "4", "-b", "all", "-v", "long"))` | `logcat` | `-f <path> -r 8196 -n 4 -b all -v long` (ротация: 4 файла по 8 KB) | Метод `c()` — долгоживущий процесс logcat с ротацией. Процесс сохраняется через `hc1(14, processStart)`. |

### 3. Root-detection (file-exists, без exec)

| # | Файл | Строка | Что проверяется | Контекст |
|---|------|--------|-----------------|----------|
| 5 | `defpackage/r04.java:148-149` | `new File("/system/app/Superuser.apk").exists()` + `new File("/system/xbin/su").exists()` | Root-detection | Метод `G(Context)` — встроенная проверка MAX. Также проверяет `Build.TAGS.contains("test-keys")` и `Build.PRODUCT == "sdk"`. |
| 6 | `com/my/tracker/core/o/m.java:116` | File.exists() по 10 путям: `/system/app/Superuser.apk`, `/sbin/su`, `/system/bin/su`, `/system/xbin/su`, `/data/local/xbin/su`, `/data/local/bin/su`, `/system/sd/xbin/su`, `/system/bin/failsafe/su`, `/data/local/su`, `/su/bin/su` | Root-detection | MyTracker SDK. Отправляет `isRooted` в телеметрию. |

### 4. /proc/self/* — чтение файлов (не exec)

| # | Файл | Строка | Что читается | Контекст |
|---|------|--------|--------------|----------|
| 7 | `defpackage/k6e.java:13` | `new File("/proc/self/stat")` | CPU time (utime/stime/cutime/cstime/starttime/vsize) | Apptracer performance monitoring — парсит поля 13-24 из `/proc/self/stat`. |
| 8 | `defpackage/gy4.java:62` | `new RandomAccessFile("/proc/self/stat", "r")` | То же — CPU ticks | Дублирующий reader для performance metrics. |
| 9 | `defpackage/uoa.java:62` | `new File("/proc/self/statm")` | RSS/shared memory (в страницах) | Memory monitoring — умножает на `_SC_PAGESIZE`. |
| 10 | `defpackage/ui7.java:47` | `String[] i = {"/proc/self", "/data/data/ru.oneme.app"}` | Константы путей | Utility class — используется для проверки доступности путей (не exec). |
| 11 | `com/facebook/soloader/SysUtil$LollipopSysdeps.java:62` | `Os.readlink("/proc/self/exe")` | ABI detection (64-bit?) | Facebook SoLoader — определяет архитектуру процесса. |
| 12 | `defpackage/lel.java:16` | `Files.readSymbolicLink("/proc/self/fd/<fd>")` | Resolve file path from fd | Utility — получает реальный путь файла по file descriptor. |

### 5. Серверный флаг `android-use-logcat-logger` (PmsKey)

| Файл | Описание |
|------|----------|
| `PmsKey.java:43` | `f4androiduselogcatlogger = new PmsKey("android-use-logcat-logger", 5)` |
| `rtd.java:84` | Привязан к `jbg` (boolean PmsKey observer) — **сервер может включить/выключить logcat-логирование** |
| `j6.java:91` | Enum: `EMBEDDED` (1) vs `LOGCAT` (2) — выбор backend'а логирования |
| `h6.java:51` | Путь: `<dataDir>/logcat_logs` |

**Важно:** сервер через PmsKey `android-use-logcat-logger` может активировать запуск процесса `logcat` на устройстве пользователя. Логи пишутся в `logcat_logs/` и потенциально отправляются через `userdebugreport` (PmsKey `f292userdebugreport`).

### 6. Runtime.getRuntime() — НЕ exec (только availableProcessors/maxMemory)

| Файл | Использование |
|------|---------------|
| `bbi.java`, `g6.java`, `db4.java`, `ga4.java`, `hj5.java`, `pd5.java`, `d92.java`, `mpf.java`, `aec.java`, `l94.java`, `fg.java`, `z5l.java`, `vw0.java`, `ghg.java`, `ngb.java`, `y50.java`, `ilb.java` | `availableProcessors()` — размер thread pool |
| `yzd.java`, `gz0.java`, `dd5.java`, `pb5.java` | `maxMemory()` — лимит heap |
| `bj6.java:1663` | `Runtime.getRuntime()` сохраняется для `nativeLoad()` через reflection (Facebook SoLoader) |

### 7. Нативные библиотеки — system/exec/fork

| Библиотека | Найдено | Оценка |
|------------|---------|--------|
| `libtracernative.so` | `__system_property_get`, `system` (как C++ namespace `boost::system`) | **Нет** реального `system()` call — это boost::system::error_category |
| `libEnhancementLibShared.so` | `__system_property_get`, `/sys/devices/system/cpu/*` | Чтение CPU topology. **Нет** shell exec. |
| `libjingle_peerconnection_so.so` | `__system_property_get`, `/sys/devices/system/cpu/*` | WebRTC. **Нет** shell exec. |
| `libffmpg.so` | `__register_atfork` | Стандартный libc. **Нет** shell exec. |

**Ни одна нативная библиотека не экспортирует и не содержит строк `system()`, `popen()`, `execvp()`, `execve()`, `/bin/sh`, `/system/bin/su`, `fork()` (кроме `__register_atfork` в libc).**

---

## Отсутствующие паттерны (подтверждённый negative)

Следующие паттерны **НЕ найдены** в Java-коде:

| Паттерн | Результат |
|---------|-----------|
| `pm list` / `pm install` / `pm grant` | ❌ Не найдено |
| `dumpsys` | ❌ Не найдено |
| `netstat` (как команда) | ❌ Только `PmsKey.netstatconfig` (WS opcode logging config) |
| `ifconfig` | ❌ Не найдено |
| `iptables` | ❌ Не найдено |
| `mount` | ❌ Не найдено |
| `kill -` | ❌ Не найдено |
| `execLine` / `execCommand` | ❌ Не найдено |
| `.sh` (shell scripts) | ❌ Не найдено (только `Shape`, `Short`, `SharedPreferences` etc.) |
| `"su"` (как команда) | ❌ Только root-detection (file exists + `which su`) |

---

## Цепочка вызовов logcat

```
[Сервер] → PmsKey "android-use-logcat-logger" = true
    → rtd.java (jbg observer)
        → j6.java: выбирает LOGCAT backend
            → g85(h6, ContextScope) constructor
                → g85.a() — одноразовый дамп (4096 строк)
                → g85.c() — долгоживущий процесс с ротацией

[DevMenu] → IntegrityLogsViewerScreen
    → np8.java → il9 → hl9.java
        → Runtime.exec("logcat -v tag -T <timestamp>")
        → читает stdout построчно
        → np8 уничтожает процесс через process.destroy()
```

---

## Оценка рисков

| Находка | Риск | Обоснование |
|---------|------|-------------|
| Logcat capture (g85/hl9) | 🟡 Средний | Logcat содержит данные всех приложений (до Android 11). Серверный флаг может включить сбор. Логи потенциально отправляются через `userdebugreport`. |
| MyTracker `which su` | 🟢 Низкий | Стандартная root-detection для антифрода. Результат — boolean. |
| `/proc/self/stat[m]` | 🟢 Низкий | Стандартный performance monitoring. Только CPU/memory метрики. |
| Серверный контроль logcat | 🟡 Средний | PmsKey позволяет серверу удалённо включить logcat-capture без ведома пользователя. |

---

## Верификация hl9.java (из предыдущего анализа)

**Подтверждено:**
- `hl9.java` — coroutine, запускает `logcat -v tag -T <timestamp>`
- Хранит `Process` в `il9.d`
- Читает stdout через `BufferedReader` в цикле `while(ti3.F(nx4Var))`
- Каждая строка передаётся в callback `il9.c.invoke(line)`
- Вызывается из `np8.java` → `IntegrityLogsViewerScreen` (DevMenu)
- Процесс уничтожается через `il9.d.destroy()` в `np8.n()`
- Ошибки логируются: "Ошибка чтения logcat", "Ошибка инициализации чтения logcat", "Ошибка завершения процесса чтения logcat"
