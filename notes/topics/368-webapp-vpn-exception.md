---
tags: [webapp, vpn-detection, http-client, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/webapp/util/WebAppHttpClient$WebAppHasVpnException.java
related:
  - "[[02-vpn-warning]]"
  - "[[10-webapp-jsbridge]]"
---

# WebAppHttpClient — VPN обнаружение в WebApp

`WebAppHttpClient$WebAppHasVpnException` — исключение при обнаружении VPN в WebApp HTTP клиенте.

## Что важно

1. **`WebAppHasVpnException`** — отдельное исключение для VPN. WebApp HTTP клиент проверяет наличие VPN перед выполнением запросов.

2. Это означает, что мини-приложения могут получать ошибку при попытке выполнить HTTP запрос через VPN.

3. Связано с [[02-vpn-warning]] — серверный контроль VPN-предупреждений.

## Сводка

`WebAppHttpClient.WebAppHasVpnException` — VPN обнаружен в WebApp HTTP клиенте. Мини-приложения блокируются при VPN.
