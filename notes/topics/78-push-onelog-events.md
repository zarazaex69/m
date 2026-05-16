---
tags: [push, onelog, telemetry, surveillance, fcm]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/a6c.java
  - work/jadx_base/sources/defpackage/bt1.java
  - work/jadx_base/sources/ru/ok/tamtam/android/services/NotificationTamService.java
  - work/jadx_base/sources/defpackage/r3c.java
related:
  - "[[19-fcm-push-payload]]"
  - "[[31-onelog-event-categories]]"
---

# PUSH OneLog — телеметрия push-уведомлений

## Операции

| Операция | p_op | Что |
|---|---|---|
| `<pushType>` | `show` | показано уведомление (с `chat_id`, `show_source`) |
| `<pushType>` | `drop` | уведомление отброшено (с `p_dr` — причина) |
| `<pushType>` | `delivered` | уведомление доставлено |
| `Action` | `open_chats` | нажатие на уведомление → открыть список чатов |
| `Action` | `open_chat` | нажатие → открыть конкретный чат (с `trid`, `eKey`) |
| `Action` | `open_url` | нажатие → открыть URL (с `trid`, `eKey`) |
| `Action` | `m_as_read` | отметить как прочитанное из уведомления (с `trid`, `eKey`) |
| `Action` | `n_q_rep` | ответить на уведомление (с `trid`, `eKey`) |
| `Action` | `n_q_rep_empty` | ответить пустым сообщением |
| `Action` | `n_canceled` | уведомление отменено |
| `Action` | `n_canceled_ch` | уведомление отменено (channel) |
| `InboundCall` | — | входящий звонок через push |

## Атрибуты

- `trid` — tracking ID уведомления
- `eKey` — ключ события
- `chat_id` — ID чата
- `show_source` — источник показа
- `p_dr` — причина отброса

## Что важно

1. **Сервер знает каждое взаимодействие с push-уведомлениями**: показ, отброс, открытие, ответ, отметка как прочитанное. Это полный лог поведения пользователя с уведомлениями.

2. **`show` с `chat_id`** — сервер знает, уведомление из какого чата было показано пользователю.

3. **`m_as_read` из уведомления** — сервер знает, когда пользователь отмечает сообщения как прочитанные прямо из notification shade, не открывая приложение.

4. **`n_q_rep`** — быстрый ответ из уведомления. Сервер знает, когда пользователь отвечает на сообщения через notification action.

## Сводка

12+ PUSH OneLog операций. Сервер получает полный лог взаимодействия с push-уведомлениями: показ (с chat_id), отброс, открытие, быстрый ответ, отметка как прочитанное.
