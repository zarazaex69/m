---
tags: [secrets, keys, firebase, mytracker, hardcoded]
status: confirmed
sources:
  - work/apktool_base/AndroidManifest.xml
  - work/apktool_base/res/values/strings.xml
  - work/jadx_base/sources/defpackage/d6.java
related:
  - "[[04-telemetry-endpoints]]"
  - "[[19-fcm-push-payload]]"
  - "[[18-manifest-deep-dive]]"
---

# Hardcoded keys / IDs / certificates audit

Краткий перечень всех захардкоженных идентификаторов в APK с пометкой «public/private».

## 1. Google Maps Geo (manifest)

`com.google.android.geo.API_KEY = AIzaSyDJbuC3fODS_aR7jcOkoP6qWIsQen9XARI` в манифесте. Это **публичный API key** для Google Maps Android (сам Google требует ограничивать его SHA1+package name, что и применяется на стороне Google). Не «секрет», просто идентификатор.

## 2. Firebase config (`res/values/strings.xml`)

Стандартное содержимое `google-services.json`, склеенное в строковые ресурсы:

| Ключ | Значение |
|---|---|
| `gcm_defaultSenderId` | `659634599081` |
| `google_app_id` | `1:659634599081:android:9605285443b661167225b8` |
| `project_id` | `max-messenger-app` |
| `google_storage_bucket` | `max-messenger-app.firebasestorage.app` |
| `google_api_key` | `AIzaSyABuDYeeDXIOrKTXLkUj30Ii143ofPe63Q` |
| `google_crash_reporting_api_key` | `AIzaSyABuDYeeDXIOrKTXLkUj30Ii143ofPe63Q` (тот же) |

Это публичная Firebase-конфигурация. FCM Sender ID `659634599081` — то, через какой sender server MAX шлёт FCM-push'и (см. [[19-fcm-push-payload]]). `project_id = max-messenger-app` — имя Firebase-проекта.

`FirebaseInitProvider` в манифесте при этом выключен (`enabled=false`, см. [[18-manifest-deep-dive]]) — это значит, что **Firebase SDK не инициализируется автоматически**; кроме FCM-канала push (через явный `FirebaseInstanceIdReceiver`+`FcmMessagingService`) больше Firebase-фич не задействовано.

## 3. MyTracker app ID

`defpackage/d6.java:137` — `MyTracker.initTracker("34982109644049932883", oneMeApplication)`.

Это application ID MAX в платформе VK MyTracker (`tracker-api.vk-analytics.ru`, см. [[04-telemetry-endpoints]]). Публичный идентификатор; используется для отчётности внутри VK-стека.

## 4. Apptracer

В коде (`defpackage/ati.java:5`) — захардкоженный URL `https://sdk-api.apptracer.ru` (см. [[17-apptracer-uplink]]). Никаких токенов в Java-коде нет; токен (`tracer_mapping_uuid`) вписывается в манифест на этапе сборки и читается из meta-data в `TracerInitializer`.

## 5. Что искал и не нашёл

- **Не нашёл** в открытом виде:
  - private-ключей RSA/EC/Ed25519 (никаких `-----BEGIN PRIVATE KEY-----`),
  - JWT-секретов,
  - HMAC-ключей вида `secret = "..."`,
  - закодированных Base64-блобов длиной похожей на ключ или сертификат,
  - встроенных x509-сертификатов в `assets/` или `res/raw/`,
  - паролей по умолчанию.

- **Не нашёл** captive внутреннего CA-bundle для tracer-стека (см. [[17-apptracer-uplink]] §5: `tracer_set_ssl_cainfo` — функция есть, но в JNI не выставлена и реализация в `.rodata` не содержит PEM).

## 6. WebSocket токены / шифрование

В коде клиента — никаких pre-shared keys для соединения с `api.oneme.ru`. Аутентификация WS-сессии — через login-flow (опкоды `LOGIN(19)`, `AUTH_CONFIRM(23)`), результат — auth-токен в SharedPreferences и/или зашифрованный сессионный ключ. PMS-флаг `net-ssl-session-validate` управляет валидацией TLS-сессии (см. [[03-pms-server-flags]]).

## 7. Итог

В APK MAX 26.15.3 нет **скрытых backdoor-ключей** в смысле «captive private key или signing secret для backdoor-канала». Все хардкоженные идентификаторы — публичные конфиг-значения экосистемы Google Firebase + VK MyTracker + Apptracer. Единственный «странный» — что один и тот же `AIzaSyABuDYeeDXIOrKTXLkUj30Ii143ofPe63Q` используется и как `google_api_key`, и как `google_crash_reporting_api_key` (норма для Firebase, одно значение покрывает весь проект).

Это **не значит, что в библиотеках нет встроенных секретов** — реверс `.rodata` всех 16 `.so` на предмет zero-knowledge зашитых блобов (например, для шифрования `vk::enh::decrypt(...)` ML-моделей, см. [[15-on-device-asr-kws-diarization]]) — отдельная задача. По имеющемуся беглому страйнг-анализу очевидных PEM/JWK не найдено.
