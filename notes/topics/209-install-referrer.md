---
tags: [telemetry, install-referrer, installer, attribution, critlog]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/d6.java
related:
  - "[[207-critlog-events]]"
  - "[[14-stat-prefs-metrics]]"
---

# GET_INSTALL_REFERRER — источник установки приложения

При первом запуске MAX собирает информацию об источнике установки через `PackageManager.getInstallerPackageName()`.

## Что собирается

| Поле | Что |
|---|---|
| `value` | **имя пакета установщика** (нормализованное: пробелы→`_`, `/`→`_`) |
| `is_update_version` | обновление или новая установка |

## Логика

1. `getInstallerPackageName(packageName)` — получить имя пакета, через который установлено приложение (Google Play = `com.android.vending`, APK = пусто, и т.д.)
2. Нормализация: пробелы и `/` заменяются на `_`
3. Отправляется через CritLog `ACTION`/`GET_INSTALL_REFERRER`
4. Сохраняется в SharedPreferences — повторно не отправляется (проверка `!zm0.c(prevVersion, "26.15.3")`)

## Что важно

1. **Сервер знает, откуда установлено приложение** — из Google Play, APK, другого магазина.

2. **`is_update_version`** — сервер знает, это обновление или новая установка.

3. Отправляется **один раз** при первом запуске новой версии.

## Сводка

`GET_INSTALL_REFERRER`: `getInstallerPackageName()` → нормализация → CritLog. Сервер знает источник установки и тип (новая/обновление).
