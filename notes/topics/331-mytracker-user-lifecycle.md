---
tags: [telemetry, mytracker, user-lifecycle, login, registration, invite, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/com/my/tracker/userlifecycle/MyTrackerUserLifecycle.java
  - work/jadx_base/sources/defpackage/rqb.java
  - work/jadx_base/sources/defpackage/glf.java
related:
  - "[[328-mytracker-init]]"
  - "[[330-mytracker-device-params]]"
---

# MyTrackerUserLifecycle — события жизненного цикла пользователя

`MyTrackerUserLifecycle` — отслеживание ключевых событий пользователя.

## Методы

| Метод | Что |
|---|---|
| `trackLoginEvent(userId, null)` | **вход в систему** |
| `trackRegistrationEvent(userId, null)` | **регистрация** |
| `trackInviteEvent()` | **приглашение** |

## Вызовы

- **`trackLoginEvent(String.valueOf(userId), null)`** — вызывается при входе пользователя. Передаёт ID пользователя.
- **`trackRegistrationEvent(String.valueOf(userId), null)`** — вызывается при регистрации.

## Что важно

1. **`userId`** — ID пользователя MAX передаётся в MyTracker при каждом входе и регистрации.

2. Второй параметр `null` — тип входа/регистрации не указывается.

3. MyTracker знает, когда пользователь входит и регистрируется.

## Сводка

`trackLoginEvent(userId, null)` — при входе. `trackRegistrationEvent(userId, null)` — при регистрации. `trackInviteEvent()` — при приглашении.
