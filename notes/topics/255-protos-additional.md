---
tags: [protocol, protos, chat, channel, restrictions, bots]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Protos.java
related:
  - "[[206-protos-schemas]]"
---

# Protos.java — дополнительные схемы

Дополнение к [[206-protos-schemas]].

## BotsInfo (в Chat)

| Поле | Что |
|---|---|
| `hasBots` | есть ли боты |
| `suspendedBot` | бот приостановлен |

## ChannelInfo (в Chat)

| Поле | Что |
|---|---|
| `admins[]` | **ID администраторов** |
| `membersCount` | количество участников |
| `signAdmin` | подписывать посты именем администратора |

## RestrictionsInfo

| Поле | Что |
|---|---|
| `expiration` | **время истечения ограничения** |

## SelfProfile

| Поле | Что |
|---|---|
| `serverId` | серверный ID |
| `profileOptions[]` | опции профиля |
| `restrictions` | **Map<Integer, RestrictionsInfo>** — ограничения |

## Widget.Content типы

`ADAPTIVE_ICON`(1), `PICTURE`(2), `TITLE_BIG`(3), `TITLE_STANDARD`(4), `DESCRIPTION`(5), `KEYBOARD`(6), `UNSUPPORTED`(0).

## Что важно

1. **`SelfProfile.restrictions`** — Map ограничений с временем истечения. Сервер может ограничить функции профиля.

2. **`ChannelInfo.admins[]`** — список ID администраторов канала.

3. **`BotsInfo.suspendedBot`** — флаг приостановки бота. Связано с `TYPE_SUSPEND_BOT` задачей.

## Сводка

`BotsInfo`: hasBots/suspendedBot. `ChannelInfo`: admins[]/membersCount/signAdmin. `SelfProfile`: restrictions(Map<Integer,RestrictionsInfo>). `RestrictionsInfo`: expiration.
