---
tags: [storage, prefs, persistence, dev-menu]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/r58.java
  - work/jadx_base/sources/defpackage/tvc.java
  - work/jadx_base/sources/defpackage/ng8.java
  - work/jadx_base/sources/defpackage/ypd.java
  - work/jadx_base/sources/defpackage/ygc.java
  - work/jadx_base/sources/defpackage/i2e.java
related:
  - "[[05-dev-menu-in-prod]]"
  - "[[14-stat-prefs-metrics]]"
  - "[[06-contacts]]"
---

# SharedPreferences в MAX — карта файлов

Перечень всех именованных `SharedPreferences`-файлов, которые создаёт клиент. Имена взяты из вызовов `getSharedPreferences("…", 0)` в декомпилированном коде. Содержимое каждого — отдельная тема для верификации; здесь фиксируется, что вообще существует.

## Файлы прямо в коде MAX

| Имя файла | Где лежит | Назначение по коду |
|---|---|---|
| `dev_tools` | `defpackage/r58.java` | DevMenu state — выбранный API-сервер, фича-флаги в обход PmsKey (см. [[05-dev-menu-in-prod]]). Доступен в release-сборке. |
| `stat_prefs` | `defpackage/tvc.java` | server-pushed метрики — `phonebook_size_size_metric` и аналоги (см. [[14-stat-prefs-metrics]]). |
| `presences.pref` | `defpackage/ypd.java` | last-seen / online-presence cache. |
| `permissions_prefs` | `defpackage/ygc.java` | состояние выданных runtime-permission'ов. |
| `app_crash_prefs` | `defpackage/ng8.java`, `defpackage/p2m.java` | `pref_last_crash_time` и связанное. |
| `in_app_review_prefs` | `defpackage/fg8.java` | счётчики триггеров in-app-review (PmsKey `fake-in-app-review` влияет). |
| `keyboard_prefs` | `defpackage/g39.java`, `defpackage/krk.java` | состояние клавиатуры (стикеры/эмодзи panel state). |
| `webrtc-android-sdk-pref` | `defpackage/i2e.java` | WebRTC SDK preferences для звонков. |
| `one.me.sdk.design.theme` | `defpackage/hxb.java` | выбранная тема UI. |
| `tracer` | `defpackage/ij9.java` | apptracer global state (build-uuid, device-token и т. д.). |
| `tracer-<подмодуль>` | `defpackage/kg.java` | per-module apptracer state (heap, anr, perf, sample). Имя формируется конкатенацией. |

## Файлы внешних SDK

`com.google.android.gms.appid`, `com.google.android.gms.signin`, `com.google.firebase.messaging`, `com.google.firebase.common.prefs:<id>`, `FirebaseHeartBeat<suffix>`, `com.google.mlkit.internal`, `androidx.work.util.preferences`, `androidx.work.util.id`, `mytracker_prefs`, `exc_count.prefs`.

Из них `mytracker_prefs` — VK MyTracker (см. [[04-telemetry-endpoints]]). `com.my.tracker.applifecycle` записывает в свои собственные prefs:

- `referrer`, `referrerSent`, `apiReferrerSent` — Play Install Referrer.
- `huaweiApiReferrerSent` — отдельно для Huawei AppGallery (то есть в кодовой базе есть путь и для HMS).
- `preinstallRead` — флаг «прочитан preinstall vendor info» (Predustanovka на устройстве — Min цифры РФ обязывает с 1 сентября 2025; см. вступительную справку).
- `appId`, `appVersion`, `appVersionName`, `installTimestamp` — фингерпринт установки.
- `attribution` — атрибуция (откуда пользователь пришёл, рекламная кампания).

## Точки внимания

- **`dev_tools`** в release-сборке — отдельная тема (см. [[05-dev-menu-in-prod]]). Любой вход в DevMenu — переключение на тестовый API без признаков «debug build».
- **`presences.pref`** — кеш presence нелокальных пользователей. Размер кеша и time-to-live — серверные параметры.
- **`exc_count.prefs`** — счётчики исключений (число ошибок по типам). Нужен tracer-у для rate-limiting non-fatal'ов.
- **`com.my.tracker.applifecycle`** хранит **`preinstallRead`**: то есть код умеет читать pre-installed информацию OEM (vendor app package), что подразумевает реальное использование «обязательной предустановки» как канала атрибуции.
- **`huaweiApiReferrerSent`** — наличие этой ветки означает поддержку Huawei AppGallery distribution path в analytics-стеке. Сами Huawei mobile services (HMS) handler-ы push-сообщений для этой версии я не нашёл (см. [[19-fcm-push-payload]]) — то есть push идёт только через FCM, но атрибуция установки умеет ловить и Huawei.

## Сводка

Хранилище SharedPreferences разделено на три слоя: (1) собственный one.me-стек (DevMenu, статистика, presence, permissions, crash, UI-тема, in-app-review), (2) per-module apptracer (`tracer-<name>`), (3) сторонние SDK (Google, MyTracker, AndroidX, MlKit, Firebase). Чувствительного содержимого (токены, MSISDN, переписки, контакты) в этих файлах не хранится — большое state идёт в БД, прокачивается через WS и попадает в Java-heap. SharedPreferences здесь — это flags/timestamps/counters.

Что осталось проверить отдельно: содержимое DataStore (Jetpack Preferences) и Room-баз, если используются. Это другой слой хранилища.
