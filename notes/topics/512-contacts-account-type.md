---
tags: [contacts, account-type, mimetype, surveillance]
status: confirmed
sources:
  - work/apktool_base/res/values/strings.xml
related:
  - "[[510-contacts-sync]]"
  - "[[06-contacts]]"
---

# Contacts Account Type + MIME Type

## Значения

| Ресурс | Значение | Что |
|---|---|---|
| `tt_contact_account_type` | `ru.ok.tamtam` | **тип аккаунта** в Android Contacts |
| `tt_contact_mimetype` | `vnd.android.cursor.item/vnd.ru.ok.tamtam.android.profile` | **MIME-тип** контакта MAX |

## Что важно

1. **`ru.ok.tamtam`** — аккаунт MAX в Android Contacts идентифицируется как `ru.ok.tamtam` (TamTam).

2. **`vnd.ru.ok.tamtam.android.profile`** — кастомный MIME-тип для профилей MAX в Android Contacts.

3. Это означает, что MAX создаёт аккаунт `ru.ok.tamtam` в системе Android Accounts.

## Сводка

`tt_contact_account_type = ru.ok.tamtam`. `tt_contact_mimetype = vnd.android.cursor.item/vnd.ru.ok.tamtam.android.profile`.
