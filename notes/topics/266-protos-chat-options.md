---
tags: [protocol, chat-options, protos, schema, server-control, privacy]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Protos.java
related:
  - "[[256-protos-chat-schema]]"
  - "[[206-protos-schemas]]"
---

# Protos.ChatOptions — настройки чата

`Protos.ChatOptions` — флаги настроек чата/канала.

## Поля

| Поле | Что |
|---|---|
| `aPlusChannel` | A+ канал |
| `allCanPinMessage` | все могут закреплять |
| `comments` | комментарии включены |
| `contentLevelChat` | **чат с контентным уровнем** |
| `joinRequest` | **запросы на вступление** |
| `membersCanSeePrivateLink` | участники видят приватную ссылку |
| `official` | официальный |
| `onlyAdminCanAddMember` | только администратор добавляет |
| `onlyAdminCanCall` | **только администратор может звонить** |
| `onlyOwnerCanChangeIconTitle` | только владелец меняет иконку/название |
| `sentByPhone` | **отправлено по телефону** |
| `serviceChat` | сервисный чат |
| `signAdmin` | подписывать именем администратора |

## Что важно

1. **`onlyAdminCanCall`** — администратор может запретить звонки для обычных участников.

2. **`contentLevelChat`** — чат с ограничением по уровню контента. Связано с `app.privacy.content.level.access`.

3. **`joinRequest`** — включены запросы на вступление. Связано с `pendingJoinRequestsCount` в [[256-protos-chat-schema]].

4. **`sentByPhone`** — флаг «отправлено по телефону». Неясное назначение.

## Сводка

`ChatOptions`: 13 флагов. Ключевые: onlyAdminCanCall/contentLevelChat/joinRequest/membersCanSeePrivateLink/signAdmin.
