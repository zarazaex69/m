---
tags: [settings, onelog, telemetry, surveillance, biometry]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/f6e.java
  - work/jadx_base/sources/defpackage/dv.java
  - work/jadx_base/sources/defpackage/uu.java
  - work/jadx_base/sources/one/me/appearancesettings/multitheme/AppearanceSettingsMultiThemeScreen.java
  - work/jadx_base/sources/defpackage/as3.java
  - work/jadx_base/sources/defpackage/n86.java
related:
  - "[[31-onelog-event-categories]]"
  - "[[37-webapp-privilege-configuration]]"
---

# SETTINGS и POWER_SAVING OneLog — телеметрия настроек

## SETTINGS операции

| Операция | Атрибуты | Что |
|---|---|---|
| `MINIAPP_BIOMETRY` | `paramValue` (0/1), `webappId` | включение/выключение биометрии для мини-апки |
| `THEME` | (тема) | смена темы оформления |
| `BACKGROUND` | (фон) | смена фона |
| `TEXT_SIZE` | (размер) | смена размера текста |

## POWER_SAVING операции

| Операция | Что |
|---|---|
| `show_shade` | показан экран энергосбережения |
| `click_shade_button` | нажата кнопка на экране энергосбережения |
| `close_shade` | закрыт экран энергосбережения |

## Что важно

1. **`MINIAPP_BIOMETRY`** — сервер знает, для каких мини-апок (`webappId`) пользователь включил биометрическую аутентификацию. Это означает, что сервер знает, какие мини-апки пользователь считает достаточно важными для биометрии.

2. **`THEME`/`BACKGROUND`/`TEXT_SIZE`** — сервер знает настройки внешнего вида приложения. Это поведенческий профиль пользователя.

3. **`POWER_SAVING`** — сервер знает, взаимодействовал ли пользователь с экраном энергосбережения. Это важно для понимания, включён ли у пользователя режим экономии батареи (что влияет на фоновую работу MAX).

## webapp_biometry в Room DB

В `OneMeRoomDatabase` есть таблица `webapp_biometry` — локальное хранение биометрических настроек для мини-апок. Сервер через `MINIAPP_BIOMETRY` OneLog-событие получает изменения этих настроек.

## Сводка

`SETTINGS` OneLog: биометрия мини-апок (с webappId), тема, фон, размер текста. `POWER_SAVING`: взаимодействие с экраном энергосбережения. Сервер получает полный профиль настроек пользователя.
