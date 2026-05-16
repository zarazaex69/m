---
tags: [call-admin, telemetry, surveillance, call-analytics]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ob2.java
  - work/jadx_base/sources/one/me/calls/ui/ui/settings/CallAdminSettingsScreen.java
related:
  - "[[444-call-admin-settings]]"
  - "[[472-log-controller-event-types]]"
---

# CALL events — аналитика звонков (admin settings)

## CALL.ADMIN_CALL_SETTINGS

(см. [[444-call-admin-settings]])

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

## CALL.ADMIN_CALL_SETTINGS_TO_USER

```json
{
  "call_id": string,
  "screen": N,
  "user_id2": long,
  ...
}
```

Логируется при изменении настроек конкретного пользователя в звонке. Содержит `user_id2` — ID пользователя.

## Что важно

1. **`user_id2`** — ID пользователя, чьи настройки изменяются. Сервер знает, кому администратор изменяет настройки.

2. **`call_id`** — ID звонка.

## Сводка

`CALL.ADMIN_CALL_SETTINGS_TO_USER {call_id, screen, user_id2, ...}`. Изменение настроек конкретного пользователя в звонке.
