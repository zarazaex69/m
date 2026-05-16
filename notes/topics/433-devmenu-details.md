---
tags: [devmenu, debug, server-switch, logs-viewer, feature-toggles, surveillance, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/devmenu/DevMenuScreen.java
  - work/jadx_base/sources/one/me/devmenu/DevMenuFeatureTogglesPageScreen.java
  - work/jadx_base/sources/one/me/devmenu/DevMenuInfoScreen.java
  - work/jadx_base/sources/one/me/devmenu/tools/server/ServerHostBottomSheet.java
  - work/jadx_base/sources/one/me/devmenu/tools/server/ServerPortBottomSheet.java
  - work/jadx_base/sources/one/me/devmenu/logsviewer/LogsViewerScreen.java
  - work/jadx_base/sources/one/me/devmenu/tools/TestCrash.java
related:
  - "[[05-dev-menu-in-prod]]"
  - "[[344-pmskey-debug-special]]"
---

# DevMenu — детали реализации

`DevMenuScreen` — dev-меню в production сборке. 3 вкладки: General, FeatureToggles, Info.

## Вкладки

### DevMenuGeneralPageScreen

Общие настройки. Список элементов с возможностью переключения.

### DevMenuFeatureTogglesPageScreen

Переключение фича-флагов. Поддерживает типы: Boolean, Long, Integer, String, JSON.

Для каждого флага показывает: текущее значение, источник (`"SERVER"` или локальный).

### DevMenuInfoScreen

Информация об устройстве: PerfClass, системная информация.

## Инструменты

### ServerHostBottomSheet

- Заголовок: **"Адрес сервера"**
- Список серверов + поле ввода кастомного адреса
- Кнопка "Установить"

### ServerPortBottomSheet

- Заголовок: **"Порт сервера"**
- Поле ввода кастомного порта
- Кнопка "Установить"

### LogsViewerScreen

Просмотр логов с поиском (EditText + EndlessRecyclerView).

### MemoryDebuggerScreen

Отладчик памяти (пустой View — заглушка).

### TestCrash

`TestCrash extends RuntimeException` — тестовый крэш.

## Что важно

1. **`ServerHostBottomSheet`** — в production сборке есть UI для смены адреса сервера.

2. **`DevMenuFeatureTogglesPageScreen`** — все фича-флаги (включая PmsKey) можно переключить вручную.

3. **Источник `"SERVER"`** — флаги показывают, пришли ли они с сервера.

4. **`LogsViewerScreen`** — просмотр логов прямо в приложении.

## Сводка

DevMenu: 3 вкладки (General/FeatureToggles/Info). Инструменты: ServerHost/ServerPort (смена сервера), LogsViewer, MemoryDebugger, TestCrash. В production сборке.
