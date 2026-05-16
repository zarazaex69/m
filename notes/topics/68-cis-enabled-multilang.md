---
tags: [localization, cis, server-control, pms, geo]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/pl5.java
  - work/jadx_base/sources/one/me/android/OneMeApplication.java
  - work/jadx_base/sources/defpackage/yag.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[03-pms-server-flags]]"
  - "[[41-server-side-client-diagnostic-struct]]"
---

# cis-enabled и multi-lang — геолокационные UI-адаптации

## cis-enabled (#81)

`PmsKey.f81cisenabled` — bool, default `false`. Описание из `pl5.java`: «Включить ui улучшения для СНГ стран».

Это server-gated включение специфических UI-улучшений для пользователей из стран СНГ. Сервер знает страну пользователя (через `location` в [[41-server-side-client-diagnostic-struct]]) и может включить `cis-enabled` для пользователей из РФ, Беларуси, Казахстана и т.п.

Что именно включает `cis-enabled` — неизвестно без дополнительного анализа. Возможно: специфические шрифты, форматы дат, платёжные методы, или UI-элементы, специфичные для СНГ-рынка.

## multi-lang (#192)

`PmsKey.f192multilang` — server-gated включение мультиязычного интерфейса. `yag.multiLangEnabled` — локальный флаг. `OneMeApplication.java:155` — при `app.lang.multilang=false` или Android >= 33 используется системный язык.

`one.me.settings.multilang.SettingsLocaleScreen` — экран выбора языка. `LocaleBottomSheet` — bottom sheet выбора локали.

## Что важно

1. **`cis-enabled`** — сервер знает, что пользователь из СНГ, и включает специфический UI. Это означает, что MAX имеет разные UI-режимы для разных регионов, управляемые сервером.

2. **`multi-lang`** — server-gated мультиязычность. Сервер может включить или выключить возможность смены языка интерфейса для конкретных пользователей.

3. **`installationMarket`** (из [[67-yag-shared-prefs-per-account]]) + **`location`** (из [[41-server-side-client-diagnostic-struct]]) + **`cis-enabled`** — три уровня геолокационной адаптации: откуда установлено, где находится, и какой UI показывать.

## Сводка

`cis-enabled` — server-gated UI-адаптации для СНГ-пользователей. `multi-lang` — server-gated мультиязычность. Оба управляются сервером на основе геолокации пользователя.
