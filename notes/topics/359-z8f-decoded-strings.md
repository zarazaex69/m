---
tags: [network, ip-detection, external-services, surveillance, obfuscated-strings]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/z8f.java
related:
  - "[[357-host-reachability-full]]"
  - "[[04-telemetry-endpoints]]"
---

# z8f.a — декодированные обфусцированные строки

`z8f.a()` — декодер обфусцированных строк (XOR + lookup table). Декодированные значения:

## IP-определение

| URL | Что |
|---|---|
| `https://api.ipify.org` | **внешний IP** |
| `https://checkip.amazonaws.com` | **внешний IP (AWS)** |
| `https://ifconfig.me/ip` | **внешний IP** |
| `https://ip.mail.ru/` | **внешний IP (Mail.ru)** |
| `https://ipv4-internet.yandex.net/api/v0/ip` | **IPv4 (Яндекс)** |
| `https://ipv6-internet.yandex.net/api/v0/ip` | **IPv6 (Яндекс)** |

## Другие

| Строка | Что |
|---|---|
| `https://trace-flow.ru` | **trace-flow.ru** |
| `/api/v1/report` | API endpoint |
| `api.oneme.ru` | основной API |
| `9774d56d682e549c` | **Android ID** (константа) |
| `127.0.0.1` | localhost |

## Что важно

1. **6 IP-определяющих сервисов** — приложение определяет внешний IP через несколько сервисов: ipify, AWS, ifconfig.me, Mail.ru, Яндекс (IPv4 и IPv6).

2. **`https://trace-flow.ru`** — неизвестный сервис. Возможно, трассировка маршрутов.

3. **`9774d56d682e549c`** — это стандартный Android ID для эмуляторов.

4. Строки обфусцированы XOR-шифрованием с lookup table.

## Сводка

IP-определение: `api.ipify.org`/`checkip.amazonaws.com`/`ifconfig.me`/`ip.mail.ru`/`ipv4-internet.yandex.net`/`ipv6-internet.yandex.net`. Также `trace-flow.ru`.
