---
tags: [multi-account, account-switching, server-control, pms]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
  - work/jadx_base/sources/defpackage/qp6.java
  - work/jadx_base/sources/one/me/android/OneMeApplication.java
  - work/jadx_base/sources/defpackage/hr9.java
  - work/jadx_base/sources/defpackage/pl5.java
related:
  - "[[03-pms-server-flags]]"
  - "[[18-manifest-deep-dive]]"
---

# Multi-account ("two-account-mvp") — server-gated до двух аккаунтов

PmsKey `two-account-mvp` (#195) — server-gated включение фичи второго аккаунта. По имени «mvp» (minimum viable product) — фича в активной разработке/раскатке.

## Сущности

- `PmsKey.f288twoaccountmvp` — server-controlled bool в `qp6.B0`.
- `OneMeApplication.initSecondAccountMvp()` (line 308) — инициализация при старте приложения, читает PmsKey, при `enabled=true` поднимает второй scope аккаунтов.
- `defpackage/hr9.java:45` — содержит лог «Swap user account from <oldId> to <newId>, new userId = <X>» — то есть UI-операция переключения между аккаунтами.
- `defpackage/pl5.java:37` — использует `f288twoaccountmvp` для условного UI-переключателя.

## Логирование

Tag `Multiaccount` / `multiaccount` (две вариации в разных классах) — `OneMeApplication.initSecondAccountMvp() isEnabled = ...` и `Swap user account from … to …, new userId = …`. Эти строки попадают в logcat при включённом флаге `log-full` (PmsKey, см. [[03-pms-server-flags]]).

## Что важно

1. **Multi-account — server-gated**, не локальная фича. Сервер может включить или выключить второй аккаунт у конкретного пользователя через PmsKey-сегментацию.
2. **Два аккаунта = два WS-сессии** одновременно. Каждый аккаунт держит свой keep-alive WS к `api.oneme.ru`. Это означает раздвоение нагрузки на сеть и батарею (плюс дополнительные точки телеметрии).
3. **`Swap user account` lives in the same process** — переключение происходит без перезапуска приложения, через `hs9.l1.o → newUserId`. Состояние одного аккаунта может пересекаться с другим в общем application-scope, например, общий FCM token (`onNewToken` в `FcmMessagingService` — см. [[19-fcm-push-payload]]) обрабатывается одинаково для обоих.
4. **Mvp** в имени намекает, что это первая итерация — возможно, не больше двух одновременных аккаунтов. Полный multi-account (3+) в этой версии кода не виден.

## Скептический разбор

- Multi-account — нормальная фича, у Telegram и WhatsApp Beta есть.
- Что специфично: server-gated rollout (как у каналов в [[36-channels-feature-gated]]). Сервер выбирает, кому фича доступна.
- В случае госмессенджера это может означать, что второй аккаунт включается только для определённых категорий (например, корпоративных пользователей) или, наоборот, отключается для пользователей, попавших в server-side список.

## Сводка

Multi-account в MAX 26.15.3 — server-gated MVP, до двух аккаунтов. Включается PmsKey `two-account-mvp` (#195), переключение в рамках одного процесса, общий FCM-канал. Серверная политика может в любой момент включить/выключить эту фичу для произвольного пользователя.
