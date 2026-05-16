---
tags: [webapp, jsbridge, ui-control, closing-behavior, back-button, open-link, brightness]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/mkk.java
  - work/jadx_base/sources/defpackage/hkk.java
  - work/jadx_base/sources/defpackage/dgk.java
  - work/jadx_base/sources/defpackage/fak.java
related:
  - "[[419-webapp-js-events]]"
  - "[[420-webapp-storage-biometry]]"
---

# WebApp UI Control — ClosingBehavior, BackButton, OpenLink, Brightness

## WebAppSetupClosingBehavior

`WebAppSetupClosingBehaviorRequest(needConfirmation: bool)` — мини-приложение управляет поведением при закрытии.

Если `needConfirmation=true` — при закрытии показывается диалог подтверждения.

## WebAppSetupBackButton

`WebAppSetupBackButtonRequest(isVisible: bool)` — мини-приложение управляет видимостью кнопки назад.

## WebAppOpenLink

`WebAppOpenLinkRequest(url)` — мини-приложение открывает URL во внешнем браузере.

## WebAppOpenMaxLink

`WebAppOpenMaxLink(url)` — мини-приложение открывает URL внутри MAX.

## WebAppChangeScreenBrightness

`WebAppChangeScreenBrightness(requestId, maxBrightness)` — мини-приложение изменяет яркость экрана.

## Что важно

1. **`WebAppOpenLink(url)`** — мини-приложение может открыть произвольный URL во внешнем браузере.

2. **`WebAppOpenMaxLink(url)`** — мини-приложение может открыть URL внутри MAX (deeplink).

3. **`WebAppChangeScreenBrightness(maxBrightness)`** — мини-приложение может изменить яркость экрана.

4. **`needConfirmation`** — мини-приложение может запросить подтверждение при закрытии.

## Сводка

`WebAppSetupClosingBehavior(needConfirmation)`. `WebAppSetupBackButton(isVisible)`. `WebAppOpenLink(url)`. `WebAppOpenMaxLink(url)`. `WebAppChangeScreenBrightness(maxBrightness)`.
