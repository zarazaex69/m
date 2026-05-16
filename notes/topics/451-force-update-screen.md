---
tags: [force-update, killswitch, server-control, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/appupdate/forceupdate/ForceUpdateScreen.java
  - work/apktool_base/res/values/strings.xml
related:
  - "[[12-force-update-killswitch]]"
---

# ForceUpdateScreen — принудительное обновление

`ForceUpdateScreen` — экран принудительного обновления (killswitch).

## Текст

- Заголовок: **"Ваша версия MAX устарела"**
- Подзаголовок: **"Писать и звонить в этой версии не получится. Обновите приложение, чтобы продолжить использовать MAX"**
- Кнопка: **"Обновить"**

## Что важно

1. Полностью блокирует использование приложения.

2. Кнопка "Обновить" ведёт на `download.max.ru` (мимо Google Play, см. [[12-force-update-killswitch]]).

3. Управляется сервером через PmsKey.

## Сводка

`ForceUpdateScreen`: "Ваша версия MAX устарела" / "Обновить". Killswitch — полная блокировка приложения.
