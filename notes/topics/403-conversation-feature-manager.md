---
tags: [calls-feature, feature-manager, roles, enable-disable, surveillance, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/feature/ConversationFeatureManager.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/feature/internal/commands/ConversationFeatureCommandExecutorImpl.java
  - work/jadx_base/sources/defpackage/ck1.java
related:
  - "[[377-record-manager]]"
  - "[[399-asr-manager-record]]"
  - "[[379-watch-together]]"
  - "[[378-p2prelay-sessionroom]]"
---

# ConversationFeatureManager — управление фичами звонка

`ConversationFeatureManager` — включение/выключение фич звонка по ролям.

## Фичи (ck1)

| Фича | Что |
|---|---|
| `ADD_PARTICIPANT` | добавление участников |
| `RECORD` | **запись звонка** |
| `MOVIE_SHARE` | **WatchTogether (совместный просмотр)** |
| `ASR_RECORD` | **запись ASR** |

## Методы

| Метод | Что |
|---|---|
| `enableFeatureForAll(feature, ...)` | включить фичу для всех |
| `enableFeatureForRoles(feature, roles, ...)` | **включить фичу для конкретных ролей** |
| `isFeatureEnabled(feature)` | включена ли фича |
| `getFeatureRoles(feature)` | роли для фичи |
| `addFeatureListener(feature, listener)` | подписаться на изменения |

## Callbacks (FeatureListener)

| Callback | Что |
|---|---|
| `onFeatureEnabledChanged(feature, isEnabled)` | фича включена/выключена |
| `onFeatureRolesChanged(feature, roles)` | роли для фичи изменились |

## Что важно

1. **`enableFeatureForRoles`** — администратор может включить запись/ASR/WatchTogether только для определённых ролей.

2. **`ASR_RECORD`** — отдельная фича для ASR-записи. Управляется независимо от обычной записи.

3. **`MOVIE_SHARE`** — WatchTogether управляется как фича.

4. Команды отправляются через signaling.

## Сводка

`ConversationFeatureManager`: 4 фичи (ADD_PARTICIPANT/RECORD/MOVIE_SHARE/ASR_RECORD). `enableFeatureForRoles(feature, roles)` через signaling.
