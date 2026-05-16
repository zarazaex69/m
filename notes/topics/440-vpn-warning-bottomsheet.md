---
tags: [vpn, warning, surveillance, server-control]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/vpnconnectedwarning/VpnConnectedWarningBottomSheet.java
  - work/apktool_base/res/values/strings.xml
related:
  - "[[02-vpn-warning]]"
  - "[[368-webapp-vpn-exception]]"
---

# VpnConnectedWarningBottomSheet — предупреждение о VPN

`VpnConnectedWarningBottomSheet` — bottom sheet с предупреждением о подключённом VPN.

## Текст

- Заголовок: **"Отключите VPN"**
- Описание: **"Чтобы пользоваться MAX"**

## Что важно

1. Это UI-компонент для серверно-управляемого предупреждения о VPN (см. [[02-vpn-warning]]).

2. Отображается при обнаружении VPN.

3. Связано с `WebAppHttpClient.WebAppHasVpnException` — мини-приложения блокируются при VPN.

## Сводка

`VpnConnectedWarningBottomSheet`: "Отключите VPN" / "Чтобы пользоваться MAX". Серверно-управляемое предупреждение.
