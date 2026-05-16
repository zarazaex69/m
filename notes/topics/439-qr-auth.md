---
tags: [qr-auth, devices, session, security, settings]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/settings/devices/SettingsDevicesScreen.java
  - work/jadx_base/sources/one/me/settings/devices/hintdialog/QrAuthHintBottomSheet.java
  - work/jadx_base/sources/defpackage/eyg.java
related:
  - "[[437-twofa-screens]]"
  - "[[438-safe-mode]]"
---

# QR Auth — авторизация через QR-код

MAX поддерживает авторизацию через QR-код для входа с другого устройства.

## Компоненты

### SettingsDevicesScreen

Экран управления устройствами. Список активных сессий + кнопка добавления устройства через QR.

### QrAuthHintBottomSheet

Подсказка для QR-авторизации.

### eyg (ViewModel)

- `authQrJob` — задача авторизации через QR
- `:qr-scanner?mode=2` — deeplink для открытия QR-сканера

## Что важно

1. **QR-авторизация** — пользователь может авторизоваться на новом устройстве, отсканировав QR-код.

2. **`mode=2`** — режим QR-сканера для авторизации.

3. **`AuthQrUseCase`** — use case для QR-авторизации.

4. Связано с `GetQrCodeUseCase` — генерация QR-кода.

## Сводка

QR Auth: SettingsDevicesScreen + QrAuthHintBottomSheet + `:qr-scanner?mode=2`. `AuthQrUseCase`/`GetQrCodeUseCase`.
