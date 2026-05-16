---
tags: [installer-package, mytracker, user-id, surveillance, telemetry, initialization]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/d6.java
related:
  - "[[452-account-initializer]]"
  - "[[385-mytracker-details]]"
  - "[[479-action-events]]"
---

# d6 — Инициализация приложения (installer + MyTracker)

`d6` — задача инициализации при запуске приложения.

## Ключевые действия

### getInstallerPackageName

```java
String installer = getPackageManager().getInstallerPackageName(packageName);
// Нормализация: пробелы → '_', '/' → '_'
ws9Var.put("is_update_version", bool);
ws9Var.put("value", normalizedInstaller);
ok9.h("ACTION", "GET_INSTALL_REFERRER", data);
```

### MyTracker инициализация

```java
MyTracker.getTrackerParams().setCustomUserId(String.valueOf(userId));
MyTracker.getTrackerConfig()
  .setOkHttpClientProvider(...)
  .setKidMode(false)
  .setBackgroundExecutor(...);
MyTracker.setAttributionListener(...);
MyTracker.getInstanceId(application);  // → yag.e.k
```

### Receivers

- `DATE_CHANGED` / `TIME_SET` / `TIMEZONE_CHANGED` / `LOCALE_CHANGED`

## Что важно

1. **`getInstallerPackageName`** — источник установки (Google Play / APK / другой магазин).

2. **`MyTracker.setCustomUserId(userId)`** — userId передаётся в MyTracker.

3. **`is_update_version`** — флаг обновления в аналитике.

4. **`setKidMode(false)`** — детский режим MyTracker отключён.

## Сводка

`d6`: `getInstallerPackageName` → `ACTION.GET_INSTALL_REFERRER`. `MyTracker.setCustomUserId(userId)`. Receivers: DATE/TIME/TIMEZONE/LOCALE.
