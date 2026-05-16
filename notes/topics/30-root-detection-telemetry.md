---
tags: [anti-tamper, root-detection, telemetry, mytracker, apptracer]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ij9.java
  - work/jadx_base/sources/defpackage/zbi.java
  - work/jadx_base/sources/defpackage/mpf.java
  - work/jadx_base/sources/com/my/tracker/core/o/m.java
related:
  - "[[09-native-libs]]"
  - "[[04-telemetry-endpoints]]"
  - "[[17-apptracer-uplink]]"
---

# Root-detection — есть, но только в телеметрию

Уточнение к предыдущей формулировке в [[FINDINGS]] §14 («нет анти-tampering / детектора root/Frida/Xposed/debuggable»).

В коде клиента **активной блокировки** rooted-устройств / Frida / Xposed / debuggable действительно нет: приложение запускается, работает, не показывает «вы используете root, выйдите».

Однако сторонние SDK, встроенные в APK, **детектируют root и сообщают об этом в телеметрию**:

| Файл | Что |
|---|---|
| `defpackage/ij9.java` (apptracer) | `JSONObject.put("isRooted", zbiVar.m)` — поле `isRooted` в session-info, отправляемом на `sdk-api.apptracer.ru` |
| `defpackage/zbi.java` (apptracer session-info) | поле `m: Boolean` со значением root-состояния |
| `defpackage/mpf.java` | `jSONObject2.put("isRooted", r04.G(context))` — добавление поля в общий event-payload |
| `com/my/tracker/core/o/m.java` (MyTracker) | проверка путей `/sbin/.magisk/`, `/sbin/.core/mirror`, `/sbin/.core/img`, `/sbin/.core/db-0/magisk.db` — детект Magisk |

То есть:

- MyTracker (VK-овский трекер, см. [[04-telemetry-endpoints]]) умеет детектить установленный Magisk через проверку файловых путей.
- Apptracer и собственный код MAX добавляют флаг `isRooted` в события, идущие на `sdk-api.apptracer.ru` (см. [[17-apptracer-uplink]]) и аналитический канал.
- Клиент не блокирует root, но **сервер MAX знает, что у пользователя X устройство rooted**.

## Что не нашёл

- Активного `isDebuggerConnected()` / `Debug.isDebuggerConnected()` в коде клиента.
- Frida-detection (поиск `/data/local/tmp/frida-server`, `frida-gadget` и пр.).
- Xposed-detection.
- SafetyNet / Play Integrity / hardware attestation вызовов.
- `debuggable` self-check.

## Сводка

Корректнее формулировка: **MAX 26.15.3 не имеет блокирующего анти-тампера, но stale `isRooted`-флаг собирается в телеметрию через apptracer + MyTracker**. Сервер MAX может в любой момент использовать это поле для серверной политики (например, помечать аккаунты для дополнительной проверки). Само приложение работает на rooted-устройстве и под Frida без сопротивления — что подтверждает, что текущий ресёрч из распакованного APK не сталкивается с защитой.
