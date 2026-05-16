---
tags: [neuroavatars, ai, registration, server-control, biometrics]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/login/neuroavatars/NeuroAvatarsScreen.java
  - work/jadx_base/sources/one/me/login/neuroavatars/RegistrationNeuroAvatarsScreen.java
  - work/jadx_base/sources/one/me/login/neuroavatars/NeuroAvatarPickerBottomSheet.java
related:
  - "[[09-native-libs]]"
  - "[[15-on-device-asr-kws-diarization]]"
---

# NeuroAvatars — AI-генерация аватаров при регистрации

В MAX присутствует полноценный модуль `one.me.login.neuroavatars` — генерация аватаров с помощью нейросети.

## Компоненты

- `NeuroAvatarsScreen` — экран выбора нейро-аватара
- `RegistrationNeuroAvatarsScreen` — экран нейро-аватара при регистрации
- `NeuroAvatarPickerBottomSheet` — bottom sheet выбора аватара
- `NeuroAvatarModel` — модель аватара
- `PresetAvatarsModel` — пресеты аватаров

## Что это значит

1. **При регистрации** пользователю предлагается выбрать нейро-аватар. Это означает, что фотография пользователя (или его данные) могут отправляться на серверы MAX для генерации аватара.

2. **`oneme_settings_change_avatar_upload_from_neuroavatars`** — строка ресурса, указывающая на возможность загрузки аватара из нейро-аватаров в настройках. Это не только при регистрации, но и в любой момент.

3. **`NeuroAvatarModel` с `onNewItemInFocus`** — при прокрутке списка аватаров каждый новый аватар в фокусе триггерит callback. Это может использоваться для аналитики (какие аватары пользователь рассматривал).

## Скептический разбор

- Нейро-аватары — стандартная фича (Telegram AI avatars, Snapchat Bitmoji).
- Что специфично: в государственном мессенджере без E2E — фотография пользователя уходит на серверы MAX для обработки нейросетью.
- Нет подтверждения, что генерация происходит on-device. Скорее всего — серверная.

## Сводка

MAX содержит полный модуль нейро-аватаров для регистрации и настроек. Фотография пользователя вероятно отправляется на серверы MAX для AI-обработки.
