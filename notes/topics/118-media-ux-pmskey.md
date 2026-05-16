---
tags: [voice-messages, video, server-control, pms, ux]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/e.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[42-voice-messages-opus-server-controlled]]"
  - "[[03-pms-server-flags]]"
---

# speedy-voice-messages, open-video-from-start, new-media-upload-ui — UX PmsKey

| Ключ | # | Default | Описание |
|---|---|---|---|
| `speedy-voice-messages` | 276 | false | «Лоудер на отправку голосовых сообщений» |
| `open-video-from-start` | 217 | false | открывать видео с начала |
| `new-media-upload-ui` | 204 | false | новый UI загрузки медиа |
| `new-media-edit-screen` | 203 | false | новый экран редактирования медиа |
| `video-fast-seek-enabled` | 296 | false | быстрый seek в видео |
| `video-speed` | 301 | — | скорость воспроизведения видео |

`speedy-voice-messages` — «Лоудер на отправку голосовых сообщений» — показывает loader при отправке голосового. Это UX-фича, server-gated.

Все эти PmsKey управляют UX-деталями медиа. Сервер контролирует даже такие мелочи как «показывать ли loader при отправке голосового».
