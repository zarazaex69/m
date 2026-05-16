---
tags: [log-external, telemetry, surveillance, onelog, collector, application, platform]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/mcc.java
related:
  - "[[373-onelog-telemetry]]"
  - "[[471-log-controller]]"
---

# mcc — log.externalLog API request

`mcc` — базовый класс для запросов к `log.externalLog` endpoint.

## Структура запроса

```json
{
  "collector": "...",
  "application": "...",
  "platform": "...",
  "items": [...]
}
```

## Параметры

| Поле | Что |
|---|---|
| `collector` | коллектор (имя сервиса) |
| `application` | `packageName:versionCode:versionName` |
| `platform` | `android:phone/tablet:OS_version` |
| `items` | список событий |

## Что важно

1. **`log.externalLog`** — endpoint для отправки логов. Это внешний лог-сервис OK.ru.

2. **`shouldGzip() = true`** — данные сжимаются.

3. **`priority = 2`** — приоритет запроса.

4. Это базовый класс для `OneLogApiRequest` и `CallAnalyticsApiRequest`.

## Сводка

`mcc`: `log.externalLog` endpoint. `{collector, application, platform, items}`. Gzip сжатие.
