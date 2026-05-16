---
tags: [telemetry, presence, critlog, surveillance, contacts]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/r3e.java
  - work/jadx_base/sources/defpackage/xra.java
related:
  - "[[207-critlog-events]]"
  - "[[36-presence-server-controlled]]"
---

# PRESENCE/EVENT_MESSAGE_COUNTER + MESSAGE_CLICKABLE_ELEMENT_ACTIONS

## EVENT_MESSAGE_COUNTER — счётчики открытия чатов

CritLog `PRESENCE`/`EVENT_MESSAGE_COUNTER` — счётчики открытия чатов по типу собеседника и его статусу:

| Счётчик | Что |
|---|---|
| `online_contact_opened` | открыт чат с онлайн-контактом |
| `online_contact_closed` | закрыт чат с онлайн-контактом |
| `online_stranger_opened` | открыт чат с онлайн-незнакомцем |
| `online_stranger_closed` | закрыт чат с онлайн-незнакомцем |
| `offline_contact_opened` | открыт чат с офлайн-контактом |
| `offline_contact_closed` | закрыт чат с офлайн-контактом |
| `offline_stranger_opened` | открыт чат с офлайн-незнакомцем |
| `offline_stranger_closed` | закрыт чат с офлайн-незнакомцем |

Счётчики накапливаются в SharedPreferences и отправляются пакетом.

## MESSAGE_CLICKABLE_ELEMENT_ACTIONS — клики в сообщениях

| Событие | Что |
|---|---|
| `clicked_clickable_element` | клик на кликабельный элемент |
| `clicked_copy` | копирование |
| `clicked_open_link` | открытие ссылки |
| `clicked_open_mail` | открытие email |
| `clicked_call` | звонок по номеру |
| `shown_update_app` | показан баннер обновления |
| `clicked_update_app` | клик на обновление |

Параметры: `messageId`, `element_type`, `source_id`, `source_type`.

## Что важно

1. **`online_stranger_opened`** — сервер знает, что пользователь открыл чат с незнакомцем, который онлайн. Это косвенно раскрывает паттерны общения.

2. **`clicked_call`** — клик на номер телефона в сообщении логируется.

3. **`clicked_open_link`** — каждое открытие ссылки из сообщения логируется с `source_id`/`source_type`.

## Сводка

`EVENT_MESSAGE_COUNTER`: 8 счётчиков открытия чатов (online/offline × contact/stranger × opened/closed). `MESSAGE_CLICKABLE_ELEMENT_ACTIONS`: 7 типов кликов в сообщениях с messageId/source_id.
