---
tags: [deeplinks, intent-filter, maps, navigation, scheme]
status: confirmed
sources:
  - work/apktool_base/AndroidManifest.xml
  - work/apktool_base/res/values/strings.xml
related:
  - "[[13-deeplinks-idp]]"
  - "[[18-manifest-deep-dive]]"
---

# Deep link схемы — полный список из манифеста

## Схемы URI

| Схема | Что |
|---|---|
| `max://` | нативная схема приложения (`app_scheme=max`) |
| `https://max.ru` | веб-схема (`app_host=max.ru`) |
| `http://` / `https://` | общие веб-ссылки |
| `yandexmaps://` | Яндекс.Карты |
| `yandexnavi://` | Яндекс.Навигатор |
| `dgis://` | 2ГИС |
| `petalmaps://` | Petal Maps (Huawei) |

## Что важно

1. **`petalmaps://`** — поддержка Huawei Petal Maps. Это означает, что MAX ориентирован на устройства без Google (Huawei).

2. **`dgis://`** — 2ГИС как альтернатива Яндекс.Картам. Три картографических сервиса: Яндекс.Карты, Яндекс.Навигатор, 2ГИС.

3. **`max://`** — нативная схема. Используется для deeplink-навигации внутри приложения.

4. Все схемы карт — для открытия геолокации из сообщений в соответствующем приложении.

## Сводка

Deep link схемы: `max://`, `https://max.ru`, `yandexmaps://`, `yandexnavi://`, `dgis://`, `petalmaps://`. Поддержка Huawei Petal Maps указывает на ориентацию на российский рынок без Google.
