---
tags: [contacts, sync, content-provider, surveillance, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ndi.java
related:
  - "[[06-contacts]]"
  - "[[430-db-presence-phones-messages]]"
---

# ndi — Contacts Sync (синхронизация контактов)

`ndi` — синхронизация контактов с Android ContactsContract.

## Операции

| Метод | Что |
|---|---|
| `d(di4, name, phone, email)` | **добавить контакт** |
| `e(Set)` | **удалить контакты** |
| `f(Collection)` | **синхронизировать** |
| `g(Collection, bool)` | **полная синхронизация** |

## Поля контакта

| Поле | Что |
|---|---|
| `account_type` | `tt_contact_account_type` |
| `sync1` | **ID пользователя MAX** |
| `mimetype` | `tt_contact_mimetype` |
| `data1`/`data2` | данные контакта |
| `vnd.android.cursor.item/name` | имя (data2=first, data3=last) |
| `vnd.android.cursor.item/phone_v2` | телефон |

## Что важно

1. **`caller_is_syncadapter=true`** — запросы помечаются как sync adapter.

2. **`sync1`** — ID пользователя MAX хранится в поле `sync1` контакта.

3. **`tt_contact_mimetype`** — кастомный MIME-тип для контактов MAX.

4. Синхронизация двусторонняя: MAX → Android Contacts и обратно.

## Сводка

`ndi`: ContactsContract sync. `sync1` = userId MAX. `tt_contact_mimetype`. Операции: add/remove/sync.
