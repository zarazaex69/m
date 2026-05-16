---
tags: [calls, roles, server-control, permissions]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/kv1.java
related:
  - "[[154-conversation-features-roles]]"
  - "[[153-signaling-commands]]"
---

# kv1 — роли участников звонка

`kv1` enum: `CREATOR` (0), `ADMIN` (1), `SPEAKER` (2).

Используются в `enableFeatureForRoles(feature, Set<kv1> roles)` — фичи звонка (RECORD, ASR_RECORD, MOVIE_SHARE, ADD_PARTICIPANT) могут быть ограничены ролями. Например, только CREATOR и ADMIN могут включить запись.

`mute-participant` (см. [[153-signaling-commands]]) — вероятно, доступно только ADMIN/CREATOR.
