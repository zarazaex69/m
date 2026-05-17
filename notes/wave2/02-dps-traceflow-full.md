---
tags: [dps, trace-flow, fingerprint, reachability, network-probe, server-control]
status: verified
sources: [zu5.java, kyk.java, kzk.java, v0l.java, wzk.java, bzk.java, qzk.java, y0l.java, DpsInitProvider.java, AccountInitializer.java, hph.java, n6.java, a0l.java, o2.java, n1l.java, q0l.java, d1l.java, f0l.java, PmsKey.java]
related: [[03-pms-server-flags]], [[04-telemetry-endpoints]], [[131-host-reachability]]
version_check: present_in_26.16.0
---

# DPS SDK / trace-flow.ru — полный реверс

## TL;DR

DPS (Digital Probe System) — встроенный SDK от `ru.trace_flow`, который при каждом выходе приложения на передний план:
1. Определяет **публичный IP** пользователя через 6 внешних сервисов
2. Проверяет **DNS/TCP/TLS-доступность** серверов MAX (`api.oneme.ru`)
3. Определяет **VPN-статус**, тип сети, оператора (MCC:MNC + имя)
4. Отправляет всё это как JSON-snapshot на **`https://trace-flow.ru/api/v1/report`**

Это **не** аналитика производительности — это **сетевой фингерпринт** пользователя с привязкой к userId и deviceId.

---

## 1. Архитектура и классы

| Обфусцированный класс | Роль | Оригинальное имя (из metadata) |
|---|---|---|
| `zu5` | Главный класс SDK, оркестратор | `Dps` (module: `dpslib`) |
| `zu5.a` | Builder | `Dps.Builder` |
| `kyk` | HTTP-клиент отправки snapshot'ов | `ReportClient` |
| `bzk` | Конфиг (хосты, таймауты, лимиты) | `Config` |
| `kzk` | Кэш конфига на диск | `ConfigCache` |
| `v0l` | Лог-файл snapshot'ов (append-only) | `LogStorage` |
| `wzk` | Проверки DNS/TCP/TLS | `ReachabilityChecker` |
| `qzk` | Определение типа сети + VPN | `NetworkInfo` |
| `y0l` | Проверка наличия интернета | `ConnectivityChecker` |
| `u4k` | Интерфейс часов | `WallClock` |
| `a0l` | ActivityLifecycleCallbacks (триггер) | `ForegroundDetector` |
| `n1l` | Snapshot-запись для отправки | `Snapshot` |
| `q0l` | Лог-запись (промежуточная) | `LogEntry` |
| `d1l` | Хост для проверки (id + hostname) | `CheckTarget` |
| `f0l` | Результат проверки (checkType + status) | `CheckResult` |
| `p1l` | Битовая маска результатов | `StatusBits` |
| `k1l` | Ответ сервера (новый конфиг + retryAfter) | `ReportResponse` |
| `DpsInitProvider` | ContentProvider для авто-инициализации | — |

---

## 2. Инициализация

### Путь 1: ContentProvider (ОТКЛЮЧЁН)

```xml
<!-- AndroidManifest.xml -->
<provider android:authorities="ru.oneme.app.dps-init-provider"
          android:enabled="false"
          android:name="ru.trace_flow.dps.internal.DpsInitProvider"/>
```

Provider `enabled="false"` — авто-инициализация через ContentProvider **отключена**.

### Путь 2: AccountInitializer (АКТИВНЫЙ)

```java
// AccountInitializer.a() — вызывается при инициализации аккаунта
// Гейтится серверным PmsKey "dps" (ordinal 228)
if (pmsFlags.getBoolean("dps")) {
    new Dps.Builder()
        .setApplication(app)
        .setApiKey("ply5hDvhupghrHVA5rqQD1ypiXAxbmE4A68ZzBa8ioc=")
        .setUserIdSupplier(() -> String.valueOf(account.getUserId()))
        .setDeviceIdSupplier(() -> account.getDeviceId())
        .setClientVersion(() -> "26.15.3")
        .setExecutorService(threadPool("dps", 0, 2, true, true, 1, 2))
        .setTlsCheckEnabled(devicePerformanceClass == HIGH)
        .setWallClock(wallClock)
        .build();
}
```

**Ключевые факты:**
- API Key: `ply5hDvhupghrHVA5rqQD1ypiXAxbmE4A68ZzBa8ioc=`
- Включение контролируется **сервером** через PmsKey `"dps"`
- TLS-проверка включается только на мощных устройствах
- userId и deviceId привязываются к snapshot'ам
- Инициализация происходит **ПОСЛЕ логина** (в `AccountInitializer`)

### Задача "Dps" в init-графе

```java
// AccountInitializer.e() — регистрация задачи
b(zukVar, "Dps", emptyList, new d6(this, oneMeApplication, 11));
```

Задача `"Dps"` не имеет зависимостей (`p56.a` = пустой список) — запускается параллельно с остальными.

---

## 3. Триггер отправки

```
App comes to foreground (first Activity started)
  → a0l.onActivityStarted() [ActivityLifecycleCallbacks]
    → posts hwf(12, a0l) to main Handler
      → a0l.b.invoke() = o2(16, zu5)
        → zu5.d(zu5) → zu5.E0()
          → executor.execute(n6(11, zu5))
            → zu5.D0(zu5) — загрузка конфига + запуск проверок
```

**Частота:** каждый раз при переходе из background в foreground (первый `onActivityStarted` после `onActivityStopped` всех Activity).

**Дополнительное ограничение:** `sampleRate` (по умолчанию 1.0 = 100%) — `m7f.b() >= bzkVar.h` пропускает если random >= sampleRate.

---

## 4. Pipeline выполнения

### Шаг 1: Загрузка конфига

`zu5.D0()`:
1. Читает кэшированный конфиг из `dps_config.bin`
2. Читает `dontReportUntil` из `dps_dont_report.bin`
3. Проверяет sampleRate — если random >= rate, пропускает
4. Иначе вызывает `g0(config)`

### Шаг 2: Определение публичного IP

`zu5.g0()`:
1. Берёт список `selfIpDetectionDomains` из конфига
2. Перемешивает (shuffle)
3. Последовательно пробует каждый URL, пока не получит IP
4. Парсит ответ regex'ом: `\b(?:[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}|[0-9a-fA-F:]+:[0-9a-fA-F:]+)\b`
5. Если IP = `127.0.0.1` — пропускает (localhost detection)

**Дефолтные IP-detection домены (hardcoded):**
```
https://ipv4-internet.yandex.net/api/v0/ip
https://ipv6-internet.yandex.net/api/v0/ip
https://ifconfig.me/ip
https://api.ipify.org
https://checkip.amazonaws.com
https://ip.mail.ru/
```

### Шаг 3: Проверки доступности (параллельно)

Для каждого хоста из `reachabilityHosts` запускаются 2-3 проверки:

| Тип (wzk.a) | Проверка | Результат (byte) |
|---|---|---|
| 0 = DNS | `InetAddress.getAllByName(host)` | 1 = resolved, 0 = failed |
| 1 = TCP | `Socket.connect(host:443, timeout)` | 2 = connected, 0 = failed |
| 2 = TLS | `SSLSocket.connect() + startHandshake()` | 4 = handshake ok, 0 = failed |

TLS-проверка (тип 2) включается только если `tlsCheckEnabled = true` (HIGH performance class).

**Дефолтный reachabilityHost:** `api.oneme.ru` (id=1)

### Шаг 4: Сбор метаданных

При каждой проверке собирается:
- `qzk.b()` → тип сети (0=unknown, 1=no_conn, 2=wifi, 3=2G, 4=3G, 5=4G+)
- `qzk.c()` → VPN-статус (проверка NetworkCapabilities + интерфейсов tun/ppp/tap/ipsec)
- `TelephonyManager.getNetworkOperator() + ":" + getNetworkOperatorName()` → оператор (e.g. "25001:MTS")

### Шаг 5: Запись в лог

Результаты записываются в `v0l` (файл `dps_log_entries.bin`) как `q0l` записи.

### Шаг 6: Отправка на сервер

`zu5.h0()` → `kyk.a()`:
1. Проверяет `dontReportUntil` — если текущее время < порога, пропускает
2. Фильтрует записи по `snapshotLifetimeMs` (по умолчанию 86400000 = 24 часа)
3. Ограничивает количество `maxSnapshots` (по умолчанию 50)
4. Формирует JSON и отправляет POST на `reportHosts`

---

## 5. Точный JSON-формат snapshot'а

### URL запроса

```
POST https://trace-flow.ru/api/v1/report?ver=<configVersion>
```

### HTTP-заголовки

```
Content-Type: application/json
User-Agent: application/json
Content-Encoding: gzip
Authorization: <apiKey>
Accept: application/json
```

### Тело запроса (JSON)

```json
{
  "snapshots": [
    {
      "id": "<UUID>",
      "clientTs": <unix_timestamp_seconds>,
      "appVersion": "<clientVersion>",
      "ip": "<detected_public_ip>",
      "connectionType": <network_type_int>,
      "operator": "<mcc_mnc:operator_name>",
      "vpn": <boolean>,
      "deviceId": "<device_id>",
      "uid": "<user_id>",
      "hosts": [
        {
          "id": <host_id_int>,
          "status": <bitmask_byte>
        }
      ]
    }
  ]
}
```

### Поля snapshot'а — источники значений

| Поле | Тип | Источник | Пример |
|---|---|---|---|
| `id` | string | `UUID.randomUUID()` | `"a1b2c3d4-..."` |
| `clientTs` | long | `wallClock.now() / 1000` (секунды) | `1715900000` |
| `appVersion` | string | `ax3.a()` (clientVersion supplier) | `"26.15.3"` |
| `ip` | string | Ответ от IP-detection сервисов, **в чистом виде** | `"185.22.33.44"` |
| `connectionType` | int | `qzk.b()` — тип сети | `2` (WiFi) |
| `operator` | string | `TelephonyManager.getNetworkOperator():getName()` | `"25001:MTS"` |
| `vpn` | boolean | `qzk.c()` — VPN detected | `true` |
| `deviceId` | string | `sm5.a()` (deviceIdSupplier) | `"abc123..."` |
| `uid` | string | `kgj.getUserId()` (userIdSupplier) | `"12345678"` |
| `hosts[].id` | int | `d1l.a` — ID хоста из конфига | `1` |
| `hosts[].status` | byte | Битовая маска: DNS(1) \| TCP(2) \| TLS(4) | `7` (все ок) |

### Значения status bitmask

```
0 = все проверки failed
1 = DNS resolved
2 = TCP connected
3 = DNS + TCP
4 = TLS handshake ok
5 = DNS + TLS
6 = TCP + TLS
7 = DNS + TCP + TLS
```

---

## 6. Ответ сервера

```json
{
  "config": {
    "reportHosts": ["https://trace-flow.ru"],
    "selfIpDetectionDomains": ["https://ipv4-internet.yandex.net/api/v0/ip", ...],
    "reachabilityHosts": [{"id": 1, "host": "api.oneme.ru"}],
    "timeoutMs": 10000,
    "version": 0,
    "maxSnapshots": 50,
    "snapshotLifetimeMs": 86400000,
    "sampleRate": 1.0
  },
  "dontReportUntil": <seconds_from_now>
}
```

- HTTP 200 + `config` → обновляет локальный конфиг, очищает лог
- HTTP 200 без `config` → только очищает лог
- HTTP 429 → `close()` (отключает SDK до перезапуска)
- HTTP 4xx → игнорирует
- HTTP 5xx → игнорирует

`dontReportUntil` — сервер может заглушить клиент на N секунд (throttling).

---

## 7. Серверные хосты

### Hardcoded (дефолтные, из `hph` case 12):

| Назначение | URL |
|---|---|
| **Report endpoint** | `https://trace-flow.ru` → `/api/v1/report?ver=N` |
| IP detection 1 | `https://ipv4-internet.yandex.net/api/v0/ip` |
| IP detection 2 | `https://ipv6-internet.yandex.net/api/v0/ip` |
| IP detection 3 | `https://ifconfig.me/ip` |
| IP detection 4 | `https://api.ipify.org` |
| IP detection 5 | `https://checkip.amazonaws.com` |
| IP detection 6 | `https://ip.mail.ru/` |
| **Reachability target** | `api.oneme.ru` (id=1, port 443) |

### Серверно-обновляемые:

Все списки (`reportHosts`, `selfIpDetectionDomains`, `reachabilityHosts`) могут быть **заменены сервером** через ответ на report. Это значит:
- Сервер может добавить **любые** хосты для проверки доступности
- Сервер может перенаправить отчёты на другой endpoint
- Сервер может добавить другие IP-detection сервисы

---

## 8. Файлы кэша

### `dps_config.bin`

Бинарный формат (DataOutputStream):
```
int    version (e field)
int    timeoutMs (d field)
int    maxSnapshots (f field)
long   snapshotLifetimeMs (g field)
float  sampleRate (h field)
int    reportHosts.size
  UTF  reportHosts[i]
int    selfIpDetectionDomains.size
  UTF  selfIpDetectionDomains[i]
int    reachabilityHosts.size
  int  reachabilityHosts[i].id
  UTF  reachabilityHosts[i].host
```

**Инвалидация:** перезаписывается при получении нового конфига в ответе сервера.

### `dps_dont_report.bin`

```
long   dontReportUntilTimestamp (millis)
```

**Инвалидация:** перезаписывается при каждом успешном report.

### `dps_log_entries.bin`

Append-only бинарный лог:
```
[repeat]:
  int    entryLength
  bytes  entry:
    UTF    id (UUID)
    long   configTimestamp
    long   checkTimestamp
    UTF    operator
    int    connectionType
    UTF    detectedIp
    boolean vpnDetected
    int    checksCount
    [repeat]:
      int  checkTypeId
      byte statusBitmask
```

**Инвалидация:** удаляется целиком после успешной отправки (HTTP 200). При ошибке чтения — тоже удаляется.

---

## 9. Вызывается ли до логина?

**НЕТ.** DPS инициализируется в `AccountInitializer.a()`, который вызывается только после успешной авторизации. Это подтверждается:
1. `AccountInitializer` получает `userId` через `account.getUserId()`
2. Задача "Dps" зарегистрирована в `AccountInitializer.e()` (post-login init graph)
3. ContentProvider `enabled="false"` — pre-login авто-инициализация отключена

Однако: userId и deviceId передаются через supplier'ы — теоретически могут быть null/empty если supplier вызван до полной инициализации аккаунта.

---

## 10. VPN-детекция (qzk)

Два метода:

### Метод 1: NetworkCapabilities API (если есть ACCESS_NETWORK_STATE)
```java
// Проверяет NET_CAPABILITY_NOT_VPN через reflection
NetworkCapabilities.hasCapability(15) // NET_CAPABILITY_NOT_VPN = 15
// false = VPN detected
```

### Метод 2: NetworkInterface enumeration (fallback)
```java
// Перебирает сетевые интерфейсы, ищет:
// - "tun" (OpenVPN, WireGuard)
// - "ppp" (PPTP, L2TP)
// - "tap" (OpenVPN tap mode)
// - "ipsec" (IPSec VPN)
```

---

## 11. Связь с PmsKey "host-reachability"

В `AccountInitializer.e()` есть отдельная задача:
```java
zukVar.c("HostReachabilityTask", p56Var, new c6(this, 17));
```

И в `o2` case 21:
```java
new rc6(lkc.g(((i58) obj).a, "host-reachability", 0, 2, true, false, 1, 2));
```

PmsKey `"host-reachability"` (ordinal 227) — это **отдельная** система от DPS, но обе проверяют доступность серверов. DPS дополнительно собирает IP + VPN + оператора.

---

## 12. Проверка в 26.16.0

| Артефакт | Статус в 26.16.0 |
|---|---|
| `DpsInitProvider` в manifest | ✅ Присутствует (`enabled="false"`) |
| `DpsInitProvider.smali` | ✅ Присутствует в `smali_classes3/ru/trace_flow/` |
| Обфусцированная строка `https://trace-flow.ru` | ✅ Присутствует в `smali/awg.smali` |
| PmsKey `dps` | ✅ Присутствует (проверено в diff) |

**Вывод:** DPS SDK полностью сохранён в 26.16.0 без изменений.

---

## 13. Выводы для статьи

### Что это

DPS — это **система сетевого фингерпринтинга**, которая:
- Определяет реальный IP пользователя (обходя VPN на уровне определения)
- Фиксирует использование VPN
- Привязывает сетевой фингерпринт к userId + deviceId
- Проверяет доступность серверов MAX с устройства пользователя
- Отправляет всё на отдельный сервер `trace-flow.ru`

### Зачем (вероятные цели)

1. **Обнаружение VPN/прокси** — для блокировки функций (ср. `WebAppHasVpnException`)
2. **Геолокация по IP** — определение реального местоположения даже при VPN
3. **Мониторинг блокировок** — если `api.oneme.ru` недоступен, сервер узнает об этом
4. **Фингерпринтинг** — связка IP + оператор + deviceId = уникальный идентификатор

### Красные флаги

1. **IP в чистом виде** — не хеш, не анонимизированный, а реальный публичный IP
2. **Серверный контроль** — сервер может добавить ЛЮБЫЕ хосты для проверки
3. **Привязка к userId** — это не анонимная телеметрия
4. **Отдельный домен** — `trace-flow.ru` не упоминается в Privacy Policy MAX
5. **Гейтится сервером** — можно включить/выключить для конкретных пользователей
6. **Обфускация** — все строки зашифрованы через `z8f.a()`, что затрудняет аудит

---

## 14. API Key и идентификация

```
API Key: ply5hDvhupghrHVA5rqQD1ypiXAxbmE4A68ZzBa8ioc=
```

Передаётся в заголовке `Authorization`. Это Base64-encoded 32-byte ключ, вероятно используется для идентификации приложения на стороне trace-flow.ru.

---

## 15. Полная цепочка вызовов

```
[Server] PmsKey "dps" = true
  → AccountInitializer.a() создаёт zu5 instance
    → zu5() конструктор регистрирует a0l (ActivityLifecycleCallbacks)

[User opens app / returns from background]
  → a0l.onActivityStarted() (d == 1, first activity)
    → Handler.post(hwf(12))
      → o2(16, zu5).invoke()
        → zu5.E0()
          → compareAndSet(false, true) — однократный запуск
          → executor.execute(n6(11, zu5))

[Background thread]
  → zu5.D0()
    → kzk.b() — читает dps_config.bin
    → kzk.d() — читает dps_dont_report.bin
    → m7f.b() < sampleRate? → zu5.g0(config)

  → zu5.g0(config)
    → y0l.a() — проверка интернета
    → Fetch IP from selfIpDetectionDomains (shuffle, try each)
    → Parse IP with regex
    → For each reachabilityHost × checkType:
        → executor.execute(tu5 → zu5.J())
          → wzk.a(timeout, host) — DNS/TCP/TLS check
          → v0l.d(logEntry) — записать результат
    → When all checks done (atomicInteger == 0):
        → zu5.h0(mergedEntries, config)

  → zu5.h0()
    → Filter by snapshotLifetimeMs
    → Limit to maxSnapshots
    → Build n1l snapshots with userId, deviceId, version
    → kyk.a(reportHost, snapshots, configVersion)

  → kyk.a()
    → POST JSON to https://trace-flow.ru/api/v1/report?ver=N
    → Parse response → update config cache / clear log
```
