---
tags: [mytracker, installed-packages, apps-list, surveillance, fingerprint]
status: confirmed
sources:
  - work/jadx_base/sources/com/my/tracker/core/o/f.java
  - work/jadx_base/sources/com/my/tracker/core/o/s.java
related:
  - "[[385-mytracker-details]]"
  - "[[417-mytracker-gaid-oaid]]"
---

# MyTracker — сбор списка установленных приложений

`f.java` (AppsDataProvider) — сбор списка установленных приложений для MyTracker.

## Логика

1. `installedPackagesProvider.getInstalledPackages()` — получить список пакетов
2. Фильтр: `(applicationInfo.flags & 1) == 0` — только **не-системные** приложения
3. Для каждого: `applicationInfo.packageName` + `packageInfo.firstInstallTime`
4. Хэш списка (`appsHash`) сравнивается с предыдущим — отправляется только при изменении

## Данные

| Поле | Что |
|---|---|
| `packageName` | **имя пакета** приложения |
| `firstInstallTime` | **время первой установки** |

## Что важно

1. **Только не-системные приложения** — `flags & 1 == 0` исключает системные.

2. **`firstInstallTime`** — время установки каждого приложения.

3. **`appsHash`** — хэш списка. Отправляется только при изменении (новое/удалённое приложение).

4. Список передаётся через `InstalledPackagesProvider` — интерфейс, который приложение должно реализовать.

## Сводка

`AppsDataProvider`: список не-системных приложений (packageName + firstInstallTime). Хэш `appsHash` — отправляется при изменении.
