---
tags: [organizations, official-org, money-transfer, server-control, pms]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ezd.java
  - work/jadx_base/sources/defpackage/ige.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[03-pms-server-flags]]"
  - "[[11-state-bots-and-content-policy]]"
  - "[[22-gost-digitalid-family]]"
---

# Организации, официальные аккаунты и денежные переводы

## Организации (org-profile, official-org)

`org-profile` (#221) — PmsKey с описанием из `ezd.java`: «Плашка представителя организации в профиле». Default `false`. Это server-gated фича, которая добавляет в профиль пользователя плашку «представитель организации».

`official-org` (#212) — `isOfficialOrgEnabled` в `rtd.java`. Включает отображение «официальных организаций» в UI — вероятно, верифицированных государственных или корпоративных аккаунтов.

`OfficialOrgLabel` (`ige.java`) — отдельный view type в списке чатов/профилей для отображения плашки официальной организации.

Что важно: сервер контролирует, кто получает статус «официальной организации» и «представителя организации». Это механизм верификации, аналогичный «синей галочке» в Twitter/Telegram, но полностью server-controlled без прозрачных критериев.

## Денежные переводы (money-transfer-botid)

`money-transfer-botid` (#81) — ID бота для денежных переводов. Аналогично `stickers-botid` и `digitalid-botid` — весь flow денежных переводов идёт через бот-аккаунт, ID которого задаётся сервером.

`rtd.moneyBotId` — accessor. Бот-ID используется для открытия чата с ботом переводов при нажатии кнопки «Перевести деньги».

Что важно: денежные переводы в MAX реализованы через бот-аккаунт, а не через нативный платёжный API. Это означает, что весь flow (сумма, получатель, подтверждение) проходит через WS-сообщения с ботом. Сервер видит все транзакции.

## Комментарии (comments-enabled)

`comments-enabled` (#?) — PmsKey `f83commentsenabled`. Server-gated включение комментариев к постам/сообщениям. По умолчанию неизвестно.

## Сводка

Три server-controlled механизма: (1) `org-profile` / `official-org` — верификация организаций и представителей без прозрачных критериев; (2) `money-transfer-botid` — денежные переводы через бот-аккаунт с server-visible транзакциями; (3) `comments-enabled` — server-gated комментарии.
