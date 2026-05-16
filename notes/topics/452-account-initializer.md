---
tags: [initialization, startup, account, dps, ssl-integrity, background-wake, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/android/initialization/AccountInitializer.java
related:
  - "[[08-background-wake]]"
  - "[[415-dps-client-zu5]]"
  - "[[443-calls-sdk-initializer]]"
---

# AccountInitializer — инициализация аккаунта

`AccountInitializer` — инициализация аккаунта при запуске приложения. Параллельные задачи через ForkJoin.

## Задачи инициализации

| Задача | Что |
|---|---|
| `FrescoStartup` | инициализация Fresco (изображения) |
| `Account` | инициализация аккаунта |
| `SetupRx` | настройка RxJava |
| `RxJavaPlugins.setErrorHandler` | обработчик ошибок RxJava |
| `Chroma.init` | инициализация темы |
| `InitialDataStorage.Banners` | загрузка баннеров |
| `InitialDataStorage.Chats` | **загрузка чатов** |
| `InitialDataStorage.Folders` | **загрузка папок** |
| `LegacyChats` | загрузка legacy чатов |
| `Legacy.StartupListeners` | startup listeners |
| `Db.NotMainThreadListener` | DB listener |
| `SslIntegrity` | **проверка SSL целостности** |
| `BackgroundWakeFeatureInit` | инициализация фоновой работы |

## DPS инициализация

```java
accountInitializer.dps = aVarA.I(xm5Var == xm5.HIGH).N(new p6(accountInitializer)).e();
```

DPS инициализируется с `xm5.HIGH` (высокий приоритет).

## Что важно

1. **`SslIntegrity`** — проверка SSL целостности при каждом запуске.

2. **`DPS`** — инициализируется с высоким приоритетом.

3. **`getUserId()`** — возвращает ID пользователя из `yag.s()`.

4. **`android_id`** — возвращается из `yag.m0[45]`.

## Сводка

`AccountInitializer`: 13 параллельных задач. `SslIntegrity` + `DPS(HIGH)` + `BackgroundWakeFeatureInit`. `getUserId()` + `android_id`.
