---
tags: [native, apptracer, crash, minidump, surveillance]
status: confirmed
sources:
  - findings/native/libtracernative.exports.txt
  - findings/native/libtracernative.strings.txt
related:
  - "[[17-apptracer-uplink]]"
  - "[[09-native-libs]]"
---

# libtracernative.so — экспорты Apptracer

`libtracernative.so` — нативная библиотека Apptracer. 24 экспортируемых символа.

## Экспорты

| Символ | Что |
|---|---|
| `tracer_init` | инициализация |
| `tracer_set_api_endpoint` | **установить API endpoint** |
| `tracer_set_key` | установить ключ |
| `tracer_set_userid` | установить ID пользователя |
| `tracer_set_platform_info` / `tracer_set_platform_info2` | платформенная информация |
| `tracer_set_ssl_cainfo` | SSL CA сертификат |
| `tracer_install_crash_handler` | установить обработчик краша |
| `tracer_disable_upload` | **отключить загрузку** |
| `tracer_keep_processed_crashes` | хранить обработанные крашы |
| `tracer_upload` / `tracer_upload_async` | загрузить данные |
| `tracer_upload_crashes` | загрузить крашы |
| `tracer_log` | логирование |
| `tracer_report_nonfatal_from_here` | нефатальная ошибка |
| `tracer_supports_arbitrary_dump` | поддержка произвольного дампа |
| `tracer_hardfix_crashclient` | хардфикс crash client |
| `tracer_aurora_collect_minidump_from_cachedir` | **сбор minidump из кэша** (Aurora OS) |
| `tracer_crashpad_set_handler_path` | путь к crashpad handler |
| `Java_ru_ok_tracer_minidump_Minidump_installMinidumpWriterImpl` | JNI: установить minidump writer |
| `Java_ru_ok_tracer_minidump_Minidump_uninstallMinidumpWriterImpl` | JNI: удалить minidump writer |
| `Java_ru_ok_tracer_nativebridge_NativeBridgeInstaller_nativeInstallBridge` | JNI: установить native bridge |

## Что важно

1. **`tracer_aurora_collect_minidump_from_cachedir`** — поддержка Aurora OS (российская ОС на базе Sailfish). MAX работает на Aurora.

2. **`tracer_supports_arbitrary_dump`** — поддержка произвольного дампа. Это подтверждение [[17-apptracer-uplink]] — heap dumps.

3. **`tracer_set_api_endpoint`** — endpoint задаётся программно. Может быть изменён сервером.

4. **`tracer_disable_upload`** — загрузку можно отключить. Используется в `UploadConfig.DEFAULT_DISABLE_UPLOAD_IN_CALL=true`.

## Сводка

`libtracernative.so`: 24 экспорта. Ключевые: `tracer_aurora_collect_minidump_from_cachedir` (Aurora OS), `tracer_supports_arbitrary_dump`, `tracer_set_api_endpoint`, `tracer_disable_upload`.
