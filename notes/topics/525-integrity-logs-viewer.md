---
tags: [integrity-logs, devmenu, debug, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/devmenu/logsviewer/IntegrityLogsViewerScreen.java
  - work/jadx_base/sources/defpackage/lp8.java
related:
  - "[[05-dev-menu-in-prod]]"
  - "[[433-dev-menu]]"
---

# IntegrityLogsViewerScreen — просмотр логов целостности

`IntegrityLogsViewerScreen` — экран просмотра "логов целостности" в DevMenu.

## Что делает

- Заголовок: `"Логи целостности"`
- Отображает список логов через `EndlessRecyclerView2`
- Адаптер `lp8` — список `ArrayList<CharSequence>`
- Паттерн для поиска в логах: `\b([\w\-\.]+\.(dex|so))\b` — ищет имена `.dex` и `.so` файлов

## Что важно

1. **Паттерн `\b([\w\-\.]+\.(dex|so))\b`** — логи целостности содержат имена `.dex` и `.so` файлов. Это логи проверки целостности APK/нативных библиотек.

2. **DevMenu в production** — `IntegrityLogsViewerScreen` доступен через DevMenu в release-сборке.

3. Кнопка "поделиться" (ImageView с иконкой) — логи можно экспортировать.

## Сводка

`IntegrityLogsViewerScreen` — просмотр логов целостности `.dex`/`.so` файлов в DevMenu (production).
