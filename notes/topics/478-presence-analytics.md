---
tags: [presence, telemetry, surveillance, online-status, contact-tracking]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/r3e.java
related:
  - "[[472-log-controller-event-types]]"
  - "[[06-contacts]]"
---

# PRESENCE.EVENT_MESSAGE_COUNTER — аналитика presence

`r3e` — сбор статистики presence (онлайн-статус контактов).

## Счётчики

| Счётчик | Что |
|---|---|
| `online_contact_opened` | **открыт чат с онлайн-контактом** |
| `online_contact_closed` | закрыт чат с онлайн-контактом |
| `online_stranger_opened` | **открыт чат с онлайн-незнакомцем** |
| `online_stranger_closed` | закрыт чат с онлайн-незнакомцем |
| `offline_contact_opened` | **открыт чат с оффлайн-контактом** |
| `offline_contact_closed` | закрыт чат с оффлайн-контактом |
| `offline_stranger_opened` | **открыт чат с оффлайн-незнакомцем** |
| `offline_stranger_closed` | закрыт чат с оффлайн-незнакомцем |

## Что важно

1. **8 счётчиков** — сервер знает, сколько раз пользователь открывал чаты с онлайн/оффлайн контактами и незнакомцами.

2. **`presenceofflinelog`** — PmsKey для логирования оффлайн presence.

3. Счётчики сохраняются в SharedPreferences и отправляются периодически.

4. Различает **контакты** и **незнакомцев** — сервер знает, с кем общается пользователь.

## Сводка

`PRESENCE.EVENT_MESSAGE_COUNTER {online_contact_opened/closed, online_stranger_opened/closed, offline_contact_opened/closed, offline_stranger_opened/closed}`.
