---
tags: [telemetry, metrics, prefs, contacts]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/tvc.java
related:
  - "[[06-contacts]]"
  - "[[04-telemetry-endpoints]]"
---

# Серверные метрики: что улетает в `stat_prefs`

`defpackage/tvc.java` — отдельный SharedPreferences `"stat_prefs"`, хранилище серверной статистики:

```java
this.f = new bbi(1, "file.open_stats", ...);        // fileOpenStats   (String)
this.g = new bbi(1, "session.opcode_stats", ...);   // opcodeStats     (String)
this.h = new bbi(1, "app.phonebook.size", ...);     // phonebookSize   (Int)
this.i = new bbi(1, "app.anr.detected", FALSE, ...);// anrDetected     (Bool)
this.j = new bbi(1, "app.crash.detected", 0, ...);  // crashDetected   (Int)
                                                    // caughtExceptionCount
                                                    // frescoStats     (FrescoStats)
                                                    // appClockDump    (AppClockDump)
```

То есть в каждой сессии MAX считает и отправляет на сервер:

- **`app.phonebook.size`** — общее количество записей в телефонной книге пользователя. Даже если ни один из этих контактов не пользуется MAX, число — на сервере.
- `session.opcode_stats` — сколько каких WebSocket-сообщений отправлено/получено (по opcode-ам).
- `file.open_stats` — статистика открытий файлов внутри MAX.
- `fresco.stats` — статистика Fresco (декодирование изображений), вместе с типами/размерами.
- `appClockDump` — серверный «дамп часов приложения» (видимо для трекинга смещения времени).
- `anrDetected`, `crashDetected`, `caughtExceptionCount` — был ли ANR/краш/exception в сессии.

## Кратко

> «MAX отдельным `SharedPreferences("stat_prefs")` накапливает и отправляет на сервер набор метрик за сессию: размер телефонной книги пользователя, статистику WebSocket-opcodes (что отправлено/принято и сколько раз), статистику открытий файлов, статистику Fresco (декодирование изображений), факт ANR/краша/исключений. То есть само число записей в адресной книге — отдельная метрика, отправляемая на сервер.»
