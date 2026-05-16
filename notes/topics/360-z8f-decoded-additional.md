---
tags: [network, dps, ip-detection, host-reachability, surveillance, obfuscated-strings]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/z8f.java
related:
  - "[[359-z8f-decoded-strings]]"
  - "[[357-host-reachability-full]]"
---

# z8f.a — дополнительные декодированные строки

Дополнение к [[359-z8f-decoded-strings]].

## DPS (Data Processing Service) файлы

| Строка | Что |
|---|---|
| `dps/` | директория DPS |
| `dps_config.bin` | **конфигурация DPS** |
| `dps_dont_report.bin` | **список не-репортируемых** |
| `dps_log_entries.bin` | **лог-записи DPS** |

## UUID

`edef8ba9-79d6-4ace-a3c8-27dcd51d21ed` — UUID. Это стандартный UUID для Bluetooth Audio (A2DP).

## Host Reachability конфигурация

| Ключ | Что |
|---|---|
| `reachabilityHosts` | хосты для проверки |
| `reportHosts` | хосты для репортинга |
| `selfIpDetectionDomains` | домены для определения IP |
| `sampleRate` | частота сэмплирования |
| `snapshotLifetimeMs` | время жизни снапшота |
| `timeoutMs` | таймаут |
| `maxSnapshots` | максимум снапшотов |

## API запрос (HostReachability)

```json
{
  "id": "...",
  "appVersion": "...",
  "clientTs": "...",
  "connectionType": "...",
  "deviceId": "...",
  "hosts": [...],
  "ip": "...",
  "operator": "...",
  "status": "...",
  "uid": "...",
  "vpn": "..."
}
```

## Что важно

1. **DPS файлы** — `dps_config.bin`/`dps_dont_report.bin`/`dps_log_entries.bin`. Это файлы MyTracker DPS.

2. **HostReachability JSON** — полная структура запроса: appVersion/clientTs/connectionType/deviceId/hosts/ip/operator/status/uid/vpn.

3. **`selfIpDetectionDomains`** — конфигурируемый список доменов для определения IP.

## Сводка

DPS файлы: `dps_config.bin`/`dps_dont_report.bin`/`dps_log_entries.bin`. HostReachability JSON: appVersion/clientTs/connectionType/deviceId/hosts/ip/operator/status/uid/vpn.
