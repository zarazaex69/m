---
tags: [twofa, 2fa, two-factor-auth, security, pin-code, password, settings]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/settings/twofa/configuration/TwoFASettingsScreen.java
  - work/jadx_base/sources/one/me/settings/twofa/creation/TwoFACreationScreen.java
  - work/jadx_base/sources/one/me/settings/twofa/password/TwoFACheckPassScreen.java
  - work/jadx_base/sources/one/me/settings/twofa/restore/TwoFAStartRestoreScreen.java
  - work/jadx_base/sources/one/me/settings/twofa/restore/ProfileDeletionInfoScreen.java
related:
  - "[[11-state-bots-and-content-policy]]"
  - "[[22-gost-digitalid-family]]"
---

# TwoFA — двухфакторная аутентификация

MAX поддерживает двухфакторную аутентификацию (2FA).

## Экраны

| Экран | Что |
|---|---|
| `TwoFASettingsScreen` | настройки 2FA (включить/выключить) |
| `TwoFACreationScreen` | создание 2FA |
| `TwoFAOnboardingScreen` | онбординг 2FA |
| `TwoFACheckPassScreen` | проверка пароля 2FA |
| `TwoFAStartRestoreScreen` | восстановление 2FA |
| `ProfileDeletionInfoScreen` | информация об удалении профиля |

## Что важно

1. **`disable_twofa_positive`** — диалог подтверждения отключения 2FA.

2. **`twofa_settings_track_id_key`** — трекинг ID для аналитики 2FA.

3. **`ProfileDeletionInfoScreen`** — экран информации об удалении профиля связан с 2FA.

4. Связано с `SafeModeOnboardingViewModelFactory` и `SafeModeNoPin` в UserSettings.

## Сводка

TwoFA: 6 экранов (Settings/Creation/Onboarding/CheckPass/StartRestore/ProfileDeletion). `twofa_settings_track_id_key` для аналитики.
