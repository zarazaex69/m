---
tags: [webapp, share, haptic, qr-code, jsbridge]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/qlk.java
  - work/jadx_base/sources/defpackage/ddk.java
  - work/jadx_base/sources/defpackage/agk.java
related:
  - "[[419-webapp-js-events]]"
  - "[[422-webapp-screen-capture-qr]]"
---

# WebApp Share + Haptic + CodeReader — детали

## WebAppShare

`WebAppShareRequest(url, title, text)` — мини-приложение инициирует шаринг.

## WebAppHapticFeedback

Три типа тактильной обратной связи:

| Тип | Что |
|---|---|
| `IMPACT_OCCURED` | удар |
| `NOTIFICATION_OCCURED` | уведомление |
| `SELECTION_CHANGED` | изменение выбора |

Статусы: `impactOccured`, `notificationOccured`, `selectionChanged`.

## WebAppOpenCodeReader

`WebAppOpenCodeReaderResponse(requestId, value)` — результат сканирования QR/штрих-кода.

## Что важно

1. **`WebAppShareRequest(url, title, text)`** — мини-приложение может инициировать шаринг с произвольным URL, заголовком и текстом.

2. **Haptic feedback** — мини-приложения могут управлять тактильной обратной связью устройства.

## Сводка

`WebAppShare(url, title, text)`. `WebAppHapticFeedback`: IMPACT/NOTIFICATION/SELECTION. `WebAppOpenCodeReader` → `(requestId, value)`.
