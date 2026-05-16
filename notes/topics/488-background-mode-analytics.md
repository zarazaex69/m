---
tags: [background-mode, telemetry, surveillance, foreground, permission, snack]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/on0.java
  - work/jadx_base/sources/defpackage/fh9.java
  - work/jadx_base/sources/defpackage/pn0.java
  - work/jadx_base/sources/one/me/background/wake/BackgroundListenService.java
related:
  - "[[442-background-listen-service]]"
  - "[[472-log-controller-event-types]]"
---

# BACKGROUND_MODE events — аналитика фонового режима

## Все события

| Событие | Что |
|---|---|
| `carpet_mode_on` | **вход в foreground** (reachabilityCheck) |
| `snack_shown` | показан snack-bar фонового режима |
| `snack_click_on` | **клик snack-bar** |
| `snack_hidden {reason: swipe}` | скрыт snack-bar (свайп) |
| `system_curtain_shown` | **foreground service запущен** |
| `system_curtain_hidden` | **foreground service остановлен** |
| `work_in_background_permission {status: allowed|denied}` | **решение по разрешению фоновой работы** |

## Что важно

1. **`carpet_mode_on`** — логируется при каждом входе приложения в foreground.

2. **`work_in_background_permission {status}`** — решение пользователя по разрешению фоновой работы логируется.

3. **`system_curtain_shown/hidden`** — запуск/остановка foreground service логируется.

4. **`snack_click_on`** — клик по snack-bar фонового режима логируется.

## Сводка

7 событий: carpet_mode_on / snack_shown / snack_click_on / snack_hidden / system_curtain_shown / system_curtain_hidden / work_in_background_permission{status: allowed|denied}.
