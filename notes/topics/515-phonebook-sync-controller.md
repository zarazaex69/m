---
tags: [contacts, phonebook, sync, surveillance, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/kfd.java
related:
  - "[[510-contacts-sync]]"
  - "[[514-contacts-content-observer]]"
  - "[[06-contacts]]"
---

# kfd — Phonebook Sync Controller

`kfd` — контроллер синхронизации телефонной книги.

## Методы

| Метод | Что |
|---|---|
| `a(list)` | `onPhonebookUpdated` — телефонная книга обновлена |
| `b(contacts, phones, map)` | `onSyncSuccess` — синхронизация успешна |
| `d()` | `syncInternal` — внутренняя синхронизация |

## Логика onSyncSuccess

1. Логирует: `contacts=%d, phones=%d, requested=%d`
2. Проверяет сломанные контакты (BrokenContactException)
3. Обнаруживает цикл синхронизации (>= 10 попыток)
4. Обновляет невалидные телефоны в БД
5. Синхронизирует с сервером через `ndi`

## Что важно

1. **`contacts=%d, phones=%d`** — количество контактов и телефонов логируется.

2. **Цикл синхронизации** — если один телефон синхронизируется >= 10 раз → `IllegalStateException("Contacts sync cycle")`.

3. **`BrokenContactException`** — сломанные контакты отправляются как исключения.

4. **`syncInternal`** — несинхронизированные телефоны (max 100 за раз).

## Сводка

`kfd`: `onPhonebookUpdated` → `syncInternal` → `onSyncSuccess(contacts, phones, requested)`. Цикл защита: >= 10 попыток.
