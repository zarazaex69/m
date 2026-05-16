---
tags: [pmskey, server-control, debug, user-debug-report, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[03-pms-server-flags]]"
  - "[[343-rtd-server-flags]]"
  - "[[244-log-sensitive-server-flag]]"
---

# PmsKey — user-debug-report и другие интересные флаги

## user-debug-report (PmsKey #4)

`user-debug-report` — серверный флаг для отладочных отчётов пользователя. Это механизм, позволяющий серверу запросить отладочный отчёт от конкретного пользователя.

## Другие интересные PmsKey

| PmsKey | Что |
|---|---|
| `user-debug-report` | **отладочный отчёт пользователя** |
| `debug-mode` | режим отладки |
| `min-log-level` | минимальный уровень логирования |
| `android-use-logcat-logger` | использовать logcat |
| `debug-profile-info` | отладочная информация профиля |
| `fake-chats` | **фейковые чаты** |
| `fake-in-app-review` | **фейковый in-app review** |
| `debug-broken-contact` | отладка сломанного контакта |
| `fb-exec-replace` | замена Firebase executors |
| `fb-exec-modifiers-names` | имена модификаторов Firebase |
| `anr-config` | конфигурация ANR |
| `watchdog-config` | конфигурация watchdog |
| `system-thread-pool-queue` | очередь системного пула потоков |
| `fresco-executor` | executor Fresco |
| `blocked-users` | **заблокированные пользователи** |
| `story` | истории |
| `ilm` | ILM |
| `landscape` | ландшафтный режим |

## Что важно

1. **`user-debug-report`** — сервер может запросить отладочный отчёт от конкретного пользователя.

2. **`fake-chats`** — сервер может включить фейковые чаты (для тестирования).

3. **`blocked-users`** — список заблокированных пользователей управляется сервером.

## Сводка

`user-debug-report`/`fake-chats`/`fake-in-app-review`/`blocked-users`/`anr-config`/`watchdog-config`.
