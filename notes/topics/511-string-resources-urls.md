---
tags: [app-host, url-scheme, deeplinks, surveillance, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/j5f.java
  - work/apktool_base/res/values/strings.xml
related:
  - "[[355-deeplink-routes-full]]"
  - "[[13-deeplinks-idp]]"
  - "[[363-google-maps-api-key]]"
---

# j5f — Строковые ресурсы (URL-схемы и хосты)

## Ключевые строки

| Ресурс | Значение | Что |
|---|---|---|
| `app_host` | `max.ru` | **основной хост** |
| `app_scheme` | `max` | **URL-схема** (`max://`) |
| `web_scheme` | `https` | веб-схема |
| `google_api_key` | `AIzaSyABuDYeeDXIOrKTXLkUj30Ii143ofPe63Q` | **Google API key** |
| `google_crash_reporting_api_key` | `AIzaSyABuDYeeDXIOrKTXLkUj30Ii143ofPe63Q` | Google Crash Reporting |

## Ссылки (из j5f)

| Ресурс | Что |
|---|---|
| `link_host_api` | API хост |
| `link_startwebapp_path` | путь для запуска WebApp |
| `link_stickerset_path` | путь для стикер-сетов |
| `link_private_channel_path` | путь для приватных каналов |
| `link_private_contact_path` | путь для приватных контактов |
| `link_call_join_path` | путь для входа в звонок |
| `link_chat_join_path` | путь для входа в чат |
| `link_chat_path` | путь чата |
| `auth_path` | путь авторизации |
| `folder_path` | путь папки |

## Что важно

1. **`max://`** — кастомная URL-схема для deeplinks.

2. **`max.ru`** — основной хост.

3. **`google_api_key`** — тот же ключ что и `google_crash_reporting_api_key`: `AIzaSyABuDYeeDXIOrKTXLkUj30Ii143ofPe63Q`.

4. Ссылки: `max://startwebapp/...`, `max://stickerset/...`, `max://joincall/...`.

## Сводка

`app_host=max.ru`, `app_scheme=max`. Google API key: `AIzaSyABuDYeeDXIOrKTXLkUj30Ii143ofPe63Q`.
