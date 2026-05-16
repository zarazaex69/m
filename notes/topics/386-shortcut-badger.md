---
tags: [badge, notification-count, launcher, shortcut]
status: confirmed
sources:
  - work/jadx_base/sources/me/leolin/shortcutbadger/ShortcutBadger.java
related:
  - "[[19-fcm-push-payload]]"
  - "[[372-notification-service-boot-receiver]]"
---

# ShortcutBadger — счётчик уведомлений на иконке

`ShortcutBadger` (me.leolin/shortcutbadger) — библиотека для отображения счётчика непрочитанных уведомлений на иконке приложения в лаунчере.

## Поддерживаемые лаунчеры (13)

Adw, Apex, Default, NewHtc, Nova, Sony, Asus, Huawei, Samsung, Zuk, Vivo, ZTE, EverythingMe, OPPO.

## API

| Метод | Что |
|---|---|
| `applyCount(context, count)` | установить счётчик |
| `removeCount(context)` | сбросить счётчик |
| `isBadgeCounterSupported(context)` | проверить поддержку |
| `applyNotification(context, notification, count)` | через Notification API |

## Что важно

Библиотека стандартная (open source). Сама по себе не является индикатором слежки. Используется для отображения числа непрочитанных сообщений на иконке MAX.

## Сводка

`ShortcutBadger.applyCount(ctx, n)` — счётчик на иконке. 13 лаунчеров. Стандартная open-source библиотека.
