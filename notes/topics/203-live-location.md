---
tags: [location, live-location, push, surveillance, fcm]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/sj9.java
  - work/jadx_base/sources/defpackage/rj9.java
  - work/jadx_base/sources/defpackage/cei.java
  - work/jadx_base/sources/defpackage/rqg.java
related:
  - "[[19-fcm-push-payload]]"
  - "[[08-background-wake]]"
---

# Live Location — геолокация в сообщениях

MAX поддерживает отправку геолокации в чатах, включая «живую» (обновляемую) геолокацию.

## LocationData (sj9)

| Поле | Что |
|---|---|
| `latitude` / `longitude` | координаты |
| `altitude` | высота |
| `accuracy` (`epu`) | точность (м) |
| `bearing` (`hdn`) | направление (heading) |
| `speed` (`spd`) | скорость |

## Сообщение с геолокацией (rj9)

Сериализуется в HashMap с полями:

| Ключ | Что |
|---|---|
| `latitude` / `longitude` | координаты |
| `livePeriod` | **период обновления** (live location) |
| `zoom` | масштаб карты |
| `alt` | высота |
| `epu` | точность |
| `hdn` | направление |
| `spd` | скорость |

`livePeriod > 0` — «живая» геолокация, обновляется в течение заданного периода.

## FCM push `LocationRequest`

FCM push с `type=LocationRequest` (см. [[19-fcm-push-payload]]) — сервер может запросить геолокацию устройства через push. Обработчик в `cei.java`: `onLocationRequestPush` → запускает `TaskLocationRequest`.

`TaskLocationRequest` (`rqg`) — задача получения геолокации:
- Использует `LocationManager.getLastKnownLocation("gps")` и `"network"`
- Требует `ACCESS_FINE_LOCATION`

## Что важно

1. **`LocationRequest` push** — сервер может инициировать получение геолокации через FCM push. Это не требует действий пользователя.

2. **Live location** — `livePeriod` позволяет отслеживать пользователя в реальном времени в течение заданного периода.

3. **Полный набор данных**: координаты + высота + точность + направление + скорость. Это не просто «точка на карте».

## Сводка

Live location: `latitude`/`longitude`/`altitude`/`accuracy`/`bearing`/`speed`/`livePeriod`. FCM push `LocationRequest` — сервер инициирует получение геолокации без действий пользователя.
