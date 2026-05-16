---
tags: [telemetry, apptracer, battery, performance, upload, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/nei.java
  - work/jadx_base/sources/defpackage/xl2.java
related:
  - "[[17-apptracer-uplink]]"
  - "[[316-battery-snapshot]]"
  - "[[320-health-stats-api]]"
---

# Apptracer perf/upload — загрузка данных производительности

`nei.java` — загрузка данных производительности (battery/CPU/network) на Apptracer.

## Endpoint

```
POST https://sdk-api.apptracer.ru/api/perf/upload?crashToken=<token>
Content-Type: application/json; charset=utf-8

{
  "samples": [...]
}
```

## Дополнительно (xl2.java)

| Поле | Что |
|---|---|
| `tracer_sample_file_path` | путь к файлу сэмпла |
| `tracer_sample_file_size` | размер файла |
| `tracer_sample_file_name` | имя файла |

## Что важно

1. **`https://sdk-api.apptracer.ru/api/perf/upload`** — подтверждение [[17-apptracer-uplink]]. Данные производительности (BatterySnapshot) загружаются на Apptracer.

2. **`samples`** — массив сэмплов. Это батарейные снимки (BatterySnapshot).

3. **`crashToken`** — токен для аутентификации. Тот же токен, что и для crash-репортов.

4. Ответ парсится как JSON и логируется через `PERFORMANCE_METRICS` CritLog.

## Сводка

`https://sdk-api.apptracer.ru/api/perf/upload?crashToken=<token>` — загрузка BatterySnapshot (CPU/battery/network) на Apptracer.
