---
tags: [shared-prefs, session, debug, gost, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ri9.java
related:
  - "[[21-shared-prefs-map]]"
  - "[[224-yag-shared-prefs-full]]"
---

# ri9 SharedPreferences — пользовательские настройки сессии

`ri9` — SharedPreferences пользовательской сессии. 50+ полей.

## Ключевые поля

| Поле | Что |
|---|---|
| `phoneCode` / `phoneNumber` | **номер телефона** |
| `locationCountryCode` | **код страны** |
| `serverHost` / `serverPort` | хост/порт сервера |
| `useTls` | использовать TLS |
| `statSessionId` | ID статистической сессии |
| `gostLicenseCheckEnabled` | **проверка лицензии ГОСТ** |
| `allowLogSensitiveData` | **разрешить логирование чувствительных данных** |
| `isDisableWebAppSsl` | **отключить SSL для WebApp** |
| `isCallsDebugMenuEnabled` | **debug-меню звонков** |
| `isVideoDebugViewAvailable` | debug-вид видео |
| `isDebugProfileInfoEnabled` | debug-профиль |
| `isDebugFresco` | debug Fresco |
| `isDevOptionsRoaming` | dev-опции роуминга |
| `leakCanaryEnabledStateFlow` | **LeakCanary включён** |
| `tenorAnonId` | анонимный ID Tenor (GIF) |
| `incomingCallRingtone` | рингтон входящего звонка |
| `ignoreBatteryOptimizationsRequestCount` | счётчик запросов игнорирования оптимизации батареи |
| `isTranscriptionOnboardingEnded` | онбординг транскрипции завершён |
| `audioVideoMessagePlaybackSpeed` | скорость воспроизведения |
| `videoPlayQuality` | качество видео |

## Что важно

1. **`allowLogSensitiveData`** — флаг разрешения логирования чувствительных данных. Если включён — в логи попадают персональные данные.

2. **`isDisableWebAppSsl`** — отключение SSL для WebApp. Debug-флаг в production.

3. **`gostLicenseCheckEnabled`** — проверка лицензии ГОСТ. Связано с [[22-gost-digitalid-family]].

4. **`phoneNumber`** — номер телефона хранится в SharedPreferences.

5. **`tenorAnonId`** — анонимный ID для Tenor (GIF-сервис). Tenor — Google-сервис.

## Сводка

`ri9` SharedPreferences: `phoneNumber`/`locationCountryCode`/`gostLicenseCheckEnabled`/`allowLogSensitiveData`/`isDisableWebAppSsl`/`leakCanaryEnabledStateFlow`/`tenorAnonId`.
