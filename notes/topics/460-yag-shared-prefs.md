---
tags: [shared-prefs, user-prefs, device-id, fcm-token, proxy, server-time, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/yag.java
related:
  - "[[21-shared-prefs-map]]"
  - "[[416-device-info-vm5-cgj]]"
  - "[[19-fcm-push-payload]]"
---

# yag — SharedPreferences пользователя (полный список)

`yag` — основной объект SharedPreferences пользователя. 50+ ключей.

## Ключевые поля

| Ключ | Что |
|---|---|
| `user.Id` | **ID пользователя** |
| `device.id` | **ID устройства** |
| `user.fcmToken` | **FCM токен** |
| `user.pushDeviceType` | тип push-устройства |
| `user.okToken` | **OK токен** |
| `user.lang` | язык (default: "ru") |
| `user.systemLang` | системный язык |
| `server.timeDelta` | **дельта времени сервера** |
| `app.currentProxyList` | текущий список прокси |
| `app.pushProxyList` | список push-прокси |
| `app.lastSuccessProxy` | последний успешный прокси |
| `app.first.login.time` | **время первого входа** |
| `app.last.login.time` | **время последнего входа** |
| `app.last.firebase_push_time` | время последнего FCM push |
| `install-market` | источник установки |
| `install-version` | версия при установке |
| `background.wake.enabled` | фоновая работа включена |
| `background.wake.suggestion.shown.time` | время показа предложения |
| `user.shownDigitalIdTooltip` | показан ли DigitalID tooltip |
| `app.debugHostRotation` | ротация хостов (debug) |
| `app.debugUaDnsEmulation` | эмуляция DNS (debug) |
| `app.calls_sdk.ice_candidate_emulation` | эмуляция ICE candidates |
| `transmit.version` | версия протокола |
| `user.callSession` | **активные сессии звонков** |
| `app.forceConnection` | принудительное соединение |
| `app.invalidate.force.ver` | версия принудительной инвалидации |
| `app.invalidate.exception.flag` | флаг исключения инвалидации |
| `user.unexpectedLogErrorCount` | счётчик неожиданных ошибок |
| `user.lastLogSendTime` | время последней отправки логов |

## Что важно

1. **`device.id`** — ID устройства хранится в SharedPreferences.

2. **`user.fcmToken`** — FCM токен хранится в SharedPreferences.

3. **`server.timeDelta`** — дельта времени сервера. Используется для синхронизации времени.

4. **`app.first.login.time`/`app.last.login.time`** — время первого и последнего входа.

5. **`user.callSession`** — список активных сессий звонков.

6. **`app.debugHostRotation`/`app.debugUaDnsEmulation`** — debug-флаги в production.

## Сводка

`yag`: 50+ ключей SharedPreferences. Ключевые: device.id/user.fcmToken/user.okToken/server.timeDelta/app.first.login.time/user.callSession.
