---
tags: [mytracker, telemetry, user-profile, surveillance, vk, ok]
status: confirmed
sources:
  - work/jadx_base/sources/com/my/tracker/MyTrackerParams.java
related:
  - "[[04-telemetry-endpoints]]"
  - "[[14-stat-prefs-metrics]]"
---

# MyTrackerParams — пользовательские параметры для MyTracker

`MyTrackerParams` — параметры пользователя, передаваемые в MyTracker SDK.

## Параметры пользователя

| Параметр | Что |
|---|---|
| `age` | **возраст** |
| `gender` | **пол** (MALE=1/FEMALE=2/UNKNOWN=0/UNSPECIFIED=-1) |
| `email` / `emails[]` | **email** |
| `phone` / `phones[]` | **номер телефона** |
| `okId` / `okIds[]` | **ID в OK.ru** |
| `vkId` / `vkIds[]` | **ID ВКонтакте** |
| `vkConnectId` / `vkConnectIds[]` | **VK Connect ID** |
| `icqId` / `icqIds[]` | **ICQ ID** |
| `customUserId` / `customUserIds[]` | кастомный ID |
| `customParam(key)` | кастомный параметр |
| `lang` | язык |

## Что важно

1. **`age`/`gender`/`email`/`phone`** — демографические данные передаются в MyTracker. Это `tracker-api.vk-analytics.ru`.

2. **`okId`/`vkId`/`vkConnectId`/`icqId`** — кросс-платформенные ID. MyTracker связывает пользователя MAX с его аккаунтами в OK.ru, ВКонтакте, ICQ.

3. Все параметры поддерживают массивы (`ids[]`) — один пользователь может иметь несколько ID.

4. `UserInfoState` — иммутабельный снапшот состояния пользователя.

## Сводка

`MyTrackerParams`: age/gender/email/phone/okId/vkId/vkConnectId/icqId. MyTracker связывает пользователя MAX с аккаунтами OK.ru/VK/ICQ.
