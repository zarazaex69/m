---
tags: [safe-mode, privacy, content-filter, settings, surveillance, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/settings/privacy/ui/onboarding/SafeModeOnboardingScreen.java
  - work/apktool_base/res/values/strings.xml
related:
  - "[[411-user-settings-full]]"
  - "[[11-state-bots-and-content-policy]]"
  - "[[437-twofa-screens]]"
---

# SafeMode — безопасный режим

`SafeModeOnboardingScreen` — онбординг безопасного режима.

## Что делает SafeMode (из строк)

| Пункт | Заголовок | Описание |
|---|---|---|
| 1 | **Профиль не показывается в поиске** | Вас не смогут найти по номеру телефона |
| 2 | **Звонки только от контактов** | Позвонить вам смогут только люди из вашего списка контактов |
| 3 | **Чаты только со знакомыми** | Пригласить вас в группу смогут только те, с кем вы уже общались |
| 4 | **Фильтр контента** | Вы увидите только безопасные посты и каналы |

Подзаголовок: "Без лишнего общения и контента"

## Кнопка

"Без кода" (`withoutPinCodeButton`) — включить SafeMode без PIN-кода.

## Связь с UserSettings

`safeMode` + `safeModeNoPin` в UserSettings (sgj). Синхронизируется с сервером.

## Что важно

1. **`safeMode`** — сервер знает, включён ли безопасный режим.

2. **Фильтр контента** — SafeMode ограничивает видимый контент. Сервер контролирует, что считается "безопасным".

3. **`safeModeNoPin`** — режим без PIN-кода. Более слабая защита.

4. Связано с `contentLevelAccess` в UserSettings.

## Сводка

SafeMode: скрытие профиля + звонки только от контактов + чаты только со знакомыми + фильтр контента. `safeMode`/`safeModeNoPin` в UserSettings.
