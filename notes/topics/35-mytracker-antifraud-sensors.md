---
tags: [antifraud, fingerprinting, mytracker, sensors, telemetry]
status: confirmed
sources:
  - work/jadx_base/sources/com/my/tracker/config/AntiFraudConfig.java
  - work/jadx_base/sources/com/my/tracker/core/TrackerConfig.java
  - work/jadx_base/sources/com/my/tracker/MyTrackerConfig.java
related:
  - "[[04-telemetry-endpoints]]"
  - "[[27-hardcoded-keys-audit]]"
  - "[[18-manifest-deep-dive]]"
---

# MyTracker AntiFraud — сенсорный фингерпринт устройства

В составе MAX присутствует MyTracker SDK от VK с модулем **AntiFraudConfig**, который собирает данные с физических сенсоров устройства. Эта функция — стандартная для VK MyTracker и работает по умолчанию.

## Какие сенсоры

`com/my/tracker/config/AntiFraudConfig.java`:

| Сенсор | Поле | Default |
|---|---|---|
| Light sensor (датчик освещённости) | `useLightSensor` | если доступен на устройстве |
| Magnetic field sensor (магнитометр) | `useMagneticFieldSensor` | true |
| Gyroscope (гироскоп) | `useGyroscope` | true |
| Pressure sensor (барометр) | `usePressureSensor` | true |
| Proximity sensor (датчик близости) | `useProximitySensor` | если доступен на устройстве |

Все пять датчиков по умолчанию включены при старте MyTracker (`AntiFraudConfig.newBuilder().build()` в `TrackerConfig.java:24`).

## Как используется

«AntiFraud» — официальное название от VK MyTracker. Назначение:

1. **Детект эмулятора** — на эмуляторе (Genymotion, Bluestacks, Android Emulator) показания сенсоров либо плоские, либо отсутствуют. Boolean-условие «есть ли реалистичные сенсорные показания» отделяет реальный девайс от автоматизированной фермы кликов.
2. **Детект автоматизации** — даже если эмулятор симулирует сенсоры, последовательность показаний (короткие пики из шума акселерометра при касании экрана, корреляция гироскоп ↔ акселерометр) типична для держания телефона в руке.
3. **Device fingerprinting** — точные характеристики (offset/calibration магнитометра, шум гироскопа на покое, уровень давления) **уникальны для каждого устройства** и используются для дополнительного fingerprint'а в attribution-графе.

## Что отправляется

MyTracker отправляет результат фингерпринта (boolean «realDevice/emulator» + некоторые derivative-метрики) на `tracker-api.vk-analytics.ru` (см. [[04-telemetry-endpoints]]) в составе attribution-payload, наряду с `referrer`, `installTimestamp`, `appId` (см. [[21-shared-prefs-map]] и [[27-hardcoded-keys-audit]]).

Настройка по умолчанию — все 5 сенсоров активно опрашиваются. PmsKey-уровневого выключения этого нет (это часть MyTracker SDK, а не one.me-кода). Глобальный гейт — PmsKey `mytracker-enabled` (см. [[03-pms-server-flags]]); если он выключен, MyTracker не работает вовсе.

## Что важно

1. **Сенсорный фингерпринт активен на каждом MAX-устройстве по умолчанию**, без UI-видимости. Permissions для гироскопа/акселерометра/магнитометра/барометра/proximity на Android ниже SDK 31 не требуют runtime-permission (только `android.sensor.*` через manifest-features), и в общем случае пользователь не предупреждается.
2. **Это не SBer-антифрод**. В прессе (Wikipedia: «Сбербанк России и VK в будущем внедрят технологии антифрода для выявления подозрительных звонков и предупреждения мошеннических действий») упоминался отдельный модуль для звонков. В коде MAX 26.15.3 я **не нашёл** интеграции с Sberbank antifraud. Только MyTracker AntiFraud (VK-овский, для attribution-целостности).
3. С точки зрения privacy этот фингерпринт **позволяет идентифицировать устройство сквозь переустановку приложения** (значения сенсоров не меняются от reinstall'а).

## Сводка

В MAX встроен сенсорный device-fingerprint (через 5 датчиков) от MyTracker SDK — стандартная практика VK для борьбы с фейковыми установками и attribution fraud. Включён по умолчанию, выключается только глобальным kill-switch'ем `mytracker-enabled` с сервера. Sberbank-antifraud (который анонсировался как функция MAX) в этой версии клиента отсутствует.
