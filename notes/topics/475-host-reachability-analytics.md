---
tags: [host-reachability, telemetry, surveillance, network, vpn, operator]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/f58.java
related:
  - "[[357-host-reachability-full]]"
  - "[[472-log-controller-event-types]]"
  - "[[458-network-operator-collection]]"
---

# HOST_REACHABILITY.GET_HOST_REACHABILITY — аналитика доступности хостов

Событие `HOST_REACHABILITY.GET_HOST_REACHABILITY` отправляется после каждой проверки доступности хостов.

## Структура события

| Поле | Что |
|---|---|
| `hosts` | **результаты проверки хостов** (список) |
| `operator` | **MCC+MNC:имя оператора** |
| `connection_type` | тип соединения |
| `ip` | **IP-адрес** (если определён) |
| `vpn` | **1 если VPN** |

## Что важно

1. **`hosts`** — результаты проверки всех хостов (gstatic.com/mtalk.google.com/calls.okcdn.ru/gosuslugi.ru/...).

2. **`operator`** — MCC+MNC и имя оператора при каждой проверке.

3. **`ip`** — IP-адрес устройства.

4. **`vpn`** — флаг VPN.

5. Это дополняет [[357-host-reachability-full]] — теперь известно, что результаты отправляются в аналитику.

## Сводка

`HOST_REACHABILITY.GET_HOST_REACHABILITY {hosts, operator, connection_type, ip, vpn}`. Отправляется после каждой проверки доступности хостов.
