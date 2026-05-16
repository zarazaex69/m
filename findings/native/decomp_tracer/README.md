# Tracer-native — декомпиляция (rizin pdg / Ghidra-decompiler)

Декомпилировано через `rizin -A -c "pdg @ sym.<name>"` (rzghidra plugin).

Источник: `work/apktool_arm64/lib/arm64-v8a/libtracernative.so` (799 KB, NDK r27).

## Архитектура

Все экспортированные C-функции (`tracer_*`) — это **тонкие wrappers поверх vtable одного глобального backend-объекта**:

```
*(int64_t *)0xc57b0   # глобальный синглтон tracer
  + 0x90              # lock guard (mutex)
  + 0x128             # vtable backend-а
  + 0x18 / 0x30 / 0x48  # offsets вирт. методов
```

Все wrappers:

1. Берут глобал.
2. Лочат mutex (`fcn.00073d24`).
3. Конструируют std::string из аргументов (SSO-style: длина <= 22 — inline).
4. Вызывают виртуальный метод backend-а через vtable.
5. Релизят SSO-струткуры.
6. Анлокают.

То есть сам класс `tracer::backend` (это абстракция; реальные реализации — `breakpad_crashclient`, `crashpad`) — определяет, **что эти setter-ы делают**. В таблице символов:

```
N6tracer11crashclientE
N6tracer6detail11jni_backendE
N6tracer6detail20breakpad_crashclientE
N6tracer7backendE
```

То есть в этой `.so` есть как минимум:

- `tracer::backend` (абстрактная база);
- `tracer::detail::jni_backend` (используется на android — пишет минидамп в файл, передаёт в Java upload);
- `tracer::detail::breakpad_crashclient` (используется при breakpad-стиле краш-репортинга);
- `tracer::crashclient` (унифицированная обёртка).

## tracer_init(api_key, environment)

```c
void tracer_init(arg1, arg2)
{
    backend = global;
    lock(global+0x90);

    if (backend->vtable_at_0x90 == 0) {
        unlock();
    } else {
        len1 = strlen(arg1);
        if (len1 > 0xfffffff) {           // ~256MB sanity check на длину строки
            // фоллбек, какие-то extra маршруты
            ...
        } else {
            // SSO для arg1
            std::string s1(arg1);
            // strlen для arg2
            std::string s2(arg2);
            // вызов виртуального метода backend->setKey(s1, s2)
            (*(backend->vtable_at_0x90))(backend, &s1, &s2);
            ...
        }
    }
    unlock();
}
```

Параметры — `(const char* key, const char* value)`. В коде Java `NativeBridge.setKey(key, value)` — это и есть тот же вызов через вирт. метод. То есть `tracer_init` сам по себе не делает init «из конфига», а **вызывает setKey по двум строкам**. Реальная инициализация (создание backend-а, открытие сетевого канала, путь к dump-кэшу и т. п.) — где-то ещё, в `JNI_OnLoad` или конструкторе backend-а.

## tracer_set_api_endpoint(url)

То же что `tracer_init` по структуре — конструирует std::string из `url`, вызывает виртуальный метод (offset 0x40 предположительно) у backend-а. То есть это тонкий setter «обнови URL у backend-а».

В Java коде эта функция **не вызывается**. То есть подменить endpoint в рантайме можно только через нативный вызов из чужого кода — которого в текущей сборке нет. Endpoint задаётся в самом backend-е (предположительно в его конструкторе) или через захардкоженный URL `https://sdk-api.apptracer.ru` в Java.

## tracer_set_ssl_cainfo(path, content?)

```c
void tracer_set_ssl_cainfo(arg1, arg2)
{
    backend = global;
    lock();
    if (backend->vtable_at_0x90 != null) {
        std::string s1(arg1);
        std::string s2(arg2);
        (*(backend->vtable_at_0x48))(backend, &s1, &s2);
    }
    unlock();
}
```

Передаёт две строки (вероятно `cainfo_path` и/или `cainfo_blob`) в backend->setSslCaInfo. По именованию — задаёт собственный CA bundle для curl-сетевого клиента. **В Java коде вызов этой функции я не нашёл.** Установка через JNI-bridge не выставлена.

Это значит: возможность задать собственный CA для tracer-стека есть, **но в текущей сборке клиента она не используется**. Сценарий «принудить tracer-стек доверять чужому CA» — отсутствует в JNI-обвязке. Без переходной dlopen-загрузки сторонней `.so` или patching — функция мёртвая.

## tracer_supports_arbitrary_dump()

```c
uint32_t tracer_supports_arbitrary_dump(void)
{
    backend = global;
    lock();
    if (backend->vtable_at_0x128 == NULL)
        result = 0;
    else
        result = (*(backend->vtable_at_0x128 + 0x18))();
    unlock();
    return result & 1;
}
```

То есть это **getter capabilities**: «спросить у backend-а, поддерживает ли он arbitrary dump». Возвращает 0 или 1. **Не делает дамп**, а только говорит, можно ли. Такой функции достаточно, чтобы клиент (Java через NativeBridge или другой нативный код) мог принять решение «снимать ли heap dump».

Имя функции тревожное, реальная семантика — нет. Это снимает один из подозрительных пунктов из [[17-apptracer-uplink]].

## tracer_install_crash_handler()

```c
void tracer_install_crash_handler(void)
{
    backend = global;
    lock();
    if (backend->vtable_at_0x90 != null) {
        (*(backend->vtable_at_0x90 + 0x30))();   // backend->install()
    }
    unlock();
}
```

Тривиальный wrapper. Реальная установка handler-а — внутри backend-а (breakpad / crashpad).

## tracer_aurora_collect_minidump_from_cachedir(...)

Существует. Семантика по имени: на Авроре (российская fork-Sailfish ОС) минидампы сначала пишутся в cache-директорию, а потом отдельно собираются и отправляются. На Android используется альтернативный путь — `Java_ru_ok_tracer_minidump_Minidump_installMinidumpWriterImpl(path)` непосредственно ставит handler через breakpad.

Это просто отдельная ветка для Авроры. На Android-сборке MAX 26.15.3 эта функция, видимо, не используется (нужно отдельно проверить вызов из Java/нативной стороны). Но **факт того, что один и тот же tracer-движок используется для android- и Аврора-сборок MAX, подтверждается.**

## JNI_OnLoad

```c
JNI_OnLoad(JavaVM*, void*)
{
    // регистрирует Java-нативные методы:
    //   Java_ru_ok_tracer_minidump_Minidump_installMinidumpWriterImpl
    //   Java_ru_ok_tracer_minidump_Minidump_uninstallMinidumpWriterImpl
    //   Java_ru_ok_tracer_nativebridge_NativeBridgeInstaller_nativeInstallBridge
    return JNI_VERSION_1_x;
}
```

Java-сторона видит только:

- `Minidump.installMinidumpWriterImpl(String dumpDirPath)` — ставит writer в каталог `dumpDirPath`.
- `Minidump.uninstallMinidumpWriterImpl()` — снимает.
- `NativeBridgeInstaller.nativeInstallBridge(NativeBridge bridge)` — передаёт Java-объект на нативную сторону, чтобы native код мог вызывать `bridge.setKey(...)` и `bridge.log(...)` (в Java-сторону). Это «обратный мост»: native пишет тэги/лог в Java.

Что **не** видно в JNI:

- ни один из `tracer_set_api_endpoint`, `tracer_set_ssl_cainfo`, `tracer_set_userid`, `tracer_init`, `tracer_supports_arbitrary_dump`, `tracer_disable_upload` **не имеет JNI-stub-а**.

То есть Java-сторона трогает только два пути в native: установить minidump-writer и зарегистрировать «обратный мост» в Java-side. Все управляющие setter-ы — внутренние, и вызываются изнутри `.so` (в конструкторе backend-а) или через `dlopen`+`dlsym` из третьей библиотеки.

В текущем APK такого dlopen-вызова `tracer_set_*` функций я не нашёл (в `libEnhancementLibShared.so`, `libffmpg.so`, `libjingle_peerconnection_so.so` ссылок нет).

## Скептический итог

Тревожные по имени функции (`tracer_supports_arbitrary_dump`, `tracer_set_ssl_cainfo`, `tracer_set_api_endpoint`) — публичный C API tracer-движка, со стандартной семантикой:

- `set_api_endpoint` — задать URL upload-сервера;
- `set_ssl_cainfo` — задать CA bundle;
- `supports_arbitrary_dump` — capability check.

В текущей Android-сборке MAX **ни одна из них не вызывается через JNI** (нет ни Java-vrapper, ни нативной экспортированной функции, которая бы их использовала из третьей `.so`). Это значит: **runtime-подмена endpoint или CA в этой сборке невозможна без кросс-`.so`-вызовов, которых я не нашёл.**

Endpoint захардкожен в Java на `https://sdk-api.apptracer.ru` (см. [[17-apptracer-uplink]]).

CA bundle — system trust store Android (если backend использует system curl/HttpURLConnection) либо встроенный в библиотеку (нужно проверить отдельно: в `.rodata` `libtracernative.so` есть какие-то PEM blob-ы или нет).
