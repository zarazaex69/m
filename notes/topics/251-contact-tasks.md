---
tags: [protocol, contacts, tasks, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Tasks.java
related:
  - "[[06-contacts]]"
  - "[[205-tasks-proto]]"
---

# ContactUpdate + ContactVerify Tasks — контакты

## ContactUpdate

| Поле | Что |
|---|---|
| `contactId` | ID контакта |
| `action` | действие |
| `newName` / `oldName` | **новое/старое имя** |
| `lastName` / `oldLastName` | **фамилия** |
| `requestId` | ID запроса |

## ContactVerify

| Поле | Что |
|---|---|
| `contactId` | ID контакта |
| `confirm` | подтвердить |
| `localName` | **локальное имя** |
| `requestId` | ID запроса |

## Что важно

1. **`ContactUpdate.oldName`/`newName`** — сервер знает старое и новое имя контакта при переименовании.

2. **`ContactVerify.localName`** — локальное имя контакта (из адресной книги) передаётся при верификации.

3. **`ContactVerify.confirm`** — подтверждение контакта. Это механизм верификации контактов.

## Сводка

`ContactUpdate`: contactId/action/newName/oldName/lastName/oldLastName. `ContactVerify`: contactId/confirm/localName.
