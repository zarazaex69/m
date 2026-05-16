---
tags: [calls, server-control, pms, signaling, telecom]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/kd1.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[45-calls-sdk-pmskey-cluster]]"
  - "[[64-calls-endpoint-server-redirect]]"
---

# Calls signaling PmsKey — таймауты и URI

| Ключ | # | Default | Описание |
|---|---|---|---|
| `early-call-start` | 101 | false | «Ранний старт входящего звонка» |
| `outgoing-call-uri` | 223 | `https://max.ru` | «URI для исходящего звонка (Telecom)» |
| `calls-android-signaling-to` | 124 | JSON | «Таймауты сигналинга» |

## calls-android-signaling-to default

```json
{"use": false, "cto": 5000, "ird": 2000, "rdsf": 1.0, "mrd": 2000}
```

- `use` — использовать ли кастомные таймауты
- `cto` — connection timeout (5000ms)
- `ird` — initial retry delay (2000ms)
- `rdsf` — retry delay scale factor (1.0)
- `mrd` — max retry delay (2000ms)

## outgoing-call-uri

`outgoing-call-uri` default `https://max.ru` — URI для Android Telecom API при исходящем звонке. Это URI, который передаётся в Android Telecom для идентификации звонка. Сервер может изменить этот URI.

## early-call-start

«Ранний старт входящего звонка» — при включении клиент начинает подготовку к звонку до полного установления соединения. Это оптимизация задержки.

## Сводка

`outgoing-call-uri` (default=`https://max.ru`) — URI для Android Telecom. `calls-android-signaling-to` — JSON с таймаутами сигналинга. `early-call-start` — оптимизация задержки входящего звонка.
