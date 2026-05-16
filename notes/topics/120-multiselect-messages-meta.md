---
tags: [ui, server-control, pms, multiselect, messages]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/nd1.java
  - work/jadx_base/sources/defpackage/as2.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[03-pms-server-flags]]"
  - "[[46-server-controlled-logging-flags]]"
---

# Multi-select и messages meta PmsKey

| Ключ | # | Default | Описание |
|---|---|---|---|
| `multi-select-bars-redesign` | 193 | false | «Мультиселект с действиями в боттом баре» |
| `chats-multi-select` | 78 | false | мультиселект чатов |
| `log-messages-meta` | 155 | false | «Сбор meta info видимых сообщений по клику» |
| `log-chat-meta` | 153 | false | логировать метаданные чатов |

`multi-select-bars-redesign` — новый дизайн мультиселекта с action bar внизу. Server-gated.

`log-messages-meta` — «Сбор meta info видимых сообщений по клику» — при клике на сообщение собирается meta info. Это дополнение к [[46-server-controlled-logging-flags]]: конкретная операция — клик на сообщение.
