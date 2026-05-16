---
tags: [network, dps, mytracker, metadata-keys, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/
related:
  - "[[360-z8f-decoded-additional]]"
  - "[[361-host-reachability-bzk-config]]"
---

# z8f.a — DPS метаданные и ключи

Дополнение к [[360-z8f-decoded-additional]]. Новые декодированные строки.

## DPS (MyTracker Data Processing Service)

| Строка | Что |
|---|---|
| `ru.trace_flow.dps.API_KEY` | **ключ API DPS** (метаданные AndroidManifest) |
| `ru.trace_flow.dps.CLIENT_VERSION` | **версия клиента DPS** |
| `ru.trace_flow.dps.USER_ID` | **ID пользователя DPS** |
| `dps_auto_init_enabled` | автоинициализация DPS |
| `dps_config.bin` | конфигурация |
| `dps_dont_report.bin` | список не-репортируемых |
| `dps_log_entries.bin` | лог-записи |

## HostReachability конфигурация

| Ключ | Что |
|---|---|
| `reachabilityHosts` | хосты для проверки |
| `reportHosts` | хосты для репортинга |
| `selfIpDetectionDomains` | домены для определения IP |
| `sampleRate` | частота сэмплирования |
| `snapshotLifetimeMs` | время жизни снапшота |
| `timeoutMs` | таймаут |
| `maxSnapshots` | максимум снапшотов |
| `dontReportUntil` | не репортировать до |

## Что важно

1. **`ru.trace_flow.dps.API_KEY`** — ключ API DPS хранится в метаданных AndroidManifest. Это MyTracker DPS API ключ.

2. **`ru.trace_flow.dps.USER_ID`** — ID пользователя DPS. Это идентификатор пользователя в MyTracker DPS.

3. **`dps_auto_init_enabled`** — флаг автоинициализации DPS.

## Сводка

DPS метаданные: `ru.trace_flow.dps.API_KEY`/`CLIENT_VERSION`/`USER_ID`. HostReachability: reachabilityHosts/reportHosts/selfIpDetectionDomains/sampleRate/snapshotLifetimeMs/timeoutMs/maxSnapshots.
