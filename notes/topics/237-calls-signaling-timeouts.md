---
tags: [calls, signaling, timeouts, server-control, reconnect]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/gb2.java
related:
  - "[[235-qp6-additional-pmskeys]]"
  - "[[57-ws-session-config-fingerprint]]"
---

# CallsSignalingTimeouts — серверные таймауты сигналинга

`CallsSignalingTimeouts` — серверно-управляемые таймауты сигналинга звонков.

## Поля (значения по умолчанию)

| Поле | Значение по умолчанию | Что |
|---|---|---|
| `enabled` | false | включены ли кастомные таймауты |
| `connectTimeout` | 5000 мс | таймаут подключения |
| `initialReconnectDelay` | 2000 мс | начальная задержка переподключения |
| `reconnectDelayScaleFactor` | 1.0 | множитель задержки |
| `maxReconnectDelay` | 2000 мс | максимальная задержка |

## Что важно

1. **`enabled=false`** по умолчанию — кастомные таймауты отключены. Сервер может включить через PmsKey `callsSignalingTimeouts`.

2. **`reconnectDelayScaleFactor`** — экспоненциальный backoff для переподключения. Сервер контролирует агрессивность переподключения.

3. Все значения задаются сервером через PmsKey.

## Сводка

`CallsSignalingTimeouts`: enabled/connectTimeout(5s)/initialReconnectDelay(2s)/reconnectDelayScaleFactor(1.0)/maxReconnectDelay(2s). Серверно-управляемые.
