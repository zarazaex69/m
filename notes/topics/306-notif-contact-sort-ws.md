---
tags: [protocol, ws, notif-contact-sort, contacts, phones, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/xzb.java
  - work/jadx_base/sources/defpackage/zx.java
related:
  - "[[06-contacts]]"
  - "[[305-notif-contact-ws]]"
---

# NOTIF_CONTACT_SORT WS — синхронизация сортировки контактов

`NOTIF_CONTACT_SORT` (опкод 118) — уведомление об изменении порядка сортировки контактов.

## Структура

| Поле | Что |
|---|---|
| `ids[]` | **ID контактов** (порядок) |
| `phones[]` | **телефоны** (порядок) |

## События

- `ContactSortEvent` — изменился порядок контактов
- `PhonesSortEvent` — изменился порядок телефонов

## Что важно

1. **Сервер синхронизирует порядок контактов** между устройствами. Это означает, что порядок контактов хранится на сервере.

2. **`phones[]`** — список телефонов с порядком. Сервер знает все телефоны пользователя.

3. Данные сохраняются в файлы `contactSort` и `phonesSort` локально.

## Сводка

`NOTIF_CONTACT_SORT`: ids[](порядок контактов)/phones[](порядок телефонов). Сервер синхронизирует порядок контактов.
