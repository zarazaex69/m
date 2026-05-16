---
tags: [fake-chats, server-control, surveillance, chats-list]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/chats/picker/chats/PickerChatsListWidget.java
  - work/jadx_base/sources/defpackage/rtd.java
related:
  - "[[520-rtd-additional-fields]]"
  - "[[03-pms-server-flags]]"
---

# isFakeChatsEnabled — фейковые чаты

`isFakeChatsEnabled` — флаг RTD для включения фейковых чатов.

## Использование

`PickerChatsListWidget(folderId, scopeId, filter, isFakeChatsEnabled, isFiltersEnabled, isInMultiSelect, onMultiSelectToggled)`

## Что важно

1. **`isFakeChatsEnabled`** — сервер может включить "фейковые чаты" в списке чатов.

2. Это параметр `PickerChatsListWidget` — виджета выбора чатов.

3. Возможно, это тестовый режим для демонстрации чатов без реальных данных.

## Сводка

`isFakeChatsEnabled` — RTD флаг для фейковых чатов в `PickerChatsListWidget`.
