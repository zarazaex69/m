---
tags: [calls, media-settings, server-control, signaling, force-mute]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/bob.java
  - work/jadx_base/sources/defpackage/vbh.java
  - work/jadx_base/sources/defpackage/z91.java
related:
  - "[[153-signaling-commands]]"
  - "[[156-microphone-manager]]"
---

# change-media-settings — принудительное изменение медиа-настроек

`change-media-settings` — сигналинг-команда для изменения медиа-настроек участника.

## MediaSettings (bob.java)

| Поле | Что |
|---|---|
| `e` | audio |
| `f` | video |
| `b` | screen capture |
| `g` | animoji |
| `c`, `d` | дополнительные флаги |

## handleForceChangeMediaSettings

`z91.java:494` — `handleForceChangeMediaSettings` — **принудительное** изменение медиа-настроек. Это означает, что сервер или администратор может принудительно изменить медиа-настройки участника (например, принудительно выключить камеру или микрофон).

## Что важно

1. **`handleForceChangeMediaSettings`** — принудительное изменение. Это отдельно от `mute-participant` (см. [[153-signaling-commands]]). Это более широкий механизм: можно принудительно выключить камеру, screen capture, animoji.

2. **`handleMediaSettingsChanged`** — обычное изменение медиа-настроек (не принудительное).

3. **`MediaSettingsSender`** — компонент, который отправляет `change-media-settings` команду. Логирует «change-media-settings command was merged with ongoing one» — команды могут объединяться.

## Сводка

`change-media-settings` — сигналинг-команда с флагами audio/video/screen_capture/animoji. `handleForceChangeMediaSettings` — принудительное изменение медиа-настроек участника сервером/администратором.
