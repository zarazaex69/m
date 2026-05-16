---
tags: [invite-by-qr, analytics, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/k77.java
related:
  - "[[523-invite-friends]]"
  - "[[439-qr-auth]]"
---

# InviteByQrBottomSheet — приглашение через QR

`InviteByQrBottomSheet` — bottom sheet для приглашения через QR-код.

## Аналитика

При клике "Пригласить":
```java
kt8.a("clicked_to_invite", 
  (screen == 100) ? "plus" : "main",  // источник
  "invite_friends"                     // триггер
)
```

## Что важно

1. **`clicked_to_invite`** — клик логируется с источником (`plus`/`main`) и триггером `"invite_friends"`.

2. **`screen == 100`** — если экран 100 → источник `"plus"`, иначе `"main"`.

## Сводка

`InviteByQrBottomSheet`: `clicked_to_invite(plus|main, invite_friends)`.
