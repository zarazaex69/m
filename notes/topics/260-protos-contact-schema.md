---
tags: [protocol, contact, protos, schema, surveillance, gender, phone]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/tamtam/nano/Protos.java
related:
  - "[[06-contacts]]"
  - "[[206-protos-schemas]]"
---

# Protos.Contact — схема контакта

`Protos.Contact` — полная структура контакта.

## Статусы аккаунта

`AccountStatus_ACTIVE`(0), `AccountStatus_BLOCKED`(1), `AccountStatus_DELETED`(2).

## Типы контакта

`OFFICIAL`(0), `BOT`(1), `SERVICE_ACCOUNT`(2).

## Флаги

`BLOCKED`(1), `REMOVED`(2), `RESTRICTED`(4), `NO_FORWARD`(5), `HAS_WEBAPP`(3).

## Поля

| Поле | Что |
|---|---|
| `serverId` | серверный ID |
| `serverPhone` | **номер телефона** |
| `gender` | **пол** (MALE=1/FEMALE=2) |
| `accountStatus` | статус аккаунта |
| `status` | статус контакта |
| `type` | тип (бот/сервис/обычный) |
| `photoId` | ID фото |
| `registrationTime` | **время регистрации** |
| `lastSyncTime` | время последней синхронизации |
| `lastUpdateTime` | время последнего обновления |
| `lastSearchClickTime` | **время последнего клика в поиске** |
| `lastShowingUnknownContactBar` | время показа плашки «не контакт» |
| `organizationIds[]` | **ID организаций** |
| `options[]` / `profileOptions[]` | опции |
| `settings` | настройки |

## ContactName типы

`UNKNOWN`(0), `CUSTOM`(1), `DEVICE`(2), `ONEME`(3).

## Что важно

1. **`serverPhone`** — номер телефона хранится в protobuf контакта.

2. **`gender`** — пол контакта хранится локально.

3. **`registrationTime`** — время регистрации контакта в MAX.

4. **`lastSearchClickTime`** — время последнего клика на контакт в поиске. Поведенческая метрика.

5. **`organizationIds[]`** — контакт может принадлежать нескольким организациям.

## Сводка

`Protos.Contact`: serverPhone/gender/accountStatus/registrationTime/lastSearchClickTime/organizationIds[]/ContactName(CUSTOM/DEVICE/ONEME).
