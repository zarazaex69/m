---
tags: [network, ip-detection, host-reachability, trace-flow, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/hph.java
  - work/jadx_base/sources/defpackage/bzk.java
related:
  - "[[359-z8f-decoded-strings]]"
  - "[[357-host-reachability-full]]"
---

# HostReachability Config (bzk) — полная конфигурация

`bzk` — конфигурация HostReachability checker. Создаётся в `hph.java` case 12.

## Конструктор bzk(reportHosts, selfIpDomains, ..., timeout, sampleRate, maxSnapshots, snapshotLifetime, ...)

### reportHosts (list a)
- `https://trace-flow.ru` — **единственный хост для репортинга**

### selfIpDetectionDomains (list b)
- `https://ipv4-internet.yandex.net/api/v0/ip`
- `https://ipv6-internet.yandex.net/api/v0/ip`
- `https://ifconfig.me/ip`
- `https://api.ipify.org`
- `https://checkip.amazonaws.com`
- `https://ip.mail.ru/`

### Параметры
- `timeout`: 10000 мс
- `sampleRate`: 0 (?)
- `maxSnapshots`: 50
- `snapshotLifetime`: `BuildConfig.MAX_TIME_TO_UPLOAD`
- `rate`: 1.0f

## Что важно

1. **`https://trace-flow.ru`** — единственный хост для репортинга результатов проверки доступности. Это MyTracker DPS endpoint.

2. **6 IP-определяющих сервисов** — Яндекс (IPv4+IPv6), ifconfig.me, ipify, AWS, Mail.ru.

3. **`MAX_TIME_TO_UPLOAD`** — время жизни снапшота ограничено константой из BuildConfig.

## Сводка

`bzk`: reportHosts=[trace-flow.ru] + selfIpDomains=[yandex/ifconfig.me/ipify/aws/mail.ru] + timeout=10000 + maxSnapshots=50.
