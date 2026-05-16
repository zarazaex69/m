---
tags: [calls-net, download, network-stat, rtt, packet-loss, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/net/DownloadService.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/net/NetworkConnectionManager.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/net/FileValidationConfig.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/net/internal/monitor/NetworkStat.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/net/internal/monitor/StatMonitor.java
related:
  - "[[380-ml-features-manager]]"
  - "[[378-p2prelay-sessionroom]]"
  - "[[382-conversation-stats]]"
---

# externcalls.sdk.net — сетевой слой SDK

## DownloadService

Загрузка файлов (ML-моделей) с сервера.

| Метод | Что |
|---|---|
| `download(url, dest, FileValidationConfig)` | загрузить файл с проверкой MD5 |

Реализация: `HttpURLConnection` → `getInputStream()` → MD5 checksum → сравнение с `expectedChecksum`. При несовпадении: `RuntimeException("Downloaded model is corrupted")`.

Валидация URL: `Patterns.WEB_URL.matcher(url).matches()`.

## FileValidationConfig

| Поле | Что |
|---|---|
| `expectedChecksum` | ожидаемый MD5 |
| `hashAlgorithm` | алгоритм хэша (MD5) |

## NetworkConnectionManager

| Метод | Что |
|---|---|
| `addNetworkConnectivityListener(listener)` | подписаться на изменения сети |
| `registerBadConnectionCallback(callback)` | callback при плохом соединении |
| `plusAssign(listener)` / `minusAssign(listener)` | Kotlin-операторы |

## NetworkStat

Метрики сети в реальном времени.

| Поле | Что |
|---|---|
| `rttMs` | **RTT в миллисекундах** |
| `audioLoss` | **потери аудио-пакетов** |
| `videoLoss` | **потери видео-пакетов** |
| `activeCandidateType` | тип активного ICE candidate |

## StatMonitor

`observeStat()` → поток `NetworkStat`. Используется `P2pRelaySwitchTrigger` для мониторинга RTT.

## Что важно

1. **`audioLoss` + `videoLoss`** — потери пакетов мониторятся в реальном времени. Используются для принятия решения о переключении на relay.

2. **`activeCandidateType`** — тип ICE candidate (host/srflx/relay). Сервер знает, через что идёт трафик.

3. **MD5 checksum** — проверка целостности загруженных ML-моделей.

## Сводка

`DownloadService`: HTTP download + MD5 validation. `NetworkStat(rttMs, audioLoss, videoLoss, activeCandidateType)`. `StatMonitor.observeStat()` → поток NetworkStat для P2P relay trigger.
