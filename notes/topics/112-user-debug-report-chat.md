---
tags: [debug, server-control, pms, logging, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ozb.java
  - work/jadx_base/sources/defpackage/rtd.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[03-pms-server-flags]]"
  - "[[05-dev-menu-in-prod]]"
  - "[[46-server-controlled-logging-flags]]"
---

# user-debug-report и userLogReportChatId — серверный debug-репорт

## user-debug-report (#292)

`PmsKey.f292userdebugreport` — long. Из `ozb.java` — парсится из конфига при обновлении PmsKey. Это режим debug-репортирования для конкретного пользователя.

## userLogReportChatId

`rtd.userLogReportChatId` — long. ID чата, в который отправляются debug-репорты пользователя. Это означает: **сервер может задать конкретный чат, в который будут отправляться логи пользователя**.

Это мощный механизм: сервер задаёт `userLogReportChatId = <chat_id>`, и клиент начинает отправлять debug-логи в этот чат. Это может быть чат с поддержкой или специальный диагностический чат.

## debug-mode (#3)

`PmsKey.f92debugmode` — int. Из `ozb.java` — парсится из конфига. Режим отладки. При ненулевом значении — включает дополнительное логирование.

## Что важно

1. **`userLogReportChatId`** — сервер может направить логи пользователя в конкретный чат. Это означает, что сервер может получать логи пользователя через WS-сообщения в чате.

2. **`user-debug-report`** — server-pushed режим debug-репортирования для конкретного пользователя. Сервер может включить расширенное логирование для конкретного пользователя.

3. **`debug-mode`** — server-pushed режим отладки. При включении — дополнительное логирование.

## Сводка

`user-debug-report` + `userLogReportChatId` — сервер может включить debug-репортирование для конкретного пользователя и направить логи в конкретный чат. `debug-mode` — server-pushed режим отладки.
