---
tags: [calls, media-settings, signaling, audio, video, screen-share, animoji, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/vbh.java
  - work/jadx_base/sources/defpackage/ivl.java
related:
  - "[[401-media-dump-manager]]"
  - "[[397-video-sdk-screen-camera]]"
  - "[[391-conversation-events-listener]]"
---

# change-media-settings — команда изменения медиа-настроек

`change-media-settings` — signaling-команда для изменения медиа-настроек участника звонка.

## Структура JSON

```json
{
  "command": "change-media-settings",
  "mediaSettings": {
    "isVideoEnabled": bool,
    "isAudioEnabled": bool,
    "isScreenSharingEnabled": bool,
    "isAnimojiEnabled": bool,
    "isFastScreenSharingEnabled": bool,  // опционально
    "isAudioSharingEnabled": bool         // опционально
  }
}
```

## Поля

| Поле | Что |
|---|---|
| `isVideoEnabled` | видео включено |
| `isAudioEnabled` | **аудио включено** |
| `isScreenSharingEnabled` | **захват экрана включён** |
| `isAnimojiEnabled` | анимоджи включено |
| `isFastScreenSharingEnabled` | быстрый screen share |
| `isAudioSharingEnabled` | **захват системного аудио** |

## Что важно

1. **`isAudioEnabled`** — команда может отключить аудио участника.

2. **`isScreenSharingEnabled`** — команда может включить/выключить захват экрана.

3. **`isAudioSharingEnabled`** — захват системного аудио при screen share.

4. **`command-discarded`** — если команда была объединена с другой, возвращается ошибка `command-discarded`.

## Сводка

`change-media-settings {isVideoEnabled, isAudioEnabled, isScreenSharingEnabled, isAnimojiEnabled, isFastScreenSharingEnabled, isAudioSharingEnabled}`.
