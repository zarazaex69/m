---
tags: [traffic-stats, network, apptracer, dps, surveillance, telemetry]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/kyk.java
related:
  - "[[04-telemetry-endpoints]]"
  - "[[415-dps-client-zu5]]"
  - "[[362-z8f-dps-metadata]]"
---

# kyk — DPS/Apptracer HTTP клиент с TrafficStats

`kyk` — HTTP клиент для DPS/Apptracer. Использует `TrafficStats` для маркировки трафика.

## Ключевые детали

### TrafficStats

```java
TrafficStats.setThreadStatsTag(url.hashCode());
// HTTP запрос
TrafficStats.clearThreadStatsTag();
```

Маркирует трафик по хэшу URL.

### Декодированные строки (z8f)

| Строка | Значение |
|---|---|
| `9dc741e3...` | `{"snapshots":[` |
| `8334362259...` | `{"id":` |
| `c333826b...` | `User-Agent` |
| `c9920d03...` | `id` |
| `f4234dff...` | `timeoutMs` |
| `fcb6c351...` | `dontReportUntil` |

### Структура запроса

```json
{
  "snapshots": [
    {"id": ..., ...}
  ]
}
```

Параметры: `timeoutMs`, `dontReportUntil`.

## Что важно

1. **`TrafficStats.setThreadStatsTag`** — трафик DPS маркируется по хэшу URL.

2. **`dontReportUntil`** — сервер может задать время, до которого не отправлять отчёты.

3. **`timeoutMs`** — таймаут запроса задаётся сервером.

4. **`User-Agent`** — заголовок User-Agent устанавливается из `this.c.getValue()`.

## Сводка

`kyk`: HTTP клиент DPS с `TrafficStats.setThreadStatsTag`. Структура: `{snapshots: [{id, ...}]}`. Параметры: `timeoutMs`/`dontReportUntil`.
