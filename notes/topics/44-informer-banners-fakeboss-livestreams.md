---
tags: [informer, banners, server-control, fake-boss, live-streams, calls]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/sk.java
  - work/jadx_base/sources/defpackage/oj8.java
  - work/jadx_base/sources/defpackage/kk8.java
  - work/jadx_base/sources/defpackage/rk8.java
  - work/jadx_base/sources/defpackage/lj6.java
  - work/jadx_base/sources/defpackage/gj6.java
  - work/jadx_base/sources/ru/ok/tamtam/nano/Protos.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[03-pms-server-flags]]"
  - "[[19-fcm-push-payload]]"
  - "[[36-channels-feature-gated]]"
---

# Informer banners, FakeBoss, LiveStreams — три server-pushed UI-механизма

## 1. Informer banners — серверные баннеры в UI

`informer_banner` — отдельная таблица в Room-базе данных (`OneMeRoomDatabase`). Схема:

| Поле | Тип | Что |
|---|---|---|
| `id` | TEXT PK | идентификатор баннера |
| `title` | TEXT | заголовок |
| `description` | TEXT | текст |
| `settings` | INTEGER | битовые флаги настроек |
| `priority` | INTEGER | порядок показа (DESC) |
| `repeat` | INTEGER | повторять ли показ |
| `rerun` | INTEGER | перезапускать ли |
| `animoji_id` | INTEGER | привязанный анимодзи |
| `url` | TEXT | URL для перехода по клику |
| `type` | INTEGER | тип баннера |
| `click_time` | INTEGER | timestamp клика |
| `show_time` | INTEGER | timestamp показа |
| `close_time` | INTEGER | timestamp закрытия |
| `show_count` | INTEGER | счётчик показов |

Баннеры синхронизируются с сервером (`informerBannersSync` timestamp в SharedPreferences `ri9`). PmsKey `informer-enabled` (#185) — глобальный kill-switch. `informer-divider-can-hidden` (#312) и `informer-icon-themed` (#313) — UI-параметры.

Телеметрия: `rk8.java` пишет `informer_id` и `informer_type` в аналитику при каждом взаимодействии с баннером. Сервер знает, какой баннер пользователь видел, кликнул, закрыл.

Баннеры с `url` — это **server-pushed deep-link'и в UI**. Сервер может показать баннер с произвольным URL, который откроется при клике. В сочетании с deeplink-роутером (см. [[38-deeplink-routes-full-map]]) это означает: сервер может через баннер открыть любой deeplink, включая `:join`, `:share`, `:auth`.

## 2. FakeBoss — «фейковый начальник»

В ресурсах и коде присутствует фича `FakeBoss`:

- `PmsKey.f48callsfakebossincomingcallenabled` = `calls-fakeboss-incoming-call-enabled` — server-gated.
- Строки: `fake_boss_in_organization`, `fake_boss_no_organization`, `fake_boss_registration`, `fake_boss_show_mutual_chats`.
- `FakeBossListItem(contactServerId=..., phoneNumber=...)` — объект с номером телефона.
- `messages_list_fake_boss_view_type` — отдельный view type в списке сообщений.
- `lj6.java` — UI регистрации FakeBoss с кнопкой «показать общие чаты».

По имени и структуре — это фича «защита от мошенников, притворяющихся начальником». Пользователь регистрирует своего «настоящего начальника», и при входящем звонке от незнакомого номера MAX показывает предупреждение «это не ваш начальник». Это социальная инженерия-защита, популярная в РФ после волны мошеннических звонков «от ФСБ/начальника».

Что важно: `calls-fakeboss-incoming-call-enabled` — **server-gated**. Сервер включает/выключает эту фичу. При включении клиент хранит `contactServerId` и `phoneNumber` «настоящего начальника» — то есть **сервер знает, кто является начальником пользователя** (или кого пользователь считает таковым).

## 3. LiveStreams — стримы через Protobuf

`Protos.LiveStream` — отдельный Protobuf-объект в `ru.ok.tamtam.nano.Protos`. Поля: `liveStream` (объект) и `liveStreamUpdateTime` (long timestamp) в составе более крупного объекта (вероятно, `Chat` или `Channel`).

PmsKey `live-streams` (#?) и `live-streams-url-prefix` (#?) — server-controlled включение стримов и URL-prefix для стриминг-сервера. Это означает, что URL стриминг-сервера задаётся сервером, а не захардкожен в клиенте.

## Сводка

Три server-pushed UI-механизма: (1) `informer_banner` — произвольные баннеры с URL, синхронизируемые с сервером, с телеметрией каждого взаимодействия; (2) `FakeBoss` — server-gated фича, при включении которой сервер узнаёт «начальника» пользователя; (3) `LiveStream` — стримы с server-pushed URL-prefix стриминг-сервера.
