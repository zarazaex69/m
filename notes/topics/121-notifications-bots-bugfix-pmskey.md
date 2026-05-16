---
tags: [notifications, bots, server-control, pms, bugfix]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/om5.java
  - work/jadx_base/sources/defpackage/ydc.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[03-pms-server-flags]]"
---

# Notifications и bots PmsKey — server-gated bugfixes и bot-фичи

| Ключ | # | Default | Описание |
|---|---|---|---|
| `cancel-stale-notifications` | 64 | false | «Отмена устаревших нотификаций в notifyAllChats» |
| `new-intent-fix` | 202 | false | «onNewIntent NPE fix» |
| `bots-channel-adding` | 19 | false | добавление ботов в каналы |
| `bot-start-param` | 18 | — | параметр запуска бота |
| `bot-complaint-enabled` | 17 | — | жалобы на ботов |

`cancel-stale-notifications` — «Отмена устаревших нотификаций в notifyAllChats». Это server-gated bugfix: при включении клиент отменяет устаревшие уведомления при обновлении списка чатов.

`new-intent-fix` — «onNewIntent NPE fix» — server-gated исправление NPE в `onNewIntent`. Это server-gated bugfix для конкретной проблемы.

Оба — server-gated bugfixes. Сервер может включить исправления для конкретных устройств/версий без обновления клиента.
