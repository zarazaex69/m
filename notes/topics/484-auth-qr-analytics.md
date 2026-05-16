---
tags: [auth-qr, telemetry, surveillance, qr-auth, permission]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/zf0.java
related:
  - "[[439-qr-auth]]"
  - "[[472-log-controller-event-types]]"
---

# AUTH_QR.LOG — аналитика QR-авторизации

Событие `AUTH_QR.LOG` логирует все шаги QR-авторизации.

## Действия (action)

| Действие | Что |
|---|---|
| `qr_login_button_click` | **клик кнопки QR-входа** |
| `permission_prompt_shown` | **показан запрос разрешения камеры** |
| `permission_decision` | **решение по разрешению камеры** |
| `qr_scan_failed` | **сканирование QR не удалось** |
| `qr_scan_succeeded` | **сканирование QR успешно** |
| `qr_not_auth_ui_shown` | **показан UI "QR не авторизован"** |

## Параметры

| Поле | Что |
|---|---|
| `permission` | `"camera"` |
| `status` | 1 (granted) / 0 (denied) |
| `fail_reason_code` | код ошибки |

## Что важно

1. **Все шаги QR-авторизации логируются** — от клика до успеха/неудачи.

2. **`permission_decision`** — решение пользователя по разрешению камеры логируется.

3. **`fail_reason_code`** — код ошибки при неудачном сканировании.

## Сводка

`AUTH_QR.LOG {action: qr_login_button_click|permission_prompt_shown|permission_decision|qr_scan_failed|qr_scan_succeeded|qr_not_auth_ui_shown, params: {permission, status, fail_reason_code}}`.
