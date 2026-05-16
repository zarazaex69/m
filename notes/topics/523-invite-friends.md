---
tags: [invite-friends, analytics, surveillance, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ht8.java
  - work/jadx_base/sources/one/me/inviteactions/invitefriendsbottomsheet/InviteFriendsToMaxBottomSheet.java
related:
  - "[[460-yag-shared-prefs]]"
  - "[[03-pms-server-flags]]"
---

# InviteFriendsToMaxBottomSheet — приглашение друзей

`InviteFriendsToMaxBottomSheet` — bottom sheet для приглашения друзей в MAX.

## Аналитика

При клике "Пригласить":
```java
kt8.a("clicked_to_invite", "main", "trigger_max")
```

## SharedPreferences

```java
yag.K.y(yagVar, yag.m0[32], Boolean.TRUE)  // app.already.invited.friends = true
```

## Текст приглашения

Берётся из RTD: `rtdVar.u.x(rtdVar, rtd.e0[10])` — это `inviteLink` или форматированная строка.

## Что важно

1. **`clicked_to_invite`** — клик "Пригласить" логируется с источником `"main"` и триггером `"trigger_max"`.

2. **`app.already.invited.friends = true`** — факт приглашения сохраняется в SharedPreferences.

3. Текст приглашения берётся с сервера через RTD.

## Сводка

`InviteFriendsToMaxBottomSheet`: `clicked_to_invite(main, trigger_max)` → `app.already.invited.friends=true`. Текст с сервера.
