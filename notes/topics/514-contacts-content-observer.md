---
tags: [contacts, content-observer, phonebook, surveillance, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/a40.java
related:
  - "[[06-contacts]]"
  - "[[510-contacts-sync]]"
  - "[[513-media-content-observer]]"
---

# a40 — Contacts ContentObserver (наблюдение за контактами)

`a40` — наблюдатель за изменениями контактов через `ContentObserver`.

## Наблюдаемый URI

```java
ContactsContract.Contacts.CONTENT_URI  // все контакты
registerContentObserver(..., true, ...)  // true = все дочерние URI
```

## Логика при изменении

При `onChange` → `checkUpdates`:
- `updatePhones` — обновлённые телефоны
- `deletedPhones` — удалённые телефоны
- `newPhones` — новые телефоны
- `phonesInDb` — телефоны в БД
- `phonesInPhonebook` — телефоны в телефонной книге
- `phonesInPhonebookDistinct` — уникальные телефоны

## Что важно

1. **`ContactsContract.Contacts.CONTENT_URI`** — наблюдение за всеми контактами.

2. **`checkUpdates`** — при каждом изменении контактов синхронизируется с сервером.

3. **Логирование** — все изменения логируются с количеством.

4. Требует разрешение `READ_CONTACTS`.

## Сводка

`a40`: `registerContentObserver(ContactsContract.Contacts.CONTENT_URI, true)` → `checkUpdates(updatePhones/deletedPhones/newPhones)`.
