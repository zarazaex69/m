---
tags: [contacts, blocking, onelog, telemetry, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/xi4.java
  - work/jadx_base/sources/defpackage/nl2.java
related:
  - "[[06-contacts]]"
  - "[[31-onelog-event-categories]]"
---

# CONTACT_OR_BLOCK OneLog — телеметрия добавления/блокировки

`defpackage/xi4.java` — аналитика UI «добавить в контакты или заблокировать».

## Операции

| Операция | Атрибуты | Что |
|---|---|---|
| `showed` | `screen`, `UIElementType=add_or_block_infobar` | показан infobar «добавить/заблокировать» |
| `clicked` | `screen`, `clickType` | нажата кнопка |

## clickType значения

- `to_contacts` — пользователь нажал «добавить в контакты»
- `block` — пользователь нажал «заблокировать»
- `close` — пользователь закрыл infobar

## Что важно

Сервер знает: когда пользователю показывался infobar «добавить/заблокировать», и что он выбрал. Это означает, что сервер знает о каждом решении пользователя заблокировать кого-либо или добавить в контакты через этот UI.

В сочетании с [[06-contacts]] (двусторонняя синхронизация контактов) — сервер имеет полную картину социального графа пользователя: кто в контактах, кто заблокирован, и когда принимались эти решения.

## Сводка

`CONTACT_OR_BLOCK` OneLog: сервер знает каждое решение пользователя добавить в контакты или заблокировать через infobar UI.
