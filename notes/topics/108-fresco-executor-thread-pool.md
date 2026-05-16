---
tags: [threading, server-control, pms, performance, fresco]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/om5.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[03-pms-server-flags]]"
---

# fresco-executor и system-thread-pool-queue — server-controlled thread pool hacks

## system-thread-pool-queue (#286)

`PmsKey.f286systemthreadpoolqueue` — bool, default `false`. Описание: «Disable LinkedTransferQueue34».

`LinkedTransferQueue` — Java concurrent queue. При `true` — отключить использование `LinkedTransferQueue` в системном thread pool. Это performance-hack для устройств с проблемами в реализации `LinkedTransferQueue`.

## fresco-executor (#122)

`PmsKey.f122frescoexecutor` — bool, default `false`. Описание: «Enable Fresco executor-hack».

Fresco — библиотека загрузки изображений от Facebook. При `true` — включить кастомный executor для Fresco. Это performance-hack для оптимизации загрузки изображений.

## Что важно

Оба флага — server-gated performance-хаки. Сервер может включить их для конкретных устройств или версий Android, где стандартное поведение проблематично. Это нормальная практика для крупных приложений, но показывает, что сервер контролирует даже низкоуровневые детали реализации.

## Сводка

`system-thread-pool-queue` — server-gated отключение `LinkedTransferQueue`. `fresco-executor` — server-gated Fresco executor hack. Оба — performance-хаки, управляемые сервером.
