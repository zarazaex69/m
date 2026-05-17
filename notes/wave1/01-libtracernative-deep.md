# libtracernative.so — глубокий реверс

## Резюме

`libtracernative.so` (799 KB, NDK r27, boost-1.87.0) — нативная библиотека crash-reporting стека Apptracer (ok.ru). Содержит полный **Google Breakpad** с `LinuxPtraceDumper`, микродамп-генератором и signal handler'ами. Ключевые находки ниже.

---

## 1. LinuxPtraceDumper — ptrace-based полный дамп процесса

**Строки:**
```
N15google_breakpad17LinuxPtraceDumperE   (line 762)
N15google_breakpad11LinuxDumperE          (line 760)
ExceptionHandler::GenerateDump sys_pipe failed:  (line 751)
ExceptionHandler::GenerateDump waitpid failed:   (line 752)
```

**Что это:** `google_breakpad::LinuxPtraceDumper` — класс, который при крэше:
1. Форкает дочерний процесс (`sys_pipe`, `waitpid`)
2. Из дочернего процесса вызывает `ptrace(PTRACE_ATTACH)` к каждому треду родителя
3. Читает `/proc/[pid]/maps`, `/proc/[pid]/auxv`, `/proc/[pid]/environ`, `/proc/[pid]/cmdline`
4. Дампит регистры и стек каждого треда через `ptrace(PTRACE_PEEKDATA)`
5. Записывает MDMP-файл (строка `MDMP` — magic минидампа, line 169)

**Подтверждающие строки:**
- `Tgid:\t` (line 558) — парсинг `/proc/[pid]/status` для определения thread group
- `PPid:\t` (line 218) — парсинг parent PID
- `maps` (line 271) — чтение `/proc/self/maps`
- `auxv` (line 288) — чтение `/proc/self/auxv`
- `environ` (line 508) — чтение `/proc/self/environ`
- `cmdline` (line 290) — чтение `/proc/self/cmdline`
- `.dmp` (line 464) — расширение файлов минидампа
- `sendfile` (line 79) — zero-copy передача файлов (дампов) между fd
- `syscall` (line 80) — прямые syscall'ы (ptrace, clone, tgkill)

**Импакт:** Минидамп содержит **полный snapshot памяти процесса** на момент крэша: все стеки всех тредов, регистры, карту памяти, переменные окружения, аргументы командной строки. Это включает:
- Ключи шифрования в памяти
- Токены авторизации
- Содержимое сообщений в буферах
- Пароли в полях ввода

Минидамп затем загружается на `https://sdk-api.apptracer.ru` через `SampleUploadWorker`.

---

## 2. DUMP_REQUESTED — триггер дампа по внешнему сигналу

**Строка:** `DUMP_REQUESTED` (line 528)

**Контекст:** В Breakpad `DUMP_REQUESTED` — это флаг, означающий что дамп был запрошен **не из-за крэша**, а программно. Breakpad поддерживает `ExceptionHandler::WriteMinidump()` — генерацию минидампа по запросу, без фатального сигнала.

**Связь с Java:** Функция `tracer_supports_arbitrary_dump()` (export) возвращает 1, если backend поддерживает генерацию дампа по запросу. В текущей сборке эта функция **не вызывается через JNI**, но сам механизм присутствует и активен в нативном коде.

**Импакт:** Сервер теоретически может инициировать дамп памяти процесса без крэша — через Java-вызов `tracer_upload` или `tracer_upload_async` (оба экспортированы).

---

## 3. Breakpad Microdump — компактный дамп в logcat

**Строки:**
```
-----BEGIN BREAKPAD MICRODUMP-----   (line 284)
-----END BREAKPAD MICRODUMP-----     (line 372)
Microdump skipped (uninteresting)    (line 359)
```

**Что это:** Microdump — альтернативный формат дампа, который пишется **в logcat** (через `__android_log_buf_write`, line 283). Содержит стек, регистры и карту модулей в текстовом виде.

**Импакт:** Если logcat перехватывается (а в MAX есть `LogcatCapture` → `apptracer.ru`, topic 535), то microdump утекает вместе с логами — без отдельного upload-механизма.

---

## 4. tracer_set_api_endpoint — runtime-подмена URL загрузки

**Export:** `tracer_set_api_endpoint` (addr 0x4fed4)

**Декомпиляция:** Принимает строку URL, передаёт через vtable (offset 0x50) в backend. Позволяет **в runtime** перенаправить все uploads на произвольный сервер.

**Текущее использование:** В JNI-обвязке **не вызывается**. Endpoint захардкожен в Java как `https://sdk-api.apptracer.ru`. Однако функция экспортирована и доступна через `dlsym` из любой другой `.so` в процессе.

**Импакт:** Любая библиотека в процессе (включая динамически загружаемые мини-аппы через WebView) может вызвать `dlsym(RTLD_DEFAULT, "tracer_set_api_endpoint")` и перенаправить дампы на свой сервер.

---

## 5. tracer_set_ssl_cainfo — подмена CA bundle

**Export:** `tracer_set_ssl_cainfo` (addr 0x4fd80)

**Декомпиляция:** Принимает два аргумента (path, content?), передаёт через vtable (offset 0x48) в backend. Позволяет задать **собственный CA certificate** для TLS-соединений tracer-стека.

**Импакт:** В комбинации с `tracer_set_api_endpoint` позволяет полностью перенаправить uploads на MITM-сервер с самоподписанным сертификатом. Оба вызова доступны через `dlsym`.

---

## 6. NativeBridge — обратный канал native→Java

**JNI_OnLoad регистрирует:**
```c
FindClass("ru/ok/tracer/nativebridge/NativeBridge")
GetMethodID(class, "setKey", "(Ljava/lang/String;Ljava/lang/String;)V")
GetMethodID(class, <0x23e79>, "(Ljava/lang/String;)V")  // "log" method
```

**Java-реализация (qb5.java):**
```java
public void setKey(String str, String str2) {
    // Передаёт key-value в adi.c(map) → сохраняет в файл tags/
    // Эти теги прикрепляются ко ВСЕМ crash-отчётам
    osi.f.c(Collections.singletonMap(str, str2));
}
```

**Импакт:** Нативный код может записывать произвольные метаданные (теги), которые затем отправляются на сервер вместе с крэш-отчётами. Это канал exfiltration: нативный код может закодировать данные в "тегах" и они уйдут на `sdk-api.apptracer.ru`.

---

## 7. SampleUploadWorker — загрузка произвольных файлов

**Файл:** `ru/ok/tracer/upload/SampleUploadWorker.java`

**Механизм:**
1. Получает путь файла из `WorkerParameters` → `tracer_sample_file_path`
2. Запрашивает `uploadToken` у `https://sdk-api.apptracer.ru/api/sample/initUpload?sampleToken=...`
3. Загружает файл на `https://sdk-api.apptracer.ru/api/sample/upload?uploadToken=...`
4. Поддерживает произвольные `tracer_custom_properties_keys` — метаданные

**Кто вызывает `xl2.v()` (триггер upload):**
- `ShrinkDumpWorker` — загружает heap dump (путь из `param_dump_path`)
- `DiskUsageWorker` — загружает JSON с полной картой файловой системы приложения

**Импакт:** `SampleUploadWorker` — универсальный uploader **любого файла** по пути. Путь задаётся через `WorkerParameters`. Если сервер может триггерить WorkManager задачи (через FCM push → topic 539), он может загрузить произвольный файл из sandbox приложения.

---

## 8. DiskUsageWorker — полная карта файловой системы

**Файл:** `ru/ok/tracer/disk/usage/DiskUsageWorker.java`

**Что делает:**
- Рекурсивно обходит `dataDir`, `externalFilesDir`, `sourceDir` (до глубины 6)
- Собирает имена файлов, размеры, структуру каталогов
- Сериализует в JSON и загружает через `xl2.v()` → `SampleUploadWorker`

**Импакт:** Сервер получает **полную карту файлов** приложения — включая имена баз данных, кэшей, shared preferences. Это reconnaissance для последующей целевой exfiltration.

---

## 9. Перехватываемые сигналы

Из строк библиотеки, breakpad handler перехватывает:
- `SIGSEGV` (segfault)
- `SIGABRT` (abort)
- `SIGFPE` (floating point)
- `SIGILL` (illegal instruction)
- `SIGTRAP` (breakpoint/debug)
- `SIGBUS` (bus error)
- `SIGSYS` (bad syscall)
- `SIGPIPE` (broken pipe)

Используются: `sigaction`, `sigemptyset`, `sigaddset` (lines 95-97).

При каждом из этих сигналов генерируется полный минидамп через `LinuxPtraceDumper`.

---

## 10. tracer_upload / tracer_upload_async — немедленная отправка

**Exports:**
```
tracer_upload       (addr 0x50544)
tracer_upload_async (addr 0x505e8)
```

Эти функции инициируют **немедленную** загрузку накопленных крэш-дампов. `tracer_upload_async` создаёт отдельный pthread (`pthread_create`, line 45) для фоновой отправки.

---

## 11. CrashReportInitializer — ApplicationExitInfo + main thread snapshots

**Файл:** `ru/ok/tracer/crash/report/CrashReportInitializer.java`

**Новая находка:** На Android 11+ использует `ActivityManager.getHistoricalProcessExitReasons()` + `ApplicationExitInfo.getTraceInputStream()` для получения **ANR trace** (полный стек всех тредов на момент ANR). Дополнительно читает `main_snapshots/` — периодические снимки стека main thread.

Всё это отправляется на `sdk-api.apptracer.ru` через `my4Var3.b(10, ...)` (тип 10 = ANR с trace).

---

## 12. Аврора-совместимость

**Export:** `tracer_aurora_collect_minidump_from_cachedir` (addr 0x50140)

Подтверждает: один и тот же tracer-движок используется для Android и Аврора ОС (российский fork Sailfish). Минидампы на Авроре пишутся в cache-директорию и собираются отдельно.

---

## Итоговая оценка угрозы

| Вектор | Severity | Доступность |
|--------|----------|-------------|
| ptrace-based полный дамп памяти процесса | CRITICAL | При любом крэше (включая искусственный SIGABRT) |
| Microdump в logcat → logcat capture → apptracer.ru | HIGH | Автоматически при крэше |
| DUMP_REQUESTED — дамп без крэша | HIGH | Через `tracer_upload`/`tracer_upload_async` (нужен нативный вызов) |
| SampleUploadWorker — upload произвольного файла | HIGH | Через WorkManager (потенциально FCM-триггер) |
| DiskUsageWorker — карта файловой системы | MEDIUM | Автоматически при превышении 10GB |
| tracer_set_api_endpoint через dlsym | MEDIUM | Из любой .so в процессе |
| NativeBridge setKey — exfiltration через теги | LOW | Только из нативного кода |

**Ключевой вывод:** `libtracernative.so` — это не просто crash reporter. Это полноценный **memory forensics toolkit**, который при каждом крэше (или по запросу) снимает полный snapshot памяти процесса (включая ключи, токены, сообщения) и отправляет на `sdk-api.apptracer.ru`. В комбинации с возможностью сервера вызвать крэш (через malformed push, killswitch, или server-side JS injection в WebView) — это механизм **on-demand memory extraction**.

---

## Источники

- `/home/reverser/max/findings/native/libtracernative.exports.txt`
- `/home/reverser/max/findings/native/libtracernative.strings.txt`
- `/home/reverser/max/findings/native/decomp_tracer/*.c`
- `/home/reverser/max/findings/native/decomp_tracer/README.md`
- `/home/reverser/max/work/jadx_base/sources/ru/ok/tracer/nativebridge/NativeBridge*.java`
- `/home/reverser/max/work/jadx_base/sources/ru/ok/tracer/minidump/Minidump.java`
- `/home/reverser/max/work/jadx_base/sources/ru/ok/tracer/upload/SampleUploadWorker.java`
- `/home/reverser/max/work/jadx_base/sources/ru/ok/tracer/crash/report/CrashReportInitializer.java`
- `/home/reverser/max/work/jadx_base/sources/ru/ok/tracer/disk/usage/DiskUsageWorker.java`
- `/home/reverser/max/work/jadx_base/sources/ru/ok/tracer/heap/dumps/exceptions/ShrinkDumpWorker.java`
- `/home/reverser/max/work/jadx_base/sources/defpackage/qb5.java` (NativeBridge impl)
- `/home/reverser/max/work/jadx_base/sources/defpackage/xl2.java` (sample upload trigger)
