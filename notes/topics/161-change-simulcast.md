---
tags: [calls, simulcast, video, server-control, signaling]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/qyf.java
related:
  - "[[153-signaling-commands]]"
  - "[[130-calls-audio-pipeline-simulcast]]"
---

# change-simulcast — изменение simulcast параметров видео

`change-simulcast` — сигналинг-команда для изменения simulcast параметров.

## Структура

```json
{
  "command": "change-simulcast",
  "mediaSource": "CAMERA",
  "layers": [
    {
      "rid": "...",
      "width": int,
      "height": int,
      "fps": int,
      "bitrateKbps": int
    }
  ]
}
```

## Что важно

1. **`mediaSource = "CAMERA"`** — команда применяется к камере.

2. **`layers`** — список слоёв simulcast. Каждый слой имеет `rid`, `width`, `height`, `fps`, `bitrateKbps`. Это позволяет передавать несколько потоков разного качества одновременно.

3. **`bitrateKbps = mehVar.e / 1000`** — bitrate в kbps. Сервер может изменить bitrate каждого слоя.

4. Только активные слои (`mehVar.c == true`) включаются в команду.

## Сводка

`change-simulcast` — изменение simulcast слоёв (width/height/fps/bitrateKbps) для камеры. Сервер может изменить качество видео-потоков во время звонка.
