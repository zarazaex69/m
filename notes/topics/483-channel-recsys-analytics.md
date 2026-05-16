---
tags: [channel-recsys, telemetry, surveillance, channel-folder, recommendation]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/m57.java
  - work/jadx_base/sources/defpackage/db0.java
related:
  - "[[472-log-controller-event-types]]"
---

# CHANNEL_RECSYS_FOLDER — аналитика рекомендательных папок каналов

## channel_folder_delete

Логируется при удалении рекомендательной папки каналов. Параметры: null.

Условие: `qp6.G()` — включён флаг.

## channel_folder_open

```json
{
  "channels_shown": [список каналов]
}
```

Логируется при открытии рекомендательной папки. Содержит список показанных каналов.

## Что важно

1. **`channels_shown`** — список каналов, показанных пользователю. Сервер знает, какие каналы видел пользователь.

2. **Рекомендательная система** — MAX имеет систему рекомендаций каналов (RECSYS).

3. Аналитика открытия/удаления папок с рекомендованными каналами.

## Сводка

`CHANNEL_RECSYS_FOLDER.channel_folder_open {channels_shown: [...]}`. `CHANNEL_RECSYS_FOLDER.channel_folder_delete`.
