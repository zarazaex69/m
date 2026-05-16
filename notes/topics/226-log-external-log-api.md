---
tags: [api, telemetry, onelog, log-upload, gzip]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/mcc.java
related:
  - "[[202-onelog-upload-mechanism]]"
  - "[[04-telemetry-endpoints]]"
---

# log.externalLog — API метод загрузки OneLog

`log.externalLog` — API-метод для загрузки OneLog телеметрии на сервер.

## Параметры запроса

```json
{
  "collector": "<collector_name>",
  "data": {
    "application": "<app:version:build>",
    "platform": "<android:phone|tablet:version>",
    "items": [...]
  }
}
```

| Поле | Что |
|---|---|
| `collector` | имя коллектора (endpoint) |
| `application` | `ru.oneme.app:26153:26.15.3` |
| `platform` | `android:phone:<Android version>` |
| `items` | массив событий |

## Технические детали

- **gzip** — запрос сжимается (`shouldGzip=true`)
- **POST** — метод POST (`shouldPost=true`)
- **priority=2** — приоритет запроса
- **scope=c** — область видимости

## Что важно

1. **gzip-сжатие** — телеметрия сжимается перед отправкой. Это затрудняет перехват без MITM.

2. **`application`** и **`platform`** — в каждом запросе. Подтверждение [[202-onelog-upload-mechanism]].

3. Используется как базовый класс для `CallAnalyticsApiRequest` (calls SDK).

## Сводка

`log.externalLog`: POST+gzip, collector/application/platform/items. Базовый класс для всей OneLog телеметрии.
