---
tags: [proxy, pmskey, notif-config, server-control, surveillance, debug]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ozb.java
related:
  - "[[461-login-response-proxy]]"
  - "[[03-pms-server-flags]]"
  - "[[343-rtd-server-flags]]"
---

# ozb — NotifConfigLogic (обработка конфигурации сервера)

`ozb` — `NotifConfigLogic` — обработка конфигурации от сервера.

## Шаги обработки конфигурации

1. **Step 2**: `serverSettings` — обновляет `PmsKey.proxy` и `PmsKey.proxydomains`
2. **Step 3**: `user settings` — обновляет настройки пользователя (safeMode и др.)
3. **Step 5**: `chats settings` — обновляет настройки чатов

## Что обновляется

| Ключ | Что |
|---|---|
| `PmsKey.proxy` | **список прокси** |
| `PmsKey.proxydomains` | **домены прокси** |
| `debug-mode` | режим отладки |
| `user-debug-report` | отчёт отладки пользователя |
| `safeMode` (sgj.v) | безопасный режим |

## Что важно

1. **`PmsKey.proxy`** — прокси обновляется при каждой конфигурации сервера.

2. **`debug-mode`/`user-debug-report`** — debug-режим управляется сервером через конфигурацию.

3. **`safeMode`** — если `safeMode=false` → вызывается `xa4.a()` (возможно, отключение SafeMode).

4. **`changeChatSettings`** — настройки чатов обновляются при конфигурации.

## Сводка

`NotifConfigLogic`: Step2(proxy/proxydomains) → Step3(user settings: debug-mode/user-debug-report/safeMode) → Step5(chats settings).
