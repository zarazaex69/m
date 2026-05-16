---
tags: [calls, api, remote-settings, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/api/request/GetSettings.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/settings/RemoteSettingsShared.java
related:
  - "[[69-remote-settings-calls-sdk]]"
  - "[[96-vchat-api-methods]]"
---

# settings.get — API метод для получения RemoteSettings звонков

`GetSettings.METHOD_NAME = "settings.get"` — HTTP API метод для получения RemoteSettings звонкового SDK.

## Механизм

`RemoteSettingsShared` — реализация `RemoteSettings`, которая:
1. Вызывает `settings.get` API с набором ключей (`RemoteSettings.getKeys()`)
2. Кэширует результат
3. Периодически обновляет (через `keepSharedSettingsMs`)

Ключи запроса: `android.dump.bitrate`, `android.rating.limits`, `android.wordspotter.config`, `android.p2prelay.config`, `android.mlfeatures.ws_0`, `android.mlfeatures.ns_1`.

## Что важно

1. **`settings.get`** — отдельный API endpoint для получения конфигов звонкового SDK. Это не WS-протокол и не PmsKey — это отдельный HTTP API.

2. **Периодическое обновление** — `keepSharedSettingsMs` задаёт, как долго кэшировать настройки. После истечения — новый запрос к `settings.get`.

3. **Все 6 ключей** запрашиваются одним запросом. Сервер возвращает JSON-конфиги для каждого ключа.

## Сводка

`settings.get` — HTTP API для получения 6 RemoteSettings ключей звонкового SDK. Периодически обновляется. Отдельный канал от WS-протокола и PmsKey.
