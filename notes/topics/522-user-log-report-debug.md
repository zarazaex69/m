---
tags: [rtd, user-log-report, debug, surveillance, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/rtd.java
  - work/jadx_base/sources/defpackage/ozb.java
related:
  - "[[520-rtd-additional-fields]]"
  - "[[03-pms-server-flags]]"
---

# userLogReportChatId + debug-mode — отладочные поля RTD

## userLogReportChatId

`getUserLogReportChatId()` — ID чата для отправки отчётов пользователя.

Сервер задаёт ID чата, в который пользователь может отправить отчёт об ошибке.

## debug-mode

`debug-mode` — режим отладки. Устанавливается через `NotifConfigLogic.b()`:

```java
((pob) rtdVar.h.getValue()).setValue(Integer.valueOf(map.get("debug-mode")));
```

## user-debug-report

`user-debug-report` — отчёт отладки пользователя. Устанавливается через `NotifConfigLogic.b()`:

```java
((pob) rtdVar.i.getValue()).setValue(Long.valueOf(map.get("user-debug-report")));
```

## Что важно

1. **`userLogReportChatId`** — сервер задаёт конкретный чат для отчётов. Это означает, что отчёты пользователей идут в конкретный чат на сервере.

2. **`debug-mode`** — режим отладки управляется сервером.

3. **`user-debug-report`** — отчёт отладки управляется сервером.

## Сводка

`userLogReportChatId` — ID чата для отчётов. `debug-mode` — режим отладки. `user-debug-report` — отчёт отладки. Все управляются сервером.
