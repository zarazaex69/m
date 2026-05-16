---
tags: [settings, telemetry, surveillance, biometry, theme, background, text-size]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/f6e.java
  - work/jadx_base/sources/defpackage/uu.java
  - work/jadx_base/sources/one/me/appearancesettings/multitheme/AppearanceSettingsMultiThemeScreen.java
related:
  - "[[472-log-controller-event-types]]"
  - "[[420-webapp-storage-biometry]]"
  - "[[426-webapp-biometry-db]]"
---

# SETTINGS events — аналитика настроек

## SETTINGS.MINIAPP_BIOMETRY

```java
ok9.h("SETTINGS", "MINIAPP_BIOMETRY", {
  "paramValue": 0 | 1,  // false | true
  "paramAdditionally": {"webappId": "..."}
})
```

Логируется при изменении биометрии мини-приложения. Содержит `webappId`.

## SETTINGS.BACKGROUND

Логируется при изменении фона чата.

## SETTINGS.THEME

Логируется при изменении темы.

## SETTINGS.TEXT_SIZE

Логируется при изменении размера текста.

## Что важно

1. **`MINIAPP_BIOMETRY`** — каждое изменение биометрии мини-приложения логируется с `webappId`.

2. **`BACKGROUND`/`THEME`/`TEXT_SIZE`** — изменения внешнего вида логируются.

3. Все изменения настроек отправляются на сервер.

## Сводка

`SETTINGS.MINIAPP_BIOMETRY {paramValue, webappId}`. `SETTINGS.BACKGROUND`. `SETTINGS.THEME`. `SETTINGS.TEXT_SIZE`.
