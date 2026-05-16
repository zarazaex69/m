---
tags: [server-control, vpn, proxy, network, session-config]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/ksg.java
related:
  - "[[02-vpn-warning]]"
  - "[[57-ws-session-config-fingerprint]]"
  - "[[140-conversation-params]]"
---

# WS Session Config — расширенные поля (ksg)

`ksg` — расширенная структура WS-сессии. Дополняет [[57-ws-session-config-fingerprint]].

## Поля

| Поле | Что |
|---|---|
| `proxy` | **прокси-сервер** |
| `proxy-domains` | **список прокси-доменов** |
| `isVpn` | **VPN обнаружен** |
| `location` | **код страны по геолокации** |
| `reg-country-code` | **код страны регистрации** |
| `lang` | язык |
| `callsSeed` | seed для звонков |
| `app-update-type` | тип обновления приложения |

## Что важно

1. **`isVpn`** — сервер знает, использует ли пользователь VPN. Это подтверждение [[02-vpn-warning]].

2. **`proxy`** и **`proxy-domains`** — сервер знает прокси-настройки устройства.

3. **`location`** (код страны) и **`reg-country-code`** (код страны регистрации) — два разных кода страны. Сервер может сравнивать их для обнаружения несоответствий.

4. **`callsSeed`** — seed для звонков. Используется для инициализации звонкового стека.

## Сводка

WS session config: `proxy`/`proxy-domains`/`isVpn`/`location`(geo country)/`reg-country-code`/`lang`/`callsSeed`/`app-update-type`. Сервер знает VPN, прокси и страну пользователя.
