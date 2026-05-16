---
tags: [contact-rename, telemetry, surveillance, contact-analytics]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ii4.java
  - work/jadx_base/sources/defpackage/ki4.java
  - work/jadx_base/sources/defpackage/c87.java
related:
  - "[[472-log-controller-event-types]]"
  - "[[06-contacts]]"
---

# CONTACT_RENAME_BANNER + CONTEXT_MENU — аналитика контактов и меню

## CONTACT_RENAME_BANNER.save

```json
{
  "user2Id": userId
}
```

Логируется при сохранении переименования контакта. Содержит ID пользователя.

## CONTACT_RENAME_BANNER.show

Логируется при показе баннера переименования контакта.

## CONTEXT_MENU.folder_context_menu_readall

Логируется при нажатии "Прочитать всё" в контекстном меню папки.

## Что важно

1. **`user2Id`** — ID пользователя при переименовании контакта. Сервер знает, кого переименовывает пользователь.

2. **`folder_context_menu_readall`** — действие "прочитать всё" в папке логируется.

## Сводка

`CONTACT_RENAME_BANNER.save {user2Id}`. `CONTACT_RENAME_BANNER.show`. `CONTEXT_MENU.folder_context_menu_readall`.
