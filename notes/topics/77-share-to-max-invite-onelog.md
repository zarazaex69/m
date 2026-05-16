---
tags: [sharing, invite, onelog, telemetry, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/y5h.java
  - work/jadx_base/sources/defpackage/kt8.java
related:
  - "[[31-onelog-event-categories]]"
  - "[[38-deeplink-routes-full-map]]"
---

# SHARE_TO_MAX и INVITE_MAX_BANNER OneLog

## SHARE_TO_MAX

`y5h.java` — аналитика «поделиться в MAX» (share intent из внешних приложений).

Атрибуты:
- `source` — источник (из какого приложения)
- `chatsInfo` — **список чатов, в которые пользователь поделился**, с типами: `DIALOG`, `DIALOG_WITH_BOT`, `DIALOG_SAVED_MESSAGES`, `PRIVATE_CHANNEL`, `PUBLIC_CHANNEL`, `PRIVATE_CHAT`, `PUBLIC_CHAT`

Это означает: когда пользователь делится контентом из внешнего приложения в MAX, сервер получает список всех чатов, в которые был отправлен контент, с их типами и ID.

## INVITE_MAX_BANNER

`kt8.java` — аналитика баннера «пригласить друзей в MAX».

Операции:
- `show` — показан баннер (entryPoint=`main`, linkType=`trigger_max`)
- `click_link` — нажата ссылка (entryPoint=`main`, linkType=`invite_friends`)

Атрибуты: `session_id`, `screen`, `entryPoint`, `linkType`, `status=success`.

## Что важно

**`SHARE_TO_MAX.chatsInfo`** — это особенно интересно. Когда пользователь делится ссылкой/файлом из браузера или другого приложения в MAX, сервер получает **список всех чатов, в которые был отправлен контент**. Это означает, что сервер знает, кому пользователь пересылает внешний контент.

## Сводка

`SHARE_TO_MAX` — сервер знает, из какого приложения и в какие чаты пользователь делится контентом. `INVITE_MAX_BANNER` — сервер знает, видел ли пользователь баннер приглашения и кликнул ли на него.
