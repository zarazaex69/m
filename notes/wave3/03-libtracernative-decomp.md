---
tags: [native, tracer, breakpad, crash-reporting, ptrace, prctl, syscall, wave3]
status: complete
sources: [libtracernative.so (arm64-v8a, 799KB), rizin+rzghidra decompilation]
related: [[17-apptracer-uplink]]
---

# libtracernative.so — полная декомпиляция и анализ syscall-уровня

## TL;DR

`libtracernative.so` — это **стандартный crash-reporting SDK** (OK.ru Tracer) на базе Google Breakpad с Boost.Log. Содержит:
- 24 экспортированных функции (тонкие wrappers над vtable backend-объекта)
- 6 прямых `ptrace` syscall (svc 0) — стандартный `LinuxPtraceDumper` для minidump
- 2 `prctl` syscall: `PR_SET_DUMPABLE(1)` + `PR_SET_PTRACER(child_pid)`
- Чтение `/proc/pid/{maps,auxv,cmdline,environ,status,task}` — стандартный breakpad
- Нет: RWX mmap, нет socket/connect, нет dlopen, нет скрытых URL/доменов, нет шифрования/обфускации

**Вердикт: ничего нового/подозрительного сверх уже задокументированного в topic 17. Библиотека — чистый crash reporter без скрытой функциональности.**

## Архитектура

```
┌─────────────────────────────────────────────────────┐
│  Java: ru.ok.tracer.minidump.Minidump               │
│  Java: ru.ok.tracer.nativebridge.NativeBridge       │
└──────────────────────┬──────────────────────────────┘
                       │ JNI (3 метода)
┌──────────────────────▼──────────────────────────────┐
│  libtracernative.so                                  │
│  ┌─────────────────────────────────────────────┐    │
│  │ Global singleton @ 0xc57b0                   │    │
│  │   +0x90: backend* (vtable 13 методов)        │    │
│  │   +0x128: crashclient* (vtable 4 метода)     │    │
│  └─────────────────────────────────────────────┘    │
│  Implementations:                                    │
│    tracer::detail::jni_backend (Android)             │
│    tracer::detail::breakpad_crashclient             │
│    tracer::crashclient                              │
│  Dependencies: google_breakpad, boost::log 1.87.0   │
└─────────────────────────────────────────────────────┘
```

## JNI-интерфейс (единственная точка входа из Java)

`JNI_OnLoad` регистрирует ровно 3 нативных метода:

```c
// JNI_OnLoad — возвращает JNI_VERSION_1_6 (0x10006)
JNI_OnLoad(JavaVM* vm) {
    // Сохраняет JavaVM* в глобал 0xc57f8
    // Резолвит:
    //   NativeBridge.setKey(String, String) -> methodID в 0xc5800
    //   NativeBridge.log(String)            -> methodID в 0xc5800+8
    return 0x10006;
}
```

Java видит только:
1. `Minidump.installMinidumpWriterImpl(String dumpDirPath)` — ставит breakpad handler
2. `Minidump.uninstallMinidumpWriterImpl()` — снимает handler
3. `NativeBridgeInstaller.nativeInstallBridge(NativeBridge obj)` — регистрирует Java-объект для обратных вызовов native→Java

**Ни один из `tracer_set_*` экспортов НЕ имеет JNI-обёртки.** Они доступны только через `dlsym` из другой .so — чего в текущем APK не происходит.

## Полная карта экспортов

| Экспорт | Размер | vtable offset | Семантика |
|---------|--------|---------------|-----------|
| `tracer_init(key, env)` | 472B | +0x00 | Передаёт key+environment в backend |
| `tracer_enable_logging_to_console()` | 16B | — | Пустая (nop) |
| `tracer_set_platform_info(5 args)` | 1008B | +0x08 | Передаёт 5 строк platform info |
| `tracer_set_platform_info2(2 args)` | 628B | +0x08 | Альтернативный setter |
| `tracer_set_key(key, value)` | 572B | +0x08 | Generic key-value setter |
| `tracer_set_userid(id)` | 368B | +0x18 | Устанавливает user ID для отчётов |
| `tracer_log(msg)` | 368B | +0x10 | Пишет лог-сообщение |
| `tracer_install_crash_handler()` | 164B | +0x30 | Устанавливает signal handler |
| `tracer_disable_upload()` | 168B | +0x38 | Отключает upload (flag=1) |
| `tracer_keep_processed_crashes()` | 168B | +0x40 | Не удалять обработанные крэши |
| `tracer_set_ssl_cainfo(path, blob)` | 340B | +0x48 | Задаёт CA bundle (НЕ вызывается) |
| `tracer_set_api_endpoint(url)` | 368B | +0x50 | Задаёт URL upload-сервера (НЕ вызывается) |
| `tracer_aurora_collect_minidump(dir, ?)` | 340B | +0x58 | Сбор дампов на Авроре |
| `tracer_report_nonfatal_from_here()` | 164B | +0x60 | Отправка non-fatal отчёта |
| `tracer_supports_arbitrary_dump()` | 188B | vtable2+0x18 | Capability check (returns 0/1) |
| `tracer_hardfix_crashclient()` | 172B | vtable2+0x20 | Хардфикс crashclient |
| `tracer_crashpad_set_handler_path(path)` | 252B | vtable2+0x28 | Путь к crashpad handler |
| `tracer_upload()` | 164B | +0x28 | Синхронный upload |
| `tracer_upload_async()` | 336B | +0x28 | Асинхронный upload (pthread_create) |
| `tracer_upload_crashes()` | 164B | +0x28 | Upload крэшей |

## Полный список syscall (svc 0) — 101 вызов

| Syscall | NR | Кол-во | Контекст |
|---------|-----|--------|----------|
| `openat` | 56 | 10 | Открытие /proc/pid/* файлов |
| `close` | 57 | 23 | Закрытие fd |
| `pipe2` | 59 | 2 | IPC pipe для child process |
| `getdents64` | 61 | 1 | Перечисление /proc/pid/task/ |
| `lseek` | 62 | 7 | Навигация по файлам |
| `read` | 63 | 7 | Чтение /proc файлов |
| `write` | 64 | 7 | Запись minidump |
| `readlinkat` | 78 | 1 | Чтение симлинков |
| `fstatat` | 79 | 2 | Stat файлов |
| `fstat` | 80 | 1 | Stat fd |
| `exit` | 93 | 1 | Выход child process |
| **`ptrace`** | **117** | **6** | **ATTACH/DETACH/PEEKDATA/GETREGSET** |
| `tgkill` | 131 | 1 | Отправка сигнала потоку |
| `sigaltstack` | 132 | 5 | Установка alt signal stack |
| `rt_sigaction` | 134 | 4 | Установка signal handlers |
| **`prctl`** | **167** | **2** | **PR_SET_DUMPABLE(1) + PR_SET_PTRACER(pid)** |
| `getpid` | 172 | 1 | Получение PID |
| `sendmsg` | 211 | 1 | IPC: отправка crash data parent-у |
| `munmap` | 215 | 7 | Освобождение памяти |
| `clone` | 220 | 1 | Создание child для dump |
| `mmap` | 222 | 5 | Выделение памяти (RW, не RWX!) |
| `wait4` | 260 | 2 | Ожидание child process |

### Отсутствуют (что важно):
- ❌ `mprotect` (226) — нет смены прав на EXEC
- ❌ `socket`/`connect`/`bind` — нет сетевых вызовов из native
- ❌ `execve` — нет запуска процессов
- ❌ `ioctl` — нет device control
- ❌ `seccomp` — нет sandbox manipulation

## Детали ptrace

Все 6 ptrace вызовов — в функциях `fcn.0006bd88`, `fcn.0006be70`, `fcn.0006c0e4` (LinuxPtraceDumper):

```asm
; PTRACE_PEEKDATA (w0=2) — чтение памяти потока
mov   w0, 2
mov   x1, x22          ; tid
add   x2, x20, x25    ; addr
mov   x3, sp           ; data buffer
mov   x8, 0x75
svc   0

; PTRACE_GETREGSET (w0=0x4204) — чтение регистров
mov   w0, 0x4204
mov   x1, x20          ; tid
mov   w2, 1            ; NT_PRSTATUS
add   x3, sp, 0x18    ; iovec
mov   x8, 0x75
svc   0

; PTRACE_ATTACH (w0=0x10) — присоединение к потоку
mov   w0, 0x10
mov   x1, x9           ; tid (из /proc/pid/task/)
mov   x2, xzr
mov   x3, xzr
mov   x8, 0x75
svc   0

; PTRACE_DETACH (w0=0x11) — отсоединение
mov   w0, 0x11
mov   x1, x9           ; tid
mov   x2, xzr
mov   x3, xzr
mov   x8, 0x75
svc   0
```

**Контекст:** Вызываются ТОЛЬКО из crash handler path (после SIGSEGV/SIGABRT). Child process (clone) аттачится к потокам parent-а, читает их регистры и стек для minidump. Стандартный паттерн google-breakpad.

## Детали prctl

```asm
; prctl(PR_SET_DUMPABLE, 1) — разрешить core dump
mov   w0, 4            ; PR_SET_DUMPABLE
mov   w1, 1            ; enable
mov   x2, xzr
mov   x3, xzr
mov   x4, xzr
mov   x8, 0xa7
svc   0

; prctl(PR_SET_PTRACER, child_pid) — разрешить child-у ptrace parent
mov   w0, 0x6d61       ; 'am'
movk  w0, 0x5961, lsl 16  ; -> 0x59616d61 = PR_SET_PTRACER
mov   x1, x21          ; child_pid (результат clone)
mov   x2, xzr
mov   x3, xzr
mov   x4, xzr
mov   x8, 0xa7
svc   0
```

**Контекст:** `PR_SET_DUMPABLE(1)` вызывается условно (только если `[x21+8] > 0`). `PR_SET_PTRACER` вызывается после `clone()` чтобы child мог аттачиться к parent-у для сбора minidump. Стандартный breakpad паттерн для Android с Yama LSM.

## Чтение /proc

Breakpad читает следующие файлы (подтверждено xref-ами к строкам):
- `/proc/<pid>/maps` — карта памяти (для stack trace symbolication)
- `/proc/<pid>/auxv` — auxiliary vector (ELF info)
- `/proc/<pid>/cmdline` — имя процесса
- `/proc/<pid>/environ` — переменные окружения
- `/proc/<pid>/status` — статус процесса (uid, gid, threads)
- `/proc/<pid>/task/` — перечисление потоков (getdents64)
- `/proc/cpuinfo` — информация о CPU

Также:
- `/sys/devices/system/cpu/possible` — количество CPU
- `/sys/devices/system/cpu/present` — активные CPU
- `/dev/urandom` — генерация случайных данных

## dlsym

Единственный вызов `dlsym`:
```c
// Резолвит android_log_buf_write для расширенного логирования
dlsym(RTLD_DEFAULT, "android_log_buf_write");
```
Стандартный паттерн — функция доступна не на всех версиях Android.

## __system_property_get

Единственное использование:
```c
// Читает "ro.arch", сравнивает с "exynos9810"
// Workaround для бага Samsung Exynos в breakpad
__system_property_get("ro.arch", buf);
strncmp(buf, "exynos9810", 10);
```

## dl_iterate_phdr

Используется в C++ exception unwinding (libunwind) — стандартная функция для перечисления загруженных .so при раскрутке стека.

## mmap — все вызовы RW (не RWX)

Все 5 mmap вызовов используют:
- `w2 = 3` (PROT_READ | PROT_WRITE) — **без PROT_EXEC**
- `w3 = 0x22` (MAP_PRIVATE | MAP_ANONYMOUS)
- `x4 = -1` (no fd)

Нет JIT-компиляции, нет self-modifying code, нет code injection primitives.

## Аврора (Sailfish OS)

Наличие `tracer_aurora_collect_minidump_from_cachedir` подтверждает: один и тот же tracer SDK используется для Android и Аврора-сборок MAX. На Авроре minidump пишется в cache, потом собирается отдельно.

## Что НЕ найдено (negative findings)

1. **Нет скрытых URL/доменов** — ни одного URL в .rodata. Endpoint задаётся из Java (`sdk-api.apptracer.ru`).
2. **Нет embedded CA certificates** — только строка `-----BEGIN BREAKPAD MICRODUMP-----` (формат дампа).
3. **Нет сетевых syscall** — upload идёт через Java (NativeBridge callback).
4. **Нет anti-debug/anti-tamper** — ptrace используется только для crash dump, не для детекции отладчика.
5. **Нет hooking primitives** — нет mprotect, нет function trampolines, нет inline hooks.
6. **Нет обфускации** — все строки в открытом виде, символы не стрипнуты.
7. **Нет скрытых команд** — `DUMP_REQUESTED` это внутренний флаг breakpad (не внешний trigger).
8. **Нет чтения /proc/self/mem** — только стандартный breakpad набор.
9. **Нет fork-bomb/DoS** — один clone для dump child, с wait4.
10. **Нет записи в /data/** — только чтение /proc и запись minidump в указанный Java каталог.

## Сравнение версий

`libtracernative.so` **не изменилась** между 26.15.3 и 26.16.0 (отсутствует в `native_diff/`). Diff содержит только `libEnhancementLibShared.so` и `libjingle.so`.

## Итоговая оценка

| Критерий | Результат |
|----------|-----------|
| Скрытая функциональность | ❌ Не обнаружена |
| Backdoor primitives | ❌ Отсутствуют |
| Arbitrary code execution | ❌ Нет RWX, нет execve |
| Network exfiltration | ❌ Нет socket/connect в native |
| Anti-analysis | ❌ Нет anti-debug, нет obfuscation |
| Scope creep | ⚠️ `tracer_set_api_endpoint` + `tracer_set_ssl_cainfo` экспортированы но не вызываются — потенциал для будущего злоупотребления через dlsym из другой .so |

**Библиотека — стандартный crash reporter (OK.ru Tracer SDK поверх Google Breakpad + Boost.Log 1.87.0). Все syscall-паттерны соответствуют открытому коду breakpad. Новых находок сверх topic 17 нет.**
