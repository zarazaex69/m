---
tags: [protocol, ws, notif-contact, contact-update, block, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/aq4.java
related:
  - "[[06-contacts]]"
  - "[[251-contact-tasks]]"
  - "[[20-ws-protocol-opcodes]]"
---

# NOTIF_CONTACT WS — уведомление об изменении контакта

`NOTIF_CONTACT` (опкод 112) — уведомление об изменении контакта.

## ContactUpdateAction — действия с контактом

| Действие | Что |
|---|---|
| `ADD` | добавить контакт |
| `UPDATE` | обновить контакт |
| `REMOVE` | удалить контакт |
| `BLOCK` | **заблокировать** |
| `UNBLOCK` | **разблокировать** |

## Что важно

1. **`BLOCK`/`UNBLOCK`** — сервер уведомляет о блокировке/разблокировке контакта.

2. Это двусторонняя синхронизация контактов — сервер уведомляет клиент об изменениях.

3. Связано с `ContactUpdate` Task (см. [[251-contact-tasks]]).

## Сводка

`NOTIF_CONTACT`: ContactUpdateAction(ADD/UPDATE/REMOVE/BLOCK/UNBLOCK).
