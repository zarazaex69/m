---
tags: [2fa, security, stories, server-control, pms]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/e.java
  - work/jadx_base/sources/defpackage/e2h.java
  - work/jadx_base/sources/one/me/settings/twofa/creation/TwoFACreationScreen.java
  - work/jadx_base/sources/one/me/settings/twofa/creation/onboarding/TwoFAOnboardingScreen.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[03-pms-server-flags]]"
  - "[[22-gost-digitalid-family]]"
---

# 2FA, Stories, ilm — дополнительные server-gated фичи

## 2FA (creation-2fa-config)

В MAX реализован полноценный 2FA-flow: `TwoFACreationScreen` и `TwoFAOnboardingScreen` в пакете `one.me.settings.twofa.creation`. Параметры flow передаются через Bundle:

- `creation_2fa_type_key` — тип 2FA (SMS, TOTP, и т.п.)
- `creation_2fa_step_key` — текущий шаг
- `creation_2fa_source_key` — откуда запущен flow
- `creation_2fa_track_id_key` — tracking ID для аналитики
- `onboarding_2fa_state_key` — состояние онбординга

PmsKey `creation-2fa-config` (#?) — server-pushed конфиг 2FA. Иконки: `oneme_settings_privacy_cloud_2fa_start_icon` и `oneme_settings_privacy_cloud_2fa_end_icon` — «облачный» 2FA (вероятно, cloud backup кодов).

Что важно: `creation_2fa_track_id_key` — tracking ID для аналитики 2FA-flow. Сервер знает, когда пользователь начал и завершил настройку 2FA, и через какой source (настройки, принудительный prompt, и т.п.).

## Stories (story PmsKey)

`PmsKey.story` — server-gated bool, default `false`. Описание в `e2h.java` пустое. Пакет `one.me.stories.publish.PublishStoryBottomSheet` — UI публикации Stories. `PickStoryPresetScreen` — выбор пресета для Stories.

Stories — это ephemeral-контент (исчезающие публикации). Фича выключена по умолчанию, включается сервером. Хранение Stories — серверное (как и у Telegram/Instagram).

## ilm — «Отключить инвалидацию последних сообщений при смене локали»

`PmsKey.ilm` — bool, default `true`. Описание из `e.java`: «Отключить инвалидацию последних сообщений и заголовков при смене локали».

Это технический PmsKey для оптимизации: при смене языка интерфейса клиент по умолчанию не инвалидирует кэш последних сообщений. Сервер может включить инвалидацию (`ilm=false`), что заставит клиент перезагрузить все заголовки чатов.

## blocked-users

`PmsKey.f16blockedusers` — bool, default `false`. Описание из `e.java`: «Уведомление о заблокированных пользователях». Server-gated включение уведомлений о том, что пользователь заблокирован кем-то.

## Сводка

2FA в MAX — полноценный flow с tracking ID для аналитики. Stories — server-gated, выключены по умолчанию. `ilm` — технический PmsKey для управления кэш-инвалидацией. `blocked-users` — server-gated уведомления о блокировках.
