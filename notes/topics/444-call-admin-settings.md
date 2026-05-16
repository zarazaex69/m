---
tags: [calls, admin-settings, debug-menu, surveillance, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/calls/ui/ui/settings/CallAdminSettingsScreen.java
  - work/jadx_base/sources/one/me/calls/ui/ui/debugmenu/CallDebugMenuScreen.java
related:
  - "[[101-calls-sdk-managers]]"
  - "[[398-media-mute-manager]]"
  - "[[433-devmenu-details]]"
---

# CallAdminSettingsScreen + CallDebugMenuScreen

## CallAdminSettingsScreen — настройки администратора звонка

Экран настроек администратора. Логирует аналитику при изменении настроек:

```json
{
  "screen": N,
  "camera": bool,
  "microphone": bool,
  "screenshare": bool,
  "recording": bool,
  "waiting": bool
}
```

Событие: `ADMIN_CALL_SETTINGS` (CALL).

## CallDebugMenuScreen — отладочное меню звонка

Отладочное меню звонка в production сборке.

## Что важно

1. **`ADMIN_CALL_SETTINGS`** — каждое изменение настроек администратора логируется с полным набором флагов.

2. **`recording`/`waiting`/`screenshare`** — администратор может управлять записью, залом ожидания и захватом экрана.

3. **`CallDebugMenuScreen`** — отладочное меню звонка в production сборке.

## Сводка

`CallAdminSettingsScreen`: логирует `ADMIN_CALL_SETTINGS{screen, camera, microphone, screenshare, recording, waiting}`. `CallDebugMenuScreen`: отладочное меню в production.
