---
tags: [calls, server-control, pms, group-calls]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/k73.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[45-calls-sdk-pmskey-cluster]]"
  - "[[03-pms-server-flags]]"
---

# call-chat-members-load-config — конфиг загрузки участников группового звонка

`PmsKey.f22callchatmembersloadconfig` — JSON-конфиг, парсится в `ChatMembersLoadConfig` (`k73.java`).

## Поля

| Поле | Тип | Что |
|---|---|---|
| `newLoadingContactsLogicEnabled` | bool | включить новую логику загрузки контактов |
| `maxLoadCount` | int | максимальное количество загружаемых участников |
| `minInCall` | int | минимальное количество участников в звонке |

Default: `ChatMembersLoadConfig(newLoadingContactsLogicEnabled=false, maxLoadCount=0, minInCall=0)`.

## Что важно

1. **`maxLoadCount`** — сервер контролирует, сколько участников группового звонка загружается в UI. При `maxLoadCount=0` — загружаются все. При ограничении — пользователь видит только часть участников.

2. **`minInCall`** — минимальное количество участников для определённого поведения (вероятно, для включения grid-view или другого UI).

3. **`newLoadingContactsLogicEnabled`** — server-gated переключение на новую логику загрузки контактов в звонке.

## Сводка

`call-chat-members-load-config` — server-pushed конфиг с тремя полями: `newLoadingContactsLogicEnabled`, `maxLoadCount`, `minInCall`. Сервер контролирует, сколько участников группового звонка видит пользователь.
