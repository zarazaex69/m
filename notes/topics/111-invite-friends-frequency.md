---
tags: [invite, server-control, pms, growth]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/chats/tab/ChatsTabWidget.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[03-pms-server-flags]]"
  - "[[44-informer-banners-fakeboss-livestreams]]"
---

# invite-friends-sheet-frequency — server-controlled частота показа приглашений

`PmsKey.f143invitefriendssheetfrequency` — массив int (дни). Управляет расписанием показа листа «пригласить друзей».

## Механизм

`ChatsTabWidget.java:454` — логика показа:
1. Читает массив `invite-friends-sheet-frequency` — список интервалов в днях.
2. Проверяет `inviteFriendsTimesShown` (счётчик показов) и `inviteFriendsShowTime` (время последнего показа).
3. Если прошло `iArrC1[timesShown]` дней с последнего показа — показывает лист.

Пример: `[1, 7, 30]` — первый раз через 1 день, второй через 7, третий через 30.

## Что важно

1. **Сервер контролирует расписание показа приглашений** — когда и как часто пользователю показывается предложение пригласить друзей.

2. **`alreadyInvitedFriends`** (в `yag.java`) — флаг, что пользователь уже приглашал друзей. Сервер знает, приглашал ли пользователь кого-то.

3. Это growth-механизм: сервер оптимизирует частоту показа для максимизации приглашений.

## Сводка

`invite-friends-sheet-frequency` — server-pushed массив интервалов (дни) для показа листа приглашений. Сервер контролирует growth-механизм приглашений.
