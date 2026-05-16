---
tags: [deeplinks, routing, max-ru, joincall, stickerset, auth, folder]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/gb9.java
related:
  - "[[13-deeplinks-idp]]"
  - "[[222-deeplink-schemes]]"
---

# gb9.java — полный список deeplink маршрутов

`gb9.java` — основной обработчик deeplink-ов MAX.

## Обрабатываемые URL

| URL | Что |
|---|---|
| `https://max.ru/:auth` | **авторизация** (IdP) |
| `https://max.ru/:share-self-out` | **шаринг из внешнего приложения** |
| `https://max.ru/joincall/<id>` | **присоединиться к звонку** |
| `https://max.ru/join/<id>` | **присоединиться** |
| `https://max.ru/stickerset/<id>` | **набор стикеров** |
| `https://max.ru/c/<chatId>/<msgId>` | **чат + сообщение** (3 сегмента) |
| `https://max.ru/:folder` | **папка** |
| `https://max.ru/:current` | **текущий экран** |
| `max://max.ru/` | нативная схема |

## Что важно

1. **`:share-self-out`** — шаринг из внешнего приложения в MAX. Это обработчик для системного шаринга.

2. **`joincall/<id>`** — присоединение к звонку по ссылке. Связано с [[144-hangup-join-api]].

3. **`c/<chatId>/<msgId>`** — прямая ссылка на сообщение в чате.

4. **`stickerset/<id>`** — установка набора стикеров по ссылке.

## Сводка

Deeplink маршруты: `:auth`/`:share-self-out`/`joincall/<id>`/`join/<id>`/`stickerset/<id>`/`c/<chatId>/<msgId>`/`:folder`/`:current`.
