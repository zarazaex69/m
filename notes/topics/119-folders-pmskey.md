---
tags: [folders, server-control, pms, ui]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/e15.java
  - work/jadx_base/sources/defpackage/om5.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[03-pms-server-flags]]"
  - "[[92-webapp-complaints-folders-polls-ws]]"
---

# Folders PmsKey — server-controlled папки чатов

| Ключ | # | Default | Что |
|---|---|---|---|
| `folders-max-count` | 121 | 30 | максимальное количество папок |
| `enable-filters-for-folders` | 106 | false | включить фильтры для папок |
| `channels-suggests-folder` | 69 | — | папка с рекомендованными каналами |

`folders-max-count=30` — сервер контролирует, сколько папок может создать пользователь. Default 30.

`enable-filters-for-folders` — server-gated включение фильтров в папках (фильтрация чатов по типу).

`channels-suggests-folder` — server-gated папка с рекомендованными каналами (см. [[36-channels-feature-gated]]).
