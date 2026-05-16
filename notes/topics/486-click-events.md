---
tags: [click-events, telemetry, surveillance, broadcast, video-speed, business]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/af9.java
  - work/jadx_base/sources/one/me/settings/SettingsListScreen.java
  - work/jadx_base/sources/defpackage/okh.java
related:
  - "[[472-log-controller-event-types]]"
---

# CLICK events — аналитика кликов

## CLICK.open_broadcast_button_click

```json
{
  "source_meta": {
    "channel_id": long,
    "hashed_broadcast_link": hash
  }
}
```

Логируется при клике кнопки трансляции. Содержит **хэш ссылки трансляции**.

## CLICK.profile_button_click

```json
{
  "source_meta": {
    "buttonName": "max_for_business"
  }
}
```

Логируется при клике кнопки "MAX для бизнеса" в профиле.

## CLICK.video_speed_change

```json
{
  "source_meta": {
    "speed": float,
    "sourceType": "MENU" | "SWIPE"
  }
}
```

Логируется при изменении скорости видео. Содержит скорость и способ изменения.

## Что важно

1. **`hashed_broadcast_link`** — хэш ссылки трансляции. Сервер знает, какую трансляцию открывает пользователь.

2. **`max_for_business`** — клик кнопки "MAX для бизнеса" логируется.

3. **`video_speed_change`** — изменение скорости видео логируется с типом (меню/свайп).

## Сводка

`CLICK.open_broadcast_button_click {channel_id, hashed_broadcast_link}`. `CLICK.profile_button_click {buttonName: max_for_business}`. `CLICK.video_speed_change {speed, sourceType: MENU|SWIPE}`.
