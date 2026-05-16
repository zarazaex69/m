---
tags: [shared-prefs, account, session, proxy, push-token, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/yag.java
related:
  - "[[21-shared-prefs-map]]"
  - "[[08-background-wake]]"
---

# yag SharedPreferences — полный список полей аккаунта

`yag` — основной SharedPreferences аккаунта. 60+ полей.

## Ключевые поля

| Поле | Что |
|---|---|
| `_userId` | ID пользователя |
| `okToken` / `okTokenRefreshTs` | **OK-токен авторизации** |
| `pushToken` / `pushDeviceType` / `pushOptions` | **FCM push токен** |
| `currentProxyList` / `currentProxyListTtlInSec` | **текущий список прокси** |
| `pushProxyList` | **прокси для push** |
| `lastSuccessProxy` / `lastProxyUpdateTime` | последний успешный прокси |
| `firstLoginTime` / `lastLoginTime` | время первого/последнего входа |
| `serverTimeDelta` | дельта времени с сервером |
| `installationMarket` / `installationInfoVersion` | источник установки |
| `isBackgroundWakeEnabled` | фоновое пробуждение |
| `isDebugHostRotationEnabled` / `isDebugUaDnsEmulationEnabled` | **debug-флаги в prod** |
| `isIceCandidateEmulationEnabled` | эмуляция ICE кандидатов |
| `lang` / `systemLang` / `multiLangEnabled` | язык |
| `digitalIdTooltipShown` | показан ли тултип DigitalID |
| `unexpectedLogErrorCount` | счётчик неожиданных ошибок |
| `lastLogSendTime` | время последней отправки логов |
| `loginFailError` | ошибка входа |
| `useTls` | использовать TLS |
| `forceConnection` | принудительное соединение |
| `resetAtTime` | время сброса |
| `transmitTaskVersion` | версия задач передачи |
| `contactsLastSync` / `callsLastSync` / `draftsLastSync` / `stickersLastSync` / `favoritesLastSync` / `reactionsLastSync` / `animojiSetsLastSync` | времена синхронизации |

## Что важно

1. **`okToken`** — токен авторизации OK.ru хранится в SharedPreferences.

2. **`pushToken`** — FCM токен хранится здесь. Сервер использует его для push-уведомлений.

3. **`currentProxyList`/`pushProxyList`** — списки прокси-серверов. Уже покрыто в [[21-shared-prefs-map]].

4. **`isDebugHostRotationEnabled`/`isDebugUaDnsEmulationEnabled`** — debug-флаги в production SharedPreferences.

5. **`isIceCandidateEmulationEnabled`** — эмуляция ICE кандидатов. Это debug-флаг для тестирования звонков.

## Сводка

`yag` SharedPreferences: 60+ полей. Ключевые: `okToken`/`pushToken`/`currentProxyList`/`pushProxyList`/`firstLoginTime`/`lastLoginTime`/`isDebugHostRotationEnabled`/`isIceCandidateEmulationEnabled`.
