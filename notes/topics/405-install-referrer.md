---
tags: [install-referrer, attribution, tracking, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/com/android/installreferrer/api/ReferrerDetails.java
  - work/jadx_base/sources/com/android/installreferrer/api/InstallReferrerClient.java
related:
  - "[[385-mytracker-details]]"
  - "[[04-telemetry-endpoints]]"
---

# InstallReferrer — атрибуция установки

`InstallReferrerClient` (Google Play Install Referrer API) — получение данных об источнике установки приложения.

## ReferrerDetails

| Поле | Что |
|---|---|
| `getInstallReferrer()` | **строка referrer** (UTM-параметры и т.д.) |
| `getInstallVersion()` | версия при установке |
| `getInstallBeginTimestampSeconds()` | **timestamp начала установки** |
| `getInstallBeginTimestampServerSeconds()` | timestamp с сервера Google |
| `getReferrerClickTimestampSeconds()` | **timestamp клика по ссылке** |
| `getReferrerClickTimestampServerSeconds()` | timestamp клика с сервера |
| `getGooglePlayInstantParam()` | установлено через Google Play Instant |

## Что важно

1. **`installReferrer`** — строка с UTM-параметрами. Позволяет отследить, откуда пришёл пользователь (реклама, ссылка, кампания).

2. **`referrerClickTimestamp`** — точное время клика по рекламной ссылке.

3. **`installBeginTimestamp`** — точное время начала установки.

4. Данные передаются в MyTracker для атрибуции.

## Сводка

`ReferrerDetails`: installReferrer/installVersion/installBeginTimestamp/referrerClickTimestamp. Передаётся в MyTracker для атрибуции установки.
