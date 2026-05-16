---
tags: [calls, bitrate-dump, server-control, webrtc]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/ConversationFactory.java
related:
  - "[[69-remote-settings-calls-sdk]]"
  - "[[136-ev1-webrtc-session-config]]"
---

# applyBitrateDumpGatheringConfig — server-controlled bitrate dump

`ConversationFactory.applyBitrateDumpGatheringConfig(ev1)` — применяет конфиг bitrate dump к WebRTC сессии.

## Логика

```
l01 = bitrateDumpGatheringConfigStorage.get("bitrate_config_key")
ev1.O = (l01 == null || !l01.a) ? null : new av1(context)
```

- Если `l01 == null` или `l01.a == false` — bitrate dump отключён (`ev1.O = null`)
- Если `l01.a == true` — создаётся `av1(context)` — файл для bitrate dump

`av1(context)` создаёт файл `target_bitrate_dump_<timestamp>` в `filesDir` (см. [[69-remote-settings-calls-sdk]]).

## Что важно

1. **`bitrate_config_key`** — ключ в локальном хранилище. Значение приходит с сервера через `android.dump.bitrate` RemoteSettings.

2. **`l01.a`** — bool `isEnabled`. Если сервер установил `isEnabled=true` через `android.dump.bitrate`, то при каждом звонке создаётся файл bitrate dump.

3. **`ev1.O`** — поле `bitrateDumpGatheringState` в WebRTC сессии (см. [[136-ev1-webrtc-session-config]]).

## Сводка

`applyBitrateDumpGatheringConfig` — применяет server-controlled bitrate dump к WebRTC сессии. При `android.dump.bitrate.isEnabled=true` — создаётся файл `target_bitrate_dump_<timestamp>` для каждого звонка.
