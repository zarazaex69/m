---
tags: [warning-link, messages, safety, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/messages/list/ui/view/WarningLinkBottomSheet.java
  - work/apktool_base/res/values/strings.xml
related:
  - "[[355-deeplink-routes-full]]"
  - "[[447-rkn-bottomsheet]]"
---

# WarningLinkBottomSheet — предупреждение при переходе по ссылке

`WarningLinkBottomSheet` — bottom sheet с предупреждением при переходе по внешней ссылке.

## Текст

- Заголовок: **"Перейти по ссылке?"**
- Тело: URL ссылки (max 300 символов)
- Кнопка 1: **"Перейти"**
- Кнопка 2: **"Отменить"**

## Параметры

- `link_arg` — URL ссылки

## Что важно

1. Предупреждение показывается при переходе по внешним ссылкам из сообщений.

2. URL ограничен 300 символами в отображении.

3. Аналитика: `a(1, 1)` — клик "Перейти", `a(1, 2)` — клик "Отменить".

## Сводка

`WarningLinkBottomSheet(link)`: "Перейти по ссылке?" + URL + "Перейти"/"Отменить". Аналитика переходов.
