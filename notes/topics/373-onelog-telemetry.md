---
tags: [onelog, telemetry, logging, uploader, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/onelog/OneLog.java
  - work/jadx_base/sources/ru/ok/android/onelog/OneLogItem.java
  - work/jadx_base/sources/ru/ok/android/onelog/OneLogApiRequest.java
  - work/jadx_base/sources/ru/ok/android/onelog/Uploader.java
  - work/jadx_base/sources/ru/ok/android/onelog/NetworkClass.java
related:
  - "[[04-telemetry-endpoints]]"
  - "[[17-apptracer-uplink]]"
---

# OneLog — телеметрия OK.ru

`OneLog` — телеметрия OK.ru. Отправляет события на `log.externalLog` endpoint.

## Структура OneLogItem

| Поле | Что |
|---|---|
| `collector` | коллектор (имя сервиса) |
| `operation` | операция |
| `uid` | user ID |
| `network` | тип сети (excellent/good/moderate/poor) |
| `type` | тип (TYPE_SUCCESS=1/TYPE_NOP=-1/TYPE_DURATION_SUCCESS=0) |
| `count` | счётчик |
| `datum[i]` | данные |
| `group[i]` | группы |
| `custom[key,value]` | произвольные пары ключ-значение |

## OneLogApiRequest

| Поле | Что |
|---|---|
| `collector` | коллектор |
| `application` | `packageName:versionCode:versionName` |
| `platform` | `android:phone/tablet:OS_version` |
| `items` | список OneLogItem |

URI: `log.externalLog` (через `tp.b()`).

## Uploader

- `getApplicationParam()` → `packageName:versionCode:versionName`
- `getPlatformParam()` → `android:phone/tablet:OS_version`
- `shouldGzip()` → true (данные сжимаются)

## NetworkClass

| Значение | Что |
|---|---|
| `excellent` | отличная сеть |
| `good` | хорошая |
| `moderate` | умеренная |
| `poor` | плохая |

## Что важно

1. **`log.externalLog`** — endpoint для отправки логов. Это внешний лог-сервис OK.ru.

2. **`application`** — `packageName:versionCode:versionName` — идентификатор приложения.

3. **`platform`** — `android:phone/tablet:OS_version` — идентификатор платформы.

4. **`uid`** — user ID в каждом событии.

5. **`shouldGzip()`** — данные сжимаются перед отправкой.

## Сводка

`OneLog.log(item)` → `log.externalLog`. Поля: collector/operation/uid/network/type/count/datum/group/custom. Application: `packageName:versionCode:versionName`. Platform: `android:phone/tablet:OS_version`.
