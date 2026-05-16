---
tags: [join-requests, server-control, pms, groups]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/profile/screens/joinrequests/JoinRequestsScreen.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[03-pms-server-flags]]"
  - "[[36-channels-feature-gated]]"
---

# join-requests — server-gated заявки на вступление в чаты

`PmsKey.f148joinrequests` — bool. Server-gated включение функции заявок на вступление в закрытые чаты/каналы.

`JoinRequestsScreen` — экран управления заявками. Принимает `chatId` — ID чата, для которого показываются заявки.

## Что важно

При включении `join-requests` — администраторы закрытых чатов могут видеть и одобрять/отклонять заявки на вступление. Сервер контролирует, доступна ли эта функция.

## Сводка

`join-requests` — server-gated функция заявок на вступление в закрытые чаты. `JoinRequestsScreen` с `chatId`.
