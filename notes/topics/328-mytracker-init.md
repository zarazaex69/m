---
tags: [telemetry, mytracker, init, user-id, tracker-id, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/d6.java
  - work/jadx_base/sources/defpackage/qqb.java
related:
  - "[[04-telemetry-endpoints]]"
  - "[[231-mytracker-params]]"
  - "[[327-mytracker-endpoints]]"
---

# MyTracker инициализация — tracker ID и user ID

## Инициализация

```java
MyTracker.getTrackerParams().setCustomUserId(String.valueOf(userId));
MyTracker.getTrackerConfig()
    .setOkHttpClientProvider(...)
    .setKidMode(false)
    .setBackgroundExecutor(...)
    .setLogger(...);
MyTracker.setAttributionListener(...);
MyTracker.initTracker("34982109644049932883", application);
```

## Tracker ID

**`34982109644049932883`** — ID приложения в MyTracker. Это публичный идентификатор MAX в системе MyTracker.

## Что важно

1. **`setCustomUserId(userId)`** — ID пользователя MAX передаётся в MyTracker. Это связывает аналитику MyTracker с конкретным пользователем.

2. **`setKidMode(false)`** — режим для детей отключён.

3. **Tracker ID `34982109644049932883`** — уникальный идентификатор приложения в MyTracker/VK Analytics.

## Сводка

MyTracker init: `initTracker("34982109644049932883", app)` + `setCustomUserId(userId)` + `setKidMode(false)`.
