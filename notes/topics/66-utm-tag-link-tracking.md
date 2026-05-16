---
tags: [utm, tracking, server-control, pms, links]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ghj.java
  - work/jadx_base/sources/defpackage/rp8.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[03-pms-server-flags]]"
  - "[[62-white-list-links-content-control]]"
---

# utm-tag-for-trigger-link-share — server-controlled UTM-трекинг ссылок

`PmsKey.f293utmtagfortriggerlinkshare` — bool. При `true` клиент **автоматически добавляет `utm_source=trigger`** к ссылкам, которыми пользователь делится из MAX.

## Механизм (ghj.java)

При включённом PmsKey: клиент берёт URL, удаляет существующий `utm_source` параметр, и добавляет `utm_source=trigger`. Результирующая ссылка содержит маркер `?utm_source=trigger`.

Это означает: **все ссылки, которыми пользователи делятся из MAX, автоматически помечаются UTM-тегом `trigger`**. Владелец сайта, на который ведёт ссылка, видит в своей аналитике, что трафик пришёл из MAX (источник `trigger`).

## rp8.java — client=613

В `rp8.java:209` при построении URL добавляется `client=613` и `utm_source=max`. Это другой механизм — для конкретного типа ссылок (вероятно, ссылки на профили или чаты MAX) добавляется идентификатор клиента `613`.

## Что важно

1. **`utm_source=trigger`** — это не просто аналитика для MAX. Это маркер, который виден **внешним сайтам**. Любой сайт, получающий трафик из MAX, видит `utm_source=trigger` в своих логах. Это позволяет внешним сайтам идентифицировать пользователей MAX.

2. **Server-controlled** — сервер включает/выключает добавление UTM-тега. При включении все ссылки из MAX помечаются без ведома пользователя.

3. **`client=613`** — идентификатор клиента MAX в URL-параметрах. Это hardcoded, не server-controlled.

## Сводка

`utm-tag-for-trigger-link-share` — server-gated автоматическое добавление `utm_source=trigger` ко всем ссылкам, которыми пользователи делятся из MAX. Внешние сайты видят этот маркер и могут идентифицировать трафик из MAX.
