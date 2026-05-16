---
tags: [network, tls, ssl, server-control, pms, security]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ezd.java
  - work/jadx_base/sources/defpackage/ubi.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[03-pms-server-flags]]"
  - "[[04-telemetry-endpoints]]"
  - "[[17-apptracer-uplink]]"
---

# Network session PmsKey — server-controlled TLS validation и battery mode

Из `ezd.java` (dev-menu описания) и `ubi.java` (WS-конфиг):

## PmsKey сетевой сессии

| Ключ | # | Default | Описание из кода |
|---|---|---|---|
| `net-ssl-session-validate` | (в rtd) | **true** | «Validate server ssl session» |
| `net-session-suppress-bad-disconnected-state` | (в rtd) | false | «No bad disconnected state in session» |
| `net-session-rbc-enabled` | (в rtd) | false | «Reduce battery consumption in session» |
| `net-client-dns-enabled` | (в rtd) | — | кастомный DNS-клиент |
| `spin-lock-enabled` | (в rtd) | false | «Enable SpinLock in concurrency» |
| `watchdog-config` | (в om5) | — | конфиг watchdog-а приложения |

## Что важно

1. **`net-ssl-session-validate=true` по умолчанию** — это нормально. Но сервер может установить `net-ssl-session-validate=false` через PmsKey, что **отключит валидацию SSL-сессии**. Это означает, что сервер может удалённо отключить TLS-проверку для конкретного пользователя. Это критически важный флаг: при `false` клиент перестаёт проверять сертификат сервера.

2. **`net-ssl-session-validate` передаётся в WS-конфиг** (`ubi.java:64` — `u6d("net-ssl-session-validate", String.valueOf(...))`). Это значит, что текущее значение флага отправляется на сервер как часть диагностической информации о сессии. Сервер знает, включена ли валидация у конкретного клиента.

3. **`net-session-rbc-enabled`** — «Reduce battery consumption» в сетевой сессии. Вероятно, управляет агрессивностью keep-alive и polling. Default `false`.

4. **`net-client-dns-enabled`** — кастомный DNS-клиент. Если включён, клиент использует собственный DNS-резолвер вместо системного. Это может быть DoH (DNS over HTTPS) или просто кастомный resolver с hardcoded серверами.

5. **`watchdog-config`** — JSON-конфиг watchdog-а приложения. Watchdog следит за зависаниями (ANR). Конфиг задаётся сервером — сервер контролирует, при каких условиях watchdog срабатывает и что делает (crash report, restart, etc.).

## Скептический разбор

- `net-ssl-session-validate=false` — это **красный флаг**. Если сервер может отключить TLS-валидацию, это означает возможность MITM-атаки с ведома сервера. Нужно проверить, есть ли certificate pinning поверх этого флага.
- Нет подтверждения, что certificate pinning реализован в MAX. В `libtracernative.so` есть `tracer_set_ssl_cainfo` (см. [[17-apptracer-uplink]]), но это для apptracer, не для основного WS.
- `net-ssl-session-validate` может относиться к session resumption (TLS session tickets), а не к certificate validation. Это нужно уточнить через анализ нативного сетевого кода.

## Сводка

`net-ssl-session-validate` (default=true) — server-controlled флаг валидации TLS-сессии. Сервер может его отключить. Текущее значение отправляется на сервер в диагностике. `watchdog-config` — server-pushed конфиг watchdog-а. `net-client-dns-enabled` — кастомный DNS-клиент.
