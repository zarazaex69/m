---
tags: [calls, media-modifiers, denoise, server-control, signaling]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ubh.java
  - work/jadx_base/sources/defpackage/z91.java
related:
  - "[[153-signaling-commands]]"
  - "[[158-change-media-settings]]"
---

# update-media-modifiers — шумоподавление через сигналинг

`update-media-modifiers` — сигналинг-команда для обновления медиа-модификаторов.

## Структура

```json
{
  "command": "update-media-modifiers",
  "mediaModifiers": {
    "denoise": bool,
    "denoiseAnn": bool
  }
}
```

## Поля

| Поле | Что |
|---|---|
| `denoise` | включить шумоподавление |
| `denoiseAnn` | включить ANN (нейросетевое) шумоподавление |

## Что важно

1. **`denoise`** — стандартное шумоподавление. Это отдельно от `calls-android-ns` PmsKey (см. [[45-calls-sdk-pmskey-cluster]]).

2. **`denoiseAnn`** — нейросетевое шумоподавление (ANN = Artificial Neural Network). Это более продвинутый алгоритм.

3. Команда отправляется через сигналинг — это означает, что шумоподавление может быть включено/выключено **во время звонка** без перезапуска.

4. `z91.java:289` — `handleMediaModifiers` — обработчик входящих `mediaModifiers` от сервера. Это означает, что **сервер может принудительно изменить настройки шумоподавления** участника.

## Сводка

`update-media-modifiers` — сигналинг-команда с `denoise` и `denoiseAnn`. Сервер может принудительно изменить настройки шумоподавления участника во время звонка.
