---
tags: [power-saving, telemetry, surveillance, battery-optimization, notifications]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/n86.java
  - work/jadx_base/sources/defpackage/b5c.java
related:
  - "[[472-log-controller-event-types]]"
  - "[[468-startup-report]]"
---

# POWER_SAVING events — аналитика энергосбережения

## POWER_SAVING.show_shade

```json
{
  "reason": "settings"
}
```

Логируется при показе шторки энергосбережения из настроек.

## POWER_SAVING.click_shade_button

Логируется при клике кнопки в шторке энергосбережения. Параметры: null.

## POWER_SAVING.close_shade

Логируется при закрытии шторки энергосбережения. Параметры: null.

## Что важно

1. Шторка энергосбережения — это UI для запроса разрешения `isIgnoringBatteryOptimizations`.

2. Все взаимодействия с шторкой логируются.

3. Связано с [[468-startup-report]] — `isIgnoringBatteryOptimizations` проверяется при запуске.

## Сводка

`POWER_SAVING.show_shade {reason: settings}`. `POWER_SAVING.click_shade_button`. `POWER_SAVING.close_shade`.
