---
tags: [webapp, jsbridge, js-events, biometry, nfc, storage, surveillance, phone-number]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ijk.java
related:
  - "[[10-webapp-jsbridge]]"
  - "[[07-nfc-hce-webapp]]"
  - "[[368-webapp-vpn-exception]]"
---

# WebApp JS-события — полный список (34 события)

Полный список JS-событий, которые мини-приложения могут вызывать через JS-bridge.

## Биометрия

| Событие | Что |
|---|---|
| `WebAppBiometryGetInfo` | **получить информацию о биометрии** |
| `WebAppBiometryOpenSettings` | открыть настройки биометрии |
| `WebAppBiometryRequestAccess` | **запросить доступ к биометрии** |
| `WebAppBiometryRequestAuth` | **аутентификация через биометрию** |
| `WebAppBiometryUpdateToken` | обновить токен биометрии |

## NFC

| Событие | Что |
|---|---|
| `WebAppNfcEmulateNfcTag` | **эмулировать NFC-тег** |
| `WebAppNfcGetInfo` | получить информацию о NFC |
| `WebAppNfcOpenSystemSettings` | открыть настройки NFC |

## Хранилище

| Событие | Что |
|---|---|
| `WebAppDeviceStorageSaveKey` | **сохранить ключ в хранилище устройства** |
| `WebAppDeviceStorageGetKey` | **получить ключ из хранилища** |
| `WebAppDeviceStorageClear` | очистить хранилище |
| `WebAppSecureStorageSaveKey` | **сохранить ключ в защищённом хранилище** |
| `WebAppSecureStorageGetKey` | **получить ключ из защищённого хранилища** |
| `WebAppSecureStorageClear` | очистить защищённое хранилище |

## Навигация и UI

| Событие | Что |
|---|---|
| `WebAppReady` | мини-приложение готово |
| `WebAppClose` | закрыть мини-приложение |
| `WebAppSetupBackButton` | настроить кнопку назад |
| `WebAppSetupClosingBehavior` | поведение при закрытии |
| `WebAppSetupScreenCaptureBehavior` | **поведение захвата экрана** |
| `WebAppBackButtonPressed` | нажата кнопка назад |
| `WebAppGetViewportSize` | получить размер viewport |
| `WebAppChangeScreenBrightness` | **изменить яркость экрана** |

## Шаринг и ссылки

| Событие | Что |
|---|---|
| `WebAppShare` | поделиться |
| `WebAppMaxShare` | поделиться через MAX |
| `WebAppOpenLink` | открыть ссылку |
| `WebAppOpenMaxLink` | открыть ссылку в MAX |
| `WebAppDownloadFile` | **скачать файл** |
| `WebAppUrlInterceptor` | **перехватить URL** |

## Устройство

| Событие | Что |
|---|---|
| `WebAppRequestPhone` | **запросить номер телефона** |
| `WebAppVerifyMobileId` | **верифицировать Mobile ID (MSISDN)** |
| `WebAppOpenCodeReader` | открыть QR-сканер |
| `WebAppHapticFeedbackImpact` | тактильная обратная связь |
| `WebAppHapticFeedbackNotification` | тактильное уведомление |
| `WebAppHapticFeedbackSelectionChange` | тактильное изменение выбора |

## Что важно

1. **`WebAppBiometryRequestAuth`** — мини-приложения могут запрашивать биометрическую аутентификацию.

2. **`WebAppNfcEmulateNfcTag`** — мини-приложения могут эмулировать NFC-теги.

3. **`WebAppSecureStorageSaveKey`/`GetKey`** — мини-приложения имеют доступ к защищённому хранилищу.

4. **`WebAppVerifyMobileId`** — мини-приложения могут получать MSISDN (см. [[10-webapp-jsbridge]]).

5. **`WebAppSetupScreenCaptureBehavior`** — мини-приложения могут управлять захватом экрана.

6. **`WebAppUrlInterceptor`** — мини-приложения могут перехватывать URL-переходы.

7. **`WebAppChangeScreenBrightness`** — мини-приложения могут менять яркость экрана.

## Сводка

34 JS-события. Критические: WebAppBiometryRequestAuth/WebAppNfcEmulateNfcTag/WebAppSecureStorageSaveKey/WebAppVerifyMobileId/WebAppSetupScreenCaptureBehavior/WebAppUrlInterceptor.
