---
tags: [messaging, server-control, pms, p2p, queue]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ezd.java
  - work/jadx_base/sources/defpackage/vei.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[03-pms-server-flags]]"
  - "[[20-ws-protocol-opcodes]]"
---

# client-conv-id и send-queue-size — P2P conversation IDs и очередь отправки

## client-conv-id (#82)

`PmsKey.f82clientconvid` — bool, default `false`. Описание из `ezd.java`: «Включить клиентское создание conversations id».

При `client-conv-id=false` (default) — ID разговора генерируется сервером. При `true` — клиент генерирует ID самостоятельно. Это P2P-оптимизация: клиент не ждёт подтверждения от сервера для создания conversation ID.

Связь с `enableP2PClientConversationIds` в `qp6.java` — это тот же флаг.

## send-queue-size (#265)

`PmsKey.f265sendqueuesize` — int, default `30`. Максимальный размер очереди отправки сообщений. Сервер контролирует, сколько сообщений может быть в очереди одновременно.

## Что важно

1. **`client-conv-id`** — при включении клиент генерирует conversation ID без сервера. Это означает, что ID разговора может быть предсказуемым (если алгоритм генерации известен). Потенциально — возможность коллизий или предсказания ID.

2. **`send-queue-size=30`** — сервер ограничивает количество сообщений в очереди. При `send-queue-size=1` пользователь не сможет отправлять сообщения быстро (DoS-like ограничение).

## Сводка

`client-conv-id` — server-gated P2P conversation ID generation. `send-queue-size` — server-controlled лимит очереди отправки (default 30).
