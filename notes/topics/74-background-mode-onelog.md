---
tags: [background, onelog, telemetry, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/background/wake/BackgroundListenService.java
  - work/jadx_base/sources/defpackage/pn0.java
  - work/jadx_base/sources/defpackage/fh9.java
  - work/jadx_base/sources/defpackage/on0.java
  - work/jadx_base/sources/defpackage/jr9.java
related:
  - "[[08-background-wake]]"
  - "[[31-onelog-event-categories]]"
---

# BACKGROUND_MODE OneLog — телеметрия фонового режима

## Операции

| Операция | Что |
|---|---|
| `system_curtain_shown` | системная шторка (notification shade) открыта |
| `system_curtain_hidden` | системная шторка закрыта |
| `work_in_background_permission` | статус разрешения «работа в фоне» |
| `snack_shown` | показан snackbar «работа в фоне» |
| `snack_hidden` | скрыт snackbar «работа в фоне» |
| `snack_click_on` | пользователь нажал на snackbar |
| `carpet_mode_on` | включён «ковровый режим» (carpet mode) |

## Что важно

1. **`system_curtain_shown/hidden`** — `BackgroundListenService` логирует, когда пользователь открывает и закрывает notification shade. Это означает, что сервер знает, когда пользователь взаимодействует с уведомлениями.

2. **`work_in_background_permission`** — статус разрешения на фоновую работу. Сервер знает, разрешил ли пользователь MAX работать в фоне.

3. **`carpet_mode_on`** — «ковровый режим» — вероятно, режим постоянного фонового присутствия (аналог «работы в фоне при перебоях интернета» из [[08-background-wake]]).

4. **`snack_shown/hidden/click_on`** — сервер знает, видел ли пользователь предложение включить фоновый режим и кликнул ли на него.

## Сводка

7 BACKGROUND_MODE операций в OneLog. Сервер знает: когда пользователь открывает notification shade, статус разрешения на фоновую работу, и взаимодействие с UI-предложением включить фоновый режим.
