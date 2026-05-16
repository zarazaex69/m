---
tags: [devmenu, device-info, debug, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/devmenu/DevMenuInfoScreen.java
  - work/jadx_base/sources/one/me/devmenu/threadsviewer/ThreadsStateViewerScreen.java
  - work/jadx_base/sources/one/me/devmenu/memorydebugger/MemoryDebuggerScreen.java
related:
  - "[[05-dev-menu-in-prod]]"
  - "[[433-dev-menu]]"
  - "[[527-devmenu-feature-toggles]]"
---

# DevMenuInfoScreen + ThreadsStateViewerScreen — DevMenu дополнительные экраны

## DevMenuInfoScreen

Экран информации об устройстве в DevMenu.

### Данные об устройстве

```
Об устройстве:
  PerfClass: <performance class>
  DefaultDensity: <density>
  CurrentDensity: <density>
  DensityDpi: <dpi>
```

Обновляется при каждом `onActivityStarted`.

## ThreadsStateViewerScreen

Экран просмотра состояния потоков. Использует `nmi` ViewModel для получения состояния потоков.

## MemoryDebuggerScreen

Пустой экран (заглушка): `return new View(layoutInflater.getContext())`.

## Что важно

1. **`DevMenuInfoScreen`** — показывает PerfClass/Density/DensityDpi устройства.

2. **`ThreadsStateViewerScreen`** — просмотр состояния потоков в production.

3. **`MemoryDebuggerScreen`** — заглушка, не реализована.

## Сводка

DevMenu: `DevMenuInfoScreen` (PerfClass/Density), `ThreadsStateViewerScreen` (потоки), `MemoryDebuggerScreen` (заглушка).
