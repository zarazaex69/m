---
tags: [anr, watchdog, server-control, pms, performance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/m6.java
  - work/jadx_base/sources/defpackage/pn.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[03-pms-server-flags]]"
  - "[[61-leakcanary-gost-debug-flags]]"
---

# anr-config и watchdog-config — серверный контроль ANR-детектора

## anr-config (#85)

`PmsKey.f5anrconfig` — JSON-конфиг ANR-детектора. Структура:

```json
{
  "enabled": true/false,
  "timeout": {
    "low": <ms>,
    "avg": <ms>,
    "high": <ms>
  }
}
```

Три уровня таймаута: `low`, `avg`, `high` — соответствуют классам производительности устройства (low/average/high из [[89-video-transcoding-config]]).

`AnrConfig(timeout=<ms>)` — итоговый конфиг с одним таймаутом, выбранным по классу устройства.

## Что важно

1. **Сервер контролирует ANR-таймаут** — через сколько миллисекунд зависание считается ANR. При низком таймауте — больше ANR-репортов. При высоком — меньше.

2. **`enabled`** — сервер может полностью отключить ANR-детектор. Это означает, что зависания приложения не будут репортиться.

3. **Три уровня по классу устройства** — сервер адаптирует таймаут под производительность устройства. На слабых устройствах (`low`) — более высокий таймаут.

## watchdog-config (#305)

`PmsKey.f305watchdogconfig` — JSON-конфиг watchdog-а. Watchdog следит за зависаниями основного потока. Конфиг задаётся сервером.

## Сводка

`anr-config` — server-pushed JSON с `enabled` и `timeout.low/avg/high`. Сервер контролирует ANR-детектор: включён ли, и при каком таймауте срабатывает. `watchdog-config` — аналогичный конфиг для watchdog-а.
