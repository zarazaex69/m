---
tags: [calls, features, roles, server-control, asr, recording]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ck1.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/feature/ConversationFeatureManagerImpl.java
  - work/jadx_base/sources/ru/ok/android/externcalls/sdk/feature/internal/commands/ConversationFeatureCommandExecutorImpl.java
related:
  - "[[153-signaling-commands]]"
  - "[[152-asr-online-manager]]"
  - "[[25-resources-audit]]"
---

# ConversationFeature — фичи звонков с role-based access

`ck1` — enum фич звонка:

| Фича | Что |
|---|---|
| `ADD_PARTICIPANT` | добавить участника |
| `RECORD` | **запись звонка** |
| `MOVIE_SHARE` | шаринг видео (watch together) |
| `ASR_RECORD` | **запись для ASR (транскрипции)** |

## ConversationFeatureManager

`enableFeatureForAll(feature, onComplete, onError)` — включить фичу для всех.
`enableFeatureForRoles(feature, roles, onComplete, onError)` — включить фичу для определённых ролей.

Отправляет сигналинг-команду `enable-feature-for-roles` с `feature` и `roles`.

## Что важно

1. **`ASR_RECORD`** — отдельная фича для записи аудио для ASR. Это означает, что запись для транскрипции — отдельный механизм от обычной записи звонка.

2. **`RECORD`** — запись звонка. Уже упомянуто в [[25-resources-audit]] (call_record_start.m4a/call_record_stop.m4a). Теперь подтверждено: запись — отдельная фича с role-based access.

3. **Role-based access** — фичи могут быть включены только для определённых ролей (например, только для администраторов). Это server-controlled через сигналинг.

4. **`MOVIE_SHARE`** — watch together. Уже упомянуто в [[143-start-conversation-api]] (`onlyAdminCanShareMovie`).

## Сводка

4 фичи звонка: `ADD_PARTICIPANT`, `RECORD`, `MOVIE_SHARE`, `ASR_RECORD`. Все с role-based access через `enable-feature-for-roles` сигналинг. `ASR_RECORD` — отдельная фича для записи аудио для транскрипции.
