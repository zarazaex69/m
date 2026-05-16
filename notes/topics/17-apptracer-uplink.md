---
tags: [telemetry, tracer, apptracer, heap-dump, crash-report, server-control, native-libs]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tracer/**
  - work/jadx_base/sources/defpackage/yxb.java
  - work/jadx_base/sources/defpackage/ati.java
  - work/jadx_base/sources/defpackage/nei.java
  - work/jadx_base/sources/defpackage/ktg.java
  - findings/native/libtracernative.exports.txt
  - findings/native/libtracernative.strings.txt
related:
  - "[[09-native-libs]]"
  - "[[04-telemetry-endpoints]]"
  - "[[03-pms-server-flags]]"
---

# Apptracer стек — что улетает в `sdk-api.apptracer.ru`

В [[04-telemetry-endpoints]] зафиксирован хост `sdk-api.apptracer.ru`. Здесь — конкретика: какие endpoint-ы используются, какие данные туда уходят, что управляется сервером.

## 1. Захардкоженный хост

URL `https://sdk-api.apptracer.ru` встречается в Java коде в нескольких местах:

```
defpackage/ati.java:5     wdh a = new wdh("https://sdk-api.apptracer.ru");
ru/ok/tracer/upload/SampleUploadWorker.java   /api/sample/initUpload, /api/sample/upload
defpackage/yxb.java:621                       /<dynamic str4>?crashToken=...
defpackage/nei.java:188                       /api/perf/upload?crashToken=...
defpackage/ktg.java:41                        /api/crash/trackSession?crashToken=...
```

Эндпоинт **в коде клиента** жёстко зашит, не задаётся из manifest meta-data и не читается из remote config. Нативная функция `tracer_set_api_endpoint` в `libtracernative.so` существует, но в JNI-биндингах (`Java_ru_ok_tracer_*`) не экспортируется и через `NativeBridge` не вызывается. То есть теоретически URL менять можно, но в текущей сборке он не меняется.

## 2. Какие endpoint реально используются

| Endpoint | Что отдаёт | Источник |
|---|---|---|
| `POST /api/sample/initUpload?sampleToken=...` | resumable-upload init | `SampleUploadWorker.h(...)` |
| `PUT /api/sample/upload?uploadToken=...` | тело файла | `SampleUploadWorker.i(...)` |
| `POST /api/perf/upload?crashToken=...` | perf metrics JSON | `defpackage/nei.java:188` |
| `POST /api/crash/trackSession?crashToken=...` | session metadata JSON | `defpackage/ktg.java:41` |
| `POST /<dyn>?crashToken=...` | разные суб-endpoints для краш-флоу | `defpackage/yxb.java:621` |

Token-ы (`sampleToken`, `uploadToken`, `crashToken`) приходят с того же хоста при `init` и используются в URL query.

## 3. Что такое «sample»

`SampleUploadWorker` (сэмпл-uploader) — общий механизм отправки произвольных артефактов. Параметры WorkRequest:

```java
"tracer_sample_file_path"     // путь к файлу на устройстве
"tracer_sample_file_size"
"tracer_sample_file_name"
"tracer_version_code"         // versionCode на момент создания задачи
"tracer_custom_properties_keys" // массив имён доп. метаданных (любое имя)
```

То есть SDK кладёт файл по пути, плюс набор key/value метаданных, и `SampleUploadWorker` через WorkManager заливает это на `sdk-api.apptracer.ru`.

«Sample» — не семантика «образец», это **общий контейнер**: то же API используют heap-dumps, перформанс-логи, recyclerview-traces, anything.

## 4. HeapDump через ShrinkDumpWorker

`ru/ok/tracer/heap/dumps/exceptions/ShrinkDumpWorker`:

- Читает hprof из `param_dump_path` (производится HeapDumpInitializer в native-стороне через minidump-writer? нужно отдельно проверить).
- Если `length < 256MB` (PlaybackStateCompat.ACTION_SET_CAPTIONING_ENABLED == 0x1000_0000, 256 MiB) — **удаляет** файл и выходит. Для крупных дампов идёт обработка.
- Открывает через `x58.C0` (внутренний hprof-парсер), пишет урезанную версию в `ayb.e(context, e4Var)`.
- Удаляет оригинал.
- Регистрирует upload-job через `xl2.v(context, e4Var, fileE, strE2, length, null, 200)` — где `200` это, видимо, тип уплоада «sample/heap».

То есть heap-dump пилится клиентом (выкидываются строки/чувствительные данные? нужно проверить `x58` отдельно), и заливается через тот же `sample` upload-механизм.

`HeapDumpInitializer` сидит в манифесте как один из `androidx.startup.InitializationProvider`'ов. То есть включается на старте приложения.

Условия запуска heap dump-а — серверно-контролируемые через PmsKey `perf-events`, `perf-registrar-config`. То есть **сервер указывает «снять heap-снимок»**, клиент снимает и заливает.

В Java-heap процесса MAX в любой момент могут лежать: сообщения, переписки в DraftStore, сессионный токен, контакты, имена. После шринка часть строк должна вырезаться, но это зависит от реализации `x58` — отдельная тема для проверки.

## 5. Crash report (`/api/crash/trackSession` + breakpad-минидамп)

Из стека `defpackage/yxb.java`, `defpackage/ktg.java`, `defpackage/nei.java`:

- `/api/crash/trackSession` — **session metadata** при старте (для корреляции последующих событий).
- `/api/perf/upload` — perf metrics (память, CPU, frame drops).
- `/api/<dyn>?crashToken=...` — разные суб-роуты для разных типов краша/жалоб.

На native-стороне (`libtracernative.so`):

```
JNI_OnLoad
Java_ru_ok_tracer_minidump_Minidump_installMinidumpWriterImpl
Java_ru_ok_tracer_minidump_Minidump_uninstallMinidumpWriterImpl
Java_ru_ok_tracer_nativebridge_NativeBridgeInstaller_nativeInstallBridge

tracer_init
tracer_install_crash_handler
tracer_set_api_endpoint                 # <— функция есть, но Java-вызов не найден
tracer_set_key
tracer_set_ssl_cainfo                   # <— функция есть, но Java-вызов не найден
tracer_set_platform_info / _info2
tracer_set_userid
tracer_log
tracer_disable_upload
tracer_keep_processed_crashes
tracer_supports_arbitrary_dump          # <— название тревожное, без xref-а из Java
tracer_aurora_collect_minidump_from_cachedir
tracer_crashpad_set_handler_path
tracer_hardfix_crashclient
tracer_report_nonfatal_from_here
tracer_upload / tracer_upload_async / tracer_upload_crashes
```

Что важно:

1. **`tracer_aurora_collect_minidump_from_cachedir`** — отсылка к ОС «Аврора» (российская мобильная ОС от Ростелекома, fork Sailfish/MerProject). Tracer-стек собирается универсально под Android и Аврору — и для Авроры есть **отдельная функция сбора minidump из cache-каталога**. То есть один и тот же tracer-движок используется в android-сборке MAX и в гипотетической сборке под Аврору.
2. **`tracer_supports_arbitrary_dump`** — функция, которая «поддерживает произвольный дамп». По имени — это, видимо, флаг capabilities для текущей платформы (умеет ли native-код снять дамп процесса целиком в текущем окружении). Без xref-ов в Java и без чтения её call-graph в Ghidra — нельзя утверждать «снимает дамп процесса по серверной команде». Мой текущий статус — **подозрительно по имени, не подтверждено**.
3. **`tracer_set_ssl_cainfo`** — на нативной стороне сетевой клиент tracer-а (вероятно curl/quiche) принимает **кастомный CA bundle**. Это типовая штука для embedded-приложений с pinning-ом. В JNI на Java-сторону не выставлено, реальное значение задаётся внутри `tracer_init` (нужен Ghidra). Но факт, что функция есть, — повод проверить внутри `.so`, не загружается ли там CA из произвольного места.
4. **Под Android** реальный flow native crash-а — Google Breakpad: handler ставится через `tracer_install_crash_handler`, при SIGSEGV/SIGABRT пишется minidump (`Minidump.installMinidumpWriterImpl`), потом Java-код через WorkManager заливает его как `sample` на `sdk-api.apptracer.ru`.

## 6. Что управляется сервером

PmsKey, относящиеся к этому стеку (см. `findings/raw/pms_keys.txt`):

```
tracer-non-fatal-crashed-enabled    # включить отправку non-fatal exceptions
perf-events                          # подписки на типы perf-событий
perf-registrar-config                # конфиг регистратора perf
anr-config                           # конфиг ANR (что считать ANR-ом)
debug-profile-info                   # доп. info при отладке
log-full / log-sensitive / log-messages-meta / log-chat-meta / log-violations
                                     # уровни логирования (включая «sensitive»)
user-debug-report                    # включает отправку пользовательских отчётов
analytics-enabled / mytracker-enabled # ON/OFF аналитики
error-stat-limit                     # лимит на отправку error-стат
```

То есть сервер может в рантайме включить:
- non-fatal стек-репорты (любые исключения, не приводящие к крэшу);
- расширенный perf (подписки на типы событий);
- логирование sensitive данных и метаданных сообщений / чатов;
- detailed user-debug отчёты.

Все эти данные уходят на тот же `sdk-api.apptracer.ru` через `sample`-uploader.

## 7. Скептический разбор

- Apptracer — это собственный SDK Одноклассников (`apptracer.ru` принадлежит VK / OK Group). Это не сторонняя слежка — это «своя» внутренняя.
- Endpoint захардкожен. Подмена через `tracer_set_api_endpoint` в текущей сборке не используется. Возможность есть, эксплуатация через сервер не подтверждена.
- `tracer_supports_arbitrary_dump` и `tracer_set_ssl_cainfo` тревожны по имени, но без анализа в Ghidra (callgraph внутри `.so`) — это hypothesis. Нужно отдельно вычитать `tracer_init`.
- Шринк heap-дампа (`ShrinkDumpWorker`) перед отправкой потенциально вырезает чувствительные строки, но это зависит от `x58` и `ga.f(...)`. Что именно вырезается — не проверял. По умолчанию hprof содержит **полные значения всех строк в Java-heap**, в том числе пароли в EditText-ах, токены, тексты сообщений в DraftStore. Pessimistic assumption: до проверки `x58` — heap-dump содержит чувствительные строки.

## 8. Что осталось проверить

1. Что именно вырезает `x58` при шринке heap-дампа.
2. Внутренности `tracer_init` (Ghidra): откуда читается `api_endpoint` и `ssl_cainfo`, есть ли путь «прочитать из конфига и заменить».
3. Внутренности `tracer_supports_arbitrary_dump`: что считается arbitrary dump.
4. Условия триггера `HeapDumpInitializer.b()` через PmsKey `perf-events` — какая именно нагрузка приводит к heap-snapshot-у.
