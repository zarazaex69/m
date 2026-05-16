---
tags: [webapp, contact-data, jsbridge, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/yr7.java
  - work/jadx_base/sources/defpackage/mel.java
related:
  - "[[419-webapp-js-events]]"
  - "[[421-webapp-request-phone]]"
  - "[[06-contacts]]"
---

# WebAppContactData — данные контакта для мини-приложений

`WebAppContactData` — объект с данными контакта, передаваемый мини-приложению.

## Поля

| Поле | Что |
|---|---|
| `displayName` | **отображаемое имя** |
| `avatarUrl` | **URL аватара** |
| `abbreviationModel` | модель аббревиатуры |

## Контекст

`GetWebAppContactDataUseCase` — use case для получения данных контакта для мини-приложения.

## Что важно

1. **`displayName`** — мини-приложение получает имя пользователя.

2. **`avatarUrl`** — мини-приложение получает URL аватара пользователя.

3. Это дополняет `WebAppRequestPhone` — мини-приложение может получить имя + аватар + телефон.

## Сводка

`WebAppContactData(displayName, avatarUrl, abbreviationModel)`. Мини-приложение получает данные профиля пользователя.
