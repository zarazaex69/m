---
tags: [deeplink, intent-surface, attack-surface, navigation]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/zh6.java
  - work/jadx_base/sources/defpackage/f64.java
  - work/jadx_base/sources/defpackage/ar9.java
  - work/jadx_base/sources/defpackage/rv2.java
  - work/jadx_base/sources/defpackage/kae.java
  - work/jadx_base/sources/defpackage/vw8.java
  - work/jadx_base/sources/defpackage/j4h.java
related:
  - "[[13-deeplinks-idp]]"
  - "[[18-manifest-deep-dive]]"
  - "[[29-external-callback-idp-flow]]"
  - "[[19-fcm-push-payload]]"
---

# Deeplink-роутер — внутренние схемы MAX

В дополнение к [[13-deeplinks-idp]] (deeplink `:auth?externalCallback=1` и `LinkInterceptorActivity`) — здесь полная карта внутренних роутов deeplink-системы, которые обрабатывает `LinkInterceptorActivity` и его подсистема.

Все роуты регистрируются через `ps0.A(target, ":<route_name>", positionalArgs, queryParams, flags)`. Это внутренний DSL роутера. Найденные роуты:

## Карта роутов

| Роут | Позиц. параметры | Query-параметры | Где зарегистрирован | Назначение |
|---|---|---|---|---|
| `:auth` | — | `externalCallback` | (см. [[13-deeplinks-idp]]) | вход в MAX как Identity Provider |
| `:external_callback` | — | `params` | `defpackage/zh6.java` | внутренний роут-callback после OIDC-flow (см. [[29-external-callback-idp-flow]]) |
| `:complaint` | — | — | `defpackage/f64.java` | UI жалобы (на пользователя/чат/канал) |
| `:settings` | — | — | `defpackage/ar9.java` | открыть настройки приложения |
| `:chats` | `id`, `type` | — | `defpackage/rv2.java` | открыть конкретный чат |
| `:profile` | `id`, `type` | — | `defpackage/kae.java` | открыть профиль пользователя или канала |
| `:join` | `id`, `link` | — | `defpackage/vw8.java` | присоединиться к чату по ссылке-приглашению |
| `:share` | `text` | — | `defpackage/j4h.java` | системный share intent в MAX (отправить текст) |

## Что особенного

1. **`:share` через deeplink** — внешнее приложение через `https://max.ru/:share?text=<...>` или `max://max.ru/:share?text=<...>` может **подставить текст в форму отправки сообщения в MAX**. По имеющемуся коду явной валидации нет; внешнее приложение может предзаполнить произвольный текст для пересылки. Не «zero-click», но «zero-confirmation после deeplink».
2. **`:join`** — присоединение к чату по `id`+`link`. Это включает каналы. Если канал «server-hidden» (см. [[36-channels-feature-gated]]), но deeplink приходит от внешнего приложения, поведение не очевидно.
3. **`:profile?id=<X>&type=<channel|user>`** — открывает чужой профиль. Сам факт открытия логируется в OneLog как `CLICK/profile_button_click` (см. [[31-onelog-event-categories]]) с `source_meta` — то есть сервер видит, кого пользователь смотрит.
4. **`:auth` + `:external_callback`** — это пара для IdP-flow (см. [[29-external-callback-idp-flow]]). `:auth?externalCallback=1` — точка входа извне; `:external_callback?params=<...>` — внутренний резолв.
5. **`:settings`** без параметров — просто открыть настройки. Полезный сценарий — если внешний сервис нужно «отправить пользователя проверить разрешения».
6. **`:complaint`** — открыть UI жалобы. Внешний сервис теоретически может через deeplink триггерить открытие формы жалобы на конкретный объект, но позиционных параметров не объявлено.

## Полная схема URL

Принимаемые форматы (см. [[18-manifest-deep-dive]] §6 и [[13-deeplinks-idp]]):

- `https://max.ru/:<route>?<params>` (autoVerify=true, system-confirmed)
- `http://max.ru/:<route>?<params>` (cleartext-вариант, тоже принимается)
- `max://max.ru/:<route>?<params>` (custom scheme)

## Что не нашёл

- Роута для прямой отправки сообщения с предзаданным получателем (`:send_to?phone=<X>&text=<Y>`).
- Роута для оформления подписки на канал.
- Роута для триггера запуска конкретной мини-апы по chat-id.
- Роута для триггера звонка.

Это не значит, что таких функций нет — некоторые могут реализовываться через `:chats?id=<X>&type=...` плюс state-машина приложения. Но как отдельных deeplink-роутов с явным DSL-объявлением через `ps0.A(...)` я их не вижу.

## Сводка

Внутренний deeplink-роутер MAX обрабатывает 8 явно объявленных роутов: `:auth`, `:external_callback`, `:complaint`, `:settings`, `:chats`, `:profile`, `:join`, `:share`. Все три префикса (`https://max.ru/`, `http://max.ru/`, `max://max.ru/`) выходят на тот же роутер. Внешние приложения через deeplink могут открыть профиль, чат, форму жалобы, форму отправки сообщения с предзаполненным текстом, и триггерить IdP-flow.
