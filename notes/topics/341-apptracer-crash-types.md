---
tags: [telemetry, apptracer, crash-types, anr, minidump, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/my4.java
related:
  - "[[340-crash-report-exit-info]]"
  - "[[339-minidump-crash-handler]]"
---

# Apptracer Crash Types — типы краш-репортов

`my4.java` — сохранение и загрузка краш-репортов.

## Типы краш-репортов

| Тип | Код | Что |
|---|---|---|
| `CRASH` | 1 | обычный краш |
| `NON_FATAL` | 2 | нефатальная ошибка |
| `FATAL` | 3 | фатальная ошибка |
| `ERROR` | 4 | ошибка |
| `WARNING` | 5 | предупреждение |
| `NOTICE` | 6 | уведомление |
| `INFO` | 7 | информация |
| `DEBUG` | 8 | отладка |
| `MINIDUMP` | 9 | **minidump** |
| `ANR` | 10 | **ANR** |

## Путь к файлам

```
getCacheDir() / "tracer" / "crashes"
```

## Что важно

1. **10 типов** краш-репортов — от DEBUG до MINIDUMP и ANR.

2. **`MINIDUMP`(9)** — нативный краш (minidump файл).

3. **`ANR`(10)** — Application Not Responding (ANR trace).

4. Файлы сохраняются в `getCacheDir()/tracer/crashes`.

## Сводка

Apptracer crash types: CRASH/NON_FATAL/FATAL/ERROR/WARNING/NOTICE/INFO/DEBUG/MINIDUMP/ANR. Путь: getCacheDir/tracer/crashes.
