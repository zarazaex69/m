---
tags: [complaints, moderation, server-control, pms]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/l64.java
  - work/jadx_base/sources/defpackage/t6b.java
  - work/jadx_base/sources/defpackage/om9.java
  - work/jadx_base/sources/defpackage/xr1.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[03-pms-server-flags]]"
  - "[[36-channels-feature-gated]]"
---

# Жалобы — server-controlled список причин и механизм

## available-complaints (#13)

`PmsKey.f13availablecomplaints` — server-pushed список строк (enum-значений), определяющий **доступные причины жалобы**. Используется в `l64.java` и `t6b.java`.

Логика: при открытии формы жалобы клиент читает `available-complaints` из PmsKey, парсит строки в enum `a64` (типы жалоб), и показывает только те причины, которые разрешены сервером.

Это означает: **сервер контролирует, на что именно пользователь может пожаловаться**. Сервер может убрать определённые причины жалобы из списка (например, «политический контент», «дезинформация»).

## server-side-complains-enabled (#266)

`PmsKey.f266serversidecomplainsenabled` — bool, default `false`. Используется в `om9.java` в 8+ местах.

При `true` — жалобы обрабатываются через серверный flow (GetAvailableComplaintsUseCase). При `false` — используется локальный hardcoded список причин.

Это означает: **сервер может переключить систему жалоб с локальной на серверную**, получив полный контроль над тем, какие жалобы принимаются и как обрабатываются.

## complainReasonsSync

`ri9.complainReasonsSync` — timestamp последней синхронизации причин жалоб. Причины жалоб синхронизируются с сервером периодически.

## Что важно

1. **Сервер контролирует список причин жалоб** — это прямой инструмент модерации. Убрав «политический контент» из `available-complaints`, сервер делает невозможным пожаловаться на такой контент через стандартный UI.

2. **`server-side-complains-enabled`** — переключение на серверный flow жалоб. При включении сервер получает полный контроль над процессом: может изменять причины, добавлять дополнительные шаги, собирать дополнительные данные.

3. **Связь с `channels-complaint-enabled`** (см. [[36-channels-feature-gated]]) — отдельный PmsKey для жалоб на каналы. Три уровня контроля: глобальный `server-side-complains-enabled`, per-feature `channels-complaint-enabled`, и per-reason `available-complaints`.

## Сводка

Система жалоб в MAX полностью server-controlled: список причин (`available-complaints`), механизм обработки (`server-side-complains-enabled`), и per-feature включение. Сервер может ограничить или расширить возможности пользователя по подаче жалоб без обновления клиента.
