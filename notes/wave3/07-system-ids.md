---
tags: [system-ids, fingerprinting, privacy, wave3]
status: complete
severity: medium
sources: [jadx decompilation, native strings analysis]
related: [[542-traceflow-dps-deanonymization]]
---

# 07 — Сбор системных идентификаторов в MAX 26.15.3

## TL;DR

**Нет IMEI/IMSI/MAC/Serial/ICCID/MSISDN-хваталок.** MAX не вызывает `getImei()`, `getSubscriberId()`, `getSimSerialNumber()`, `getLine1Number()`, `getMacAddress()`, `getHardwareAddress()`, `Build.SERIAL`, `getSerial()`. Нет обращений к `gsm.imei`, `ril.serialnumber`, `ro.serialno`, `persist.sys.*` через Java-код.

Реально собираемые системные идентификаторы — ниже. Ничего **нового и незадокументированного** уровня IMEI/IMSI не найдено. Основная находка — подтверждение того, что `kc5` (Widevine deviceUniqueId) используется **одновременно** trace-flow DPS и основным приложением как единый cross-context hardware fingerprint.

## Собираемые системные идентификаторы

### 1. Widevine deviceUniqueId (SHA-256 → 16 hex chars)

| Параметр | Значение |
|----------|----------|
| Класс | `kc5.java` (implements `sm5`) |
| Метод | `MediaDrm(Widevine UUID).getPropertyByteArray("deviceUniqueId")` → SHA-256 → first 8 bytes hex |
| Fallback | `Settings.Secure.getString("android_id")` → SHA-256 → 16 hex (если Widevine недоступен) |
| Fallback filter | Исключает `"9774d56d682e549c"` (эмулятор) |
| Потребители | **trace-flow DPS** (`DpsInitProvider.java:66`), **основное приложение** (`zu5.java:858`) |
| Куда уходит | `https://trace-flow.ru/api/v1/report` (поле `deviceId` в JSON), `api.oneme.ru` (через основной WS-протокол) |

**Почему важно:** Widevine deviceUniqueId — hardware-bound, не сбрасывается при factory reset на большинстве устройств. Используется как **persistent cross-app fingerprint**. Один и тот же хеш привязывает пользователя в trace-flow и в основном протоколе MAX.

### 2. android_id (Settings.Secure)

| Параметр | Значение |
|----------|----------|
| Файлы | `vm5.java:73`, `r04.java:144`, `kc5.java:61` (fallback) |
| Метод | `Settings.Secure.getString(resolver, "android_id")` |
| Использование в vm5 | Генерация `deviceId` для основного WS-протокола (сохраняется в SharedPreferences через `ri9`) |
| Использование в r04 | Детекция эмулятора (проверка на null) |
| Использование в kc5 | Fallback если Widevine недоступен |
| Куда уходит | `api.oneme.ru` (основной протокол), `https://trace-flow.ru` (через kc5 hash) |

### 3. MCC/MNC (SIM Operator)

| Параметр | Значение |
|----------|----------|
| Файл | `yk2.java:123` |
| Метод | `TelephonyManager.getSimOperator()` |
| Куда уходит | Google Firebase/Crashlytics CCT transport (стандартное поле `mcc_mnc`) |
| Оценка | Стандартное поведение Firebase SDK, не аномалия |

### 4. Network Operator Name + MCC/MNC

| Параметр | Значение |
|----------|----------|
| Файлы | `zu5.java:622`, `f58.java:124`, `ilb.java:671` |
| Метод | `TelephonyManager.getNetworkOperator() + ":" + getNetworkOperatorName()` |
| Куда уходит (zu5) | `https://trace-flow.ru/api/v1/report` (поле `connectionType` в snapshot JSON) |
| Куда уходит (f58) | `api.oneme.ru` — WS-опкод `HOST_REACHABILITY` (поле `operator`) |
| Куда уходит (ilb) | `sdk-api.apptracer.ru` — crash/session report |

### 5. Network Country ISO

| Параметр | Значение |
|----------|----------|
| Файлы | `bhj.java:949`, `ta0.java:45` |
| Метод | `TelephonyManager.getNetworkCountryIso()` |
| Использование | Определение страны для адаптивного битрейта (ExoPlayer) и локализации |
| Куда уходит | Локально (не отправляется на сервер) |

### 6. Google Advertising ID (GAID) / Huawei OAID

| Параметр | Значение |
|----------|----------|
| Файлы | `com/my/tracker/core/o/w.java` (GAID), `com/my/tracker/core/o/z.java` (OAID) |
| Метод | `AdvertisingIdClient.getAdvertisingIdInfo()` |
| Куда уходит | `tracker-api.vk-analytics.ru` (MyTracker proto field 31) |
| Оценка | Стандартное поведение MyTracker SDK |

### 7. MyTracker customParams: android_id + mac

| Параметр | Значение |
|----------|----------|
| Файл | `com/my/tracker/core/proto/a.java:266-267` |
| Механизм | Читает из `trackerParams.customParams` map ключи `"android_id"` и `"mac"` |
| Реальность | В коде MAX **не найден** вызов `setCustomParam("android_id", ...)` или `setCustomParam("mac", ...)` — поля остаются null |
| Куда уходит | `tracker-api.vk-analytics.ru` (если бы были заполнены) |

### 8. WiFi SSID

| Параметр | Значение |
|----------|----------|
| Файл | `org/webrtc/NetworkMonitorAutoDetect.java:620` |
| Метод | `WifiInfo.getSSID()` |
| Использование | WebRTC network monitoring (определение типа сети для ICE) |
| Куда уходит | Локально (WebRTC ICE candidate selection) |

## Settings.Secure / Settings.Global / Settings.System — полный список ключей

| Ключ | Файл | Назначение |
|------|------|-----------|
| `android_id` | `vm5.java:73`, `kc5.java:61` | Device ID generation |
| `android_id` | `r04.java:144` | Emulator detection |
| `enabled_notification_listeners` | `a3c.java:125`, `wga.java:52` | Проверка доступа к уведомлениям (для NotificationListenerService) |
| `bluetooth_name` | `com/my/tracker/core/o/s0.java:35` | Debug phone detection (MyTracker) |
| `device_name` | `com/my/tracker/core/o/s0.java:49` | Debug phone detection (MyTracker) |

## Нативный код: __system_property_get

| Библиотека | Свойства | Назначение |
|-----------|----------|-----------|
| `libEnhancementLibShared.so` | `ro.build.version.sdk`, `ro.product.board`, `ro.hardware.chipname` | Выбор оптимального DSP-пути для аудио/видео обработки |
| `libtracernative.so` | `ro.arch` | Определение архитектуры для crash reporter |
| `libjingle_peerconnection_so.so` | (generic) | WebRTC internal — стандартное поведение |

**Оценка:** Все три библиотеки читают только hardware capability properties для оптимизации. Нет чтения `gsm.imei`, `ril.serialnumber`, `ro.serialno`, `persist.sys.*` или других privacy-sensitive properties.

## Что НЕ найдено (отсутствует в коде)

- ❌ `getImei()` / `getMeid()` / `getEsn()` — нет
- ❌ `getSubscriberId()` (IMSI) — нет
- ❌ `getSimSerialNumber()` (ICCID) — нет
- ❌ `getLine1Number()` / `getMsisdn()` — нет (номер получается через Mobile ID HTTP, не через TelephonyManager)
- ❌ `getMacAddress()` / `getHardwareAddress()` — нет
- ❌ `BluetoothAdapter.getAddress()` — нет
- ❌ `Build.SERIAL` / `getSerial()` — нет
- ❌ `gsm.imei` / `ril.serialnumber` / `ro.serialno` — нет
- ❌ Обфусцированные вызовы через `z8f.a` с паттернами 'Imei'/'Imsi' — нет

## Вывод

MAX **не собирает** hardware identifiers уровня IMEI/IMSI/MAC/Serial, которые требуют `READ_PHONE_STATE`. Основной persistent fingerprint — **Widevine deviceUniqueId** (hardware-bound, не требует runtime permission), дополненный `android_id` как fallback.

Главная проблема — не сами идентификаторы, а их **связка**: один и тот же `kc5` hash используется и в trace-flow DPS (деанонимизация IP), и в основном протоколе MAX. Это позволяет серверу однозначно связать:
- Реальный публичный IP (из trace-flow) 
- Аккаунт MAX (userId из основного протокола)
- Оператора связи (networkOperator)
- VPN-статус

...через единый hardware fingerprint, без необходимости в IMEI/IMSI.

**Новых незадокументированных ID-хваталок не обнаружено.** Всё найденное — подтверждение и детализация уже задокументированного в topic 542 (trace-flow DPS).
