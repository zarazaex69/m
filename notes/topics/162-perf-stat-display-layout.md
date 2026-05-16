---
tags: [calls, signaling, perf-stat, display-layout, video]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/qyf.java
related:
  - "[[153-signaling-commands]]"
  - "[[95-calls-stats-40-metrics]]"
---

# report-perf-stat и update-display-layout — детали сигналинг-команд

## report-perf-stat

```json
{
  "command": "report-perf-stat",
  "report": {
    "framesReceived": int,
    "framesDecoded": int
  }
}
```

Клиент периодически отправляет статистику декодирования видео: `framesReceived` и `framesDecoded`. Сервер знает, сколько кадров получено и декодировано.

## update-display-layout

```json
{
  "command": "update-display-layout",
  "layouts": {
    "<participantId>": "ss" | "sz=WxH:fit=<mode>"
  },
  "snapshot": bool
}
```

Клиент сообщает серверу, как отображаются участники:
- `"ss"` — screenshare
- `"sz=WxH:fit=<mode>"` — размер и режим fit (например, `sz=640x480:fit=contain`)

`snapshot=true` — снимок текущего layout.

## Что важно

1. **`update-display-layout`** — клиент сообщает серверу, как именно отображается каждый участник (размер, режим). Сервер знает layout UI клиента.

2. **`report-perf-stat`** — статистика декодирования видео. Сервер знает, сколько кадров клиент успевает декодировать.

3. **`snapshot=true`** — полный снимок текущего layout. Это может использоваться для диагностики или аналитики.

## Сводка

`report-perf-stat` — framesReceived/framesDecoded. `update-display-layout` — layout участников (размер, режим, screenshare). Сервер знает UI layout клиента.
