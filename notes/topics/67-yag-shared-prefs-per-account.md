---
tags: [shared-prefs, proxy, debug, network, surveillance, token]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/yag.java
related:
  - "[[21-shared-prefs-map]]"
  - "[[03-pms-server-flags]]"
  - "[[41-server-side-client-diagnostic-struct]]"
---

# yag SharedPreferences — карта per-account состояния

`defpackage/yag.java` — SharedPreferences файл с per-account состоянием. Содержит 60+ полей. Ключевые:

## Сетевые и прокси поля

| Поле | Что |
|---|---|
| `currentProxyList` | текущий список прокси-серверов |
| `currentProxyListTtlInSec` | TTL списка прокси |
| `pushProxyList` | список прокси для push-уведомлений |
| `lastSuccessProxy` | последний успешный прокси |
| `lastProxyUpdateTime` | время последнего обновления прокси |
| `useTls` | использовать ли TLS |
| `serverTimeDelta` | дельта времени с сервером |

## Токены и идентификаторы

| Поле | Что |
|---|---|
| `okToken` | **OK-токен аутентификации** |
| `okTokenRefreshTs` | timestamp обновления токена |
| `pushToken` | FCM push-токен |
| `pushOptions` | опции push-уведомлений |
| `pushDeviceType` | тип устройства для push |
| `deviceId` | идентификатор устройства |
| `installationMarket` | откуда установлено (Play/APK/etc) |
| `installationInfoVersion` | версия информации об установке |

## Debug-флаги в production

| Поле | Что |
|---|---|
| `isDebugHostRotationEnabled` | **включить ротацию хостов** (debug) |
| `isDebugUaDnsEmulationEnabled` | **эмуляция UA DNS** (debug) |
| `isIceCandidateEmulationEnabled` | **эмуляция ICE candidates** (debug) |

## Синхронизация

| Поле | Что |
|---|---|
| `contactsLastSync` | timestamp синхронизации контактов |
| `stickersLastSync` | timestamp синхронизации стикеров |
| `callsLastSync` | timestamp синхронизации звонков |
| `draftsLastSync` | timestamp синхронизации черновиков |
| `chatsLastSync` | timestamp синхронизации чатов |
| `reactionsLastSync` | timestamp синхронизации реакций |

## Что важно

1. **`currentProxyList` и `pushProxyList`** — клиент хранит список прокси-серверов локально. Это отдельно от PmsKey `proxy-domains` (см. [[41-server-side-client-diagnostic-struct]]). Два независимых механизма прокси-конфигурации.

2. **`okToken`** — токен аутентификации хранится в SharedPreferences. Это стандартно для Android, но означает, что при root-доступе токен доступен без дополнительной защиты.

3. **`isDebugHostRotationEnabled`** и **`isDebugUaDnsEmulationEnabled`** — debug-флаги в production SharedPreferences. Ротация хостов и эмуляция DNS — это инструменты для тестирования сетевого поведения. Присутствие в production означает, что их можно включить через dev-меню (см. [[05-dev-menu-in-prod]]).

4. **`isIceCandidateEmulationEnabled`** — эмуляция ICE candidates для WebRTC. Это debug-инструмент для тестирования звонков без реального P2P.

5. **`installationMarket`** — клиент знает, откуда он был установлен (Google Play, APK, и т.п.). Эта информация отправляется на сервер.

## Сводка

`yag` SharedPreferences содержит 60+ per-account полей: прокси-списки, токены, device ID, debug-флаги (host rotation, DNS emulation, ICE emulation), timestamps синхронизации. Три debug-флага в production позволяют изменять сетевое поведение через dev-меню.
