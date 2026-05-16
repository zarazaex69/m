---
tags: [protocol, chat, protos, schema, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Protos.java
related:
  - "[[206-protos-schemas]]"
  - "[[255-protos-additional]]"
---

# Protos.Chat — полная схема чата

`Protos.Chat` — основная структура чата. 40+ полей.

## Типы чатов

`DIALOG`(0), `CHAT`(1), `CHANNEL`(2), `GROUP_CHAT`(3), `COMMENTS`(4).

## Статусы

`ACTIVE`(0), `LEFT`(1), `LEAVING`(2), `REMOVED`(3), `REMOVING`(4), `CLOSED`(5), `HIDDEN`(6), `BLOCKED`(7).

## Ключевые поля

| Поле | Что |
|---|---|
| `cid` / `serverId` | ID чата |
| `type` / `status` / `accessType` | тип/статус/доступ |
| `admins[]` | **ID администраторов** |
| `owner` | **ID владельца** |
| `participantsCount` / `blockedParticipantsCount` | участники |
| `pendingJoinRequestsCount` | **ожидающие запросы на вступление** |
| `commentsBlacklistCount` | чёрный список комментариев |
| `messagesTtlSec` | **TTL сообщений** |
| `draft` / `draftUpdateTime` | черновик |
| `lastMessageId` / `firstMessageId` | ID сообщений |
| `lastMentionMessageId` / `lastReactedMessageId` | упоминания/реакции |
| `pinnedMessageId` / `hidePinnedMessage` | закреплённое |
| `newMessages` / `markedAsUnread` | непрочитанные |
| `unreadPin` / `unreadReply` | непрочитанные pin/reply |
| `joinTime` / `joinRequestTime` | время вступления |
| `invitedBy` | **кто пригласил** |
| `chatFoldersIds[]` | папки чата |
| `restrictions` | ограничения |
| `subscribedToUpdates` | подписан на обновления |
| `liveStreamUpdateTime` | время обновления live-стрима |
| `lastSearchClickTime` | **время последнего клика в поиске** |
| `flagsSettings` / `participantSettings` | настройки |
| `localChanges[]` | локальные изменения |

## AdminParticipant

| Поле | Что |
|---|---|
| `id` | ID администратора |
| `inviterId` | **кто назначил** |
| `permissions` | **права** |

## Что важно

1. **`messagesTtlSec`** — TTL сообщений в чате. Сообщения автоматически удаляются.

2. **`lastSearchClickTime`** — время последнего клика в поиске по чату. Это поведенческая метрика.

3. **`pendingJoinRequestsCount`** — количество ожидающих запросов на вступление.

4. **`invitedBy`** — кто пригласил пользователя в чат.

## Сводка

`Protos.Chat`: 40+ полей. Ключевые: messagesTtlSec/lastSearchClickTime/pendingJoinRequestsCount/invitedBy/admins[]/owner/restrictions.
