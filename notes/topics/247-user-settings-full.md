---
tags: [user-settings, server-control, privacy, notifications, transcription, family-protection]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ugj.java
related:
  - "[[57-ws-session-config-fingerprint]]"
  - "[[22-gost-digitalid-family]]"
---

# UserSettings — полный список ключей

`ugj.java` — сериализатор UserSettings. Полный список ключей настроек пользователя.

## Все ключи

| Ключ | Что |
|---|---|
| `app.extra.text.size.sp` / `app.extra.text.size.mode` | размер текста |
| `app.pinLock.screenshotEnabled` | скриншоты при блокировке |
| `app.calls.incoming.ringtone` | рингтон входящего звонка |
| `app.notification.chats.show` / `app.notification.dialogs.show` | показ уведомлений |
| `app.notification.chats.show.last` | показ последнего уведомления |
| `app.notification.dontDisturbUntil` | не беспокоить до |
| `app.notification.show.new.users` | уведомления о новых пользователях |
| `app.notification.ringtone` / `app.notification.dialogs.ringtone` / `app.notification.chats.ringtone` | рингтоны |
| `app.notification.led.color` / `app.notification.dialogs.led.color` / `app.notification.chats.led.color` | цвет LED |
| `app.notification.vibrate` / `app.notification.dialogs.vibrate` / `app.notification.chats.vibrate` | вибрация |
| `app.privacy.online.show` | показывать онлайн |
| `app.privacy.incoming.call` | кто может звонить |
| `app.privacy.chats.invite` | кто может добавлять в чаты |
| `app.privacy.inactive.ttl` | TTL неактивности |
| `app.privacy.safe_mode` / `app.privacy.safe_mode_no_pin` | безопасный режим |
| `app.privacy.search_by_phone` | поиск по телефону |
| `app.privacy.unsafe.files.default` | небезопасные файлы по умолчанию |
| `app.privacy.content.level.access` | уровень доступа к контенту |
| `app.privacy.phone.number.privacy` | приватность номера телефона |
| `app.group.chat.call.notification.status` | уведомления о звонках в группах |
| `app.suggest.stickers.status` | предложение стикеров |
| `app.family.protection.status` | **статус семейной защиты** |
| `app.messages.enable.double.tap.reactions` | реакции двойным тапом |
| `app.messages.double.tap.reaction` | реакция двойным тапом |
| `app.media.video.compress` | сжатие видео |
| `audio.transcription.enabled` | **транскрипция аудио включена** |

## Что важно

1. **`audio.transcription.enabled`** — пользователь может включить/выключить транскрипцию аудио-сообщений. Сервер знает этот статус.

2. **`app.family.protection.status`** — статус семейной защиты. Связано с [[22-gost-digitalid-family]].

3. **`app.privacy.phone.number.privacy`** — приватность номера телефона. Новый ключ.

4. **`app.privacy.inactive.ttl`** — TTL неактивности. Автоматическое удаление аккаунта при неактивности.

## Сводка

26+ ключей UserSettings. Ключевые: `audio.transcription.enabled`, `app.family.protection.status`, `app.privacy.phone.number.privacy`, `app.privacy.inactive.ttl`.
