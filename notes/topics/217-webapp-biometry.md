---
tags: [webapp, biometry, jsbridge, token, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/t54.java
  - work/jadx_base/sources/defpackage/fj3.java
  - work/jadx_base/sources/defpackage/r6k.java
  - work/jadx_base/sources/defpackage/u9k.java
related:
  - "[[10-webapp-jsbridge]]"
  - "[[216-local-db-tables]]"
  - "[[207-critlog-events]]"
---

# WebApp Biometry — биометрия в мини-приложениях

Мини-приложения могут запрашивать биометрическую аутентификацию через JS-bridge.

## Таблица webapp_biometry

| Поле | Что |
|---|---|
| `id` | локальный ID |
| `user_id` | ID пользователя |
| `bot_id` | **ID мини-приложения** |
| `token` | **токен биометрии** |
| `access_requested` | запрошен ли доступ |
| `access_granted` | **предоставлен ли доступ** |

## JS-bridge методы биометрии

| Класс | Метод |
|---|---|
| `WebAppBiometryGetInfoRequest` | получить информацию о биометрии |
| `WebAppBiometryAccessRequest` | запросить доступ к биометрии |
| `WebAppBiometryAuthRequest` | **аутентификация через биометрию** |
| `WebAppBiometryUpdateTokenRequest` | **обновить токен биометрии** |
| `WebAppBiometryOpenSettingsRequest` | открыть настройки биометрии |
| `WebAppBiometryInfoResponse` | ответ с информацией |
| `WebAppBiometryAuthResponse` | ответ аутентификации |
| `WebAppBiometryUnavailableResponse` | биометрия недоступна |

## Что важно

1. **`token`** — мини-приложение получает токен после биометрической аутентификации. Этот токен хранится в БД и может использоваться повторно.

2. **`WebAppBiometryUpdateTokenRequest`** — мини-приложение может обновить токен. Это означает, что токен привязан к конкретному мини-приложению (`bot_id`).

3. **`access_granted`** — флаг разрешения хранится локально. Мини-приложение не запрашивает разрешение повторно.

4. Логируется через CritLog `MINIAPP_BIOMETRY` (см. [[207-critlog-events]]).

## Сводка

`webapp_biometry`: user_id/bot_id/token/access_requested/access_granted. 8 JS-bridge методов. Мини-приложения получают биометрический токен, хранящийся в БД.
