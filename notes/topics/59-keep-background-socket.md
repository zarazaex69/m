---
tags: [network, background, server-control, pms, ws]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/jn0.java
  - work/jadx_base/sources/defpackage/hod.java
  - work/jadx_base/sources/defpackage/om5.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[08-background-wake]]"
  - "[[03-pms-server-flags]]"
  - "[[33-ws-msgpack-framing]]"
---

# keep-background-socket и ping-background-interval — фоновый WS

Два PmsKey управляют поведением WS-соединения в фоне.

## PmsKey

| Ключ | # | Что |
|---|---|---|
| `keep-background-socket` | (в qp6) | держать WS-соединение открытым когда приложение в фоне |
| `ping-background-interval` | (в qp6) | интервал ping-пакетов в фоне (мс) |
| `keep-connection` | 29 | общий keep-alive WS |
| `disconnect-timeout` | 33 | таймаут до разрыва соединения |
| `subscription-timeout-seconds` | 63 | таймаут подписки |

## Механизм

`jn0.java` — обработчик изменения PmsKey `keep-background-socket`. При изменении логирует «PMS keepBackgroundSocket changed: ...». Если PmsKey отключён (`!(hn0Var instanceof gn0)`) и фича активна — принудительно отключает её (`this.X.g(false)`).

`hod.java` — планировщик ping-пакетов. Когда приложение не интерактивно (`!isInteractive`), но `pingBackgroundInterval > 0` — планирует ping с интервалом `pingBackgroundInterval`. Логирует «schedulePing: app is not interactive, but pingBackgroundInterval = N».

## Что важно

1. **`keep-background-socket`** — сервер может включить постоянное WS-соединение в фоне. Это означает, что MAX держит открытый канал к серверу даже когда пользователь не использует приложение. В сочетании с [[08-background-wake]] (wake on boot) — MAX всегда подключён.

2. **`ping-background-interval`** — сервер задаёт, как часто клиент пингует сервер в фоне. Это не только keep-alive, но и сигнал «я онлайн» для сервера. Сервер знает, что устройство активно, даже если пользователь не открывал приложение.

3. **Связь с presence**: `ping-background-interval` напрямую влияет на presence-статус (см. [[32-presence-server-controlled]]). Частые пинги = сервер видит пользователя как «онлайн» даже в фоне.

## Сводка

`keep-background-socket` и `ping-background-interval` — server-controlled параметры фонового WS. Сервер может держать MAX постоянно подключённым и пингующим с заданным интервалом, что обеспечивает постоянный канал связи и presence-сигнал даже когда приложение в фоне.
