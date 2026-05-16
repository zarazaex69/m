---
tags: [calls, remote-settings, server-control, bitrate-dump, kws, p2p]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/api/RemoteSettings.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/ConversationImpl.java
  - work/jadx_base/sources/defpackage/av1.java
related:
  - "[[16-server-pushed-ml-models-in-calls]]"
  - "[[45-calls-sdk-pmskey-cluster]]"
  - "[[43-libjingle-webrtc-custom-build]]"
---

# RemoteSettings — server-pushed конфиги звонкового SDK

`ru.ok.android.externcalls.sdk.api.RemoteSettings` — интерфейс для server-pushed конфигов звонкового SDK. Полный список ключей:

| Ключ | Что |
|---|---|
| `android.dump.bitrate` | конфиг сбора bitrate dump во время звонков |
| `android.mlfeatures.%s` | ML-фичи (KWS, NS — см. [[16-server-pushed-ml-models-in-calls]]) |
| `android.rating.limits` | лимиты оценки качества звонка |
| `android.wordspotter.config` | конфиг Word Spotter (KWS) |
| `android.p2prelay.config` | конфиг P2P relay |
| KwsFeatureConfigProvider.remoteKey | ключ конфига KWS |
| NSFeatureConfigProvider.remoteKey | ключ конфига Noise Suppression |

## android.dump.bitrate — дамп bitrate во время звонков

`BitrateDumpGatheringConfig(isEnabled=...)` — конфиг сбора bitrate dump. При включении клиент записывает bitrate данные в файл `target_bitrate_dump_<timestamp>` в `filesDir`.

`ConversationImpl.java:1371` — `n01(remoteSettingsImplV2, a7fVar, "android.dump.bitrate", "BitrateDumpGatheringConfigProviderImpl")` — провайдер конфига получает его из `RemoteSettings`.

Это означает: **сервер может включить запись bitrate dump во время звонков**. Файл сохраняется локально и, вероятно, отправляется на сервер через apptracer `/api/sample/upload` (см. [[17-apptracer-uplink]]).

## android.rating.limits

Лимиты оценки качества звонка — сервер задаёт пороги, при которых звонок считается «плохим» по качеству. Это влияет на то, когда пользователю показывается запрос оценки звонка.

## android.p2prelay.config

Конфиг P2P relay — сервер задаёт параметры использования relay-серверов для P2P-звонков. Это дополнительный механизм управления маршрутизацией звонков (в дополнение к `calls-endpoint` из [[64-calls-endpoint-server-redirect]]).

## Сводка

`RemoteSettings` — 7 server-pushed конфигов для звонкового SDK: bitrate dump (запись данных о bitrate в файл), ML-фичи (KWS/NS), rating limits, wordspotter, P2P relay. Все управляются сервером через тот же механизм, что и ML-модели (см. [[16-server-pushed-ml-models-in-calls]]).
