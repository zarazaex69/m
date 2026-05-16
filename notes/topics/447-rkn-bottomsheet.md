---
tags: [rkn, roskomnadzor, channel, compliance, surveillance, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/profile/RknBottomSheet.java
  - work/apktool_base/res/values/strings.xml
related:
  - "[[11-state-bots-and-content-policy]]"
  - "[[22-gost-digitalid-family]]"
---

# RknBottomSheet — маркировка каналов РКН

`RknBottomSheet` — bottom sheet с информацией о регистрации канала в Роскомнадзоре.

## Текст

- Заголовок: **"Это зарегистрированный канал"**
- Подзаголовок: **"Он есть в перечне Роскомнадзора"**

## Что важно

1. **РКН** — Роскомнадзор. MAX маркирует каналы, зарегистрированные в реестре РКН.

2. Это означает, что MAX интегрирован с реестром РКН для маркировки контента.

3. Связано с `contentLevelAccess` и `familyProtection` в UserSettings.

## Сводка

`RknBottomSheet`: "Это зарегистрированный канал" / "Он есть в перечне Роскомнадзора". Маркировка каналов РКН.
