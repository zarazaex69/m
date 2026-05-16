---
tags: [onelog, telemetry, surveillance, messages, links]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/xra.java
  - work/jadx_base/sources/defpackage/zle.java
related:
  - "[[31-onelog-event-categories]]"
  - "[[62-white-list-links-content-control]]"
---

# MESSAGE_CLICKABLE_ELEMENT_ACTIONS и CHAT_PROFILE_CLICKABLE_ELEMENT_ACTIONS OneLog

## MESSAGE_CLICKABLE_ELEMENT_ACTIONS

`xra.java` — аналитика кликов на кликабельные элементы в сообщениях.

| Операция | Что |
|---|---|
| `clicked_clickable_element` | клик на кликабельный элемент |
| `clicked_copy` | копирование текста |
| `clicked_open_link` | **открытие ссылки** |
| `clicked_open_mail` | открытие email |
| `clicked_call` | **нажатие на номер телефона** |
| `shown_update_app` | показан баннер обновления |
| `clicked_update_app` | нажат баннер обновления |

Атрибуты: `message_id`, `element_type` (1-5), `source_id` (chat_id), `source_type`.

## CHAT_PROFILE_CLICKABLE_ELEMENT_ACTIONS

`zle.java` — аналитика кликов в профиле чата.

| Операция | Что |
|---|---|
| `clicked_clickable_element` | клик на элемент профиля |
| `clicked_open_context_menu` | открытие контекстного меню |
| `clicked_copy` | копирование |
| `clicked_in_context_menu` | клик в контекстном меню |

## Что важно

1. **`clicked_open_link`** с `message_id` и `source_id` — сервер знает, на какую ссылку в каком сообщении в каком чате нажал пользователь. Это полный лог переходов по ссылкам из сообщений.

2. **`clicked_call`** — сервер знает, когда пользователь нажимает на номер телефона в сообщении. Это означает, что сервер знает, какие номера телефонов пользователь пытается набрать из сообщений.

3. **`clicked_copy`** — сервер знает, когда пользователь копирует текст из сообщений.

## Сводка

`MESSAGE_CLICKABLE_ELEMENT_ACTIONS` — сервер знает каждый клик на ссылку, номер телефона, или копирование текста в сообщениях, с `message_id` и `source_id`. `CHAT_PROFILE_CLICKABLE_ELEMENT_ACTIONS` — клики в профиле чата.
