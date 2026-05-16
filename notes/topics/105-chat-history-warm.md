---
tags: [chat-history, server-control, pms, performance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/hrg.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[03-pms-server-flags]]"
  - "[[54-ws-session-config-fingerprint]]"
---

# chat-history-warm-opts — прогрев истории чатов

`PmsKey.f74chathistorywarmopts` — int, default `-1` (= `WarmOptions.All`). Управляет «прогревом» (preloading) истории чатов при запуске.

## Значения

- `-1` = `WarmOptions.All` — прогревать все чаты
- `0` = отключить прогрев
- `>0` = прогревать только N чатов

## Связанные PmsKey

| Ключ | # | Что |
|---|---|---|
| `chat-history-warm-opts` | (в rtd) | опции прогрева |
| `chat-history-warm-fail-interval` | (в rtd) | интервал при ошибке прогрева |
| `chat-history-notif-msg-strategy` | (в rtd) | стратегия уведомлений для истории |
| `chat-history-persist` | (в rtd) | персистентность истории |
| `chat-history-login-count` | (в rtd) | количество входов для прогрева |

## Что важно

1. **Прогрев истории** — при запуске MAX загружает историю чатов в кэш. Сервер контролирует, сколько чатов прогревать.

2. **`chat-history-persist`** — server-gated персистентность истории. При включении история сохраняется между сессиями.

3. **`ChatHistoryWarmPerfRegistrar`** — отдельный регистратор производительности для прогрева. Метрики прогрева отправляются на сервер.

4. Все эти параметры отправляются в WS session config (см. [[54-ws-session-config-fingerprint]]).

## Сводка

`chat-history-warm-opts` — server-controlled прогрев истории чатов. Default `-1` = все чаты. Связанные PmsKey управляют стратегией, интервалами и персистентностью.
