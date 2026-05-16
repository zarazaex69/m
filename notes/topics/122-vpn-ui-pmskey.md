---
tags: [vpn, server-control, pms, anti-vpn, ui]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/nd1.java
  - work/jadx_base/sources/defpackage/rtd.java
  - work/jadx_base/sources/ru/ok/tamtam/android/prefs/PmsKey.java
related:
  - "[[02-vpn-warning]]"
  - "[[03-pms-server-flags]]"
  - "[[41-server-side-client-diagnostic-struct]]"
---

# VPN UI PmsKey — три уровня VPN-предупреждений

В дополнение к [[02-vpn-warning]] — три отдельных PmsKey для VPN-предупреждений:

| Ключ | # | Default | Что |
|---|---|---|---|
| `show-vpn-snackbar` | 273 | false | показывать snackbar о VPN |
| `show-vpn-chat-bottomsheet` | 272 | int | показывать bottomsheet о VPN в чатах |
| `show-vpn-call-bottomsheet` | 271 | int | показывать bottomsheet о VPN в звонках |

`vpnChatBottomsheetEnabled` и `vpnCallBottomsheetEnabled` — int (не bool). Это означает несколько режимов: 0=выключено, 1=мягкое предупреждение, 2=жёсткое предупреждение, и т.п.

## Что важно

Три независимых механизма VPN-предупреждений:
1. `show-vpn-snackbar` — тихий snackbar
2. `show-vpn-chat-bottomsheet` — bottomsheet в чатах (с несколькими режимами)
3. `show-vpn-call-bottomsheet` — bottomsheet в звонках

Сервер может включить разные уровни предупреждений для разных контекстов. Это расширение [[02-vpn-warning]] (там был один флаг).

## Сводка

Три server-gated VPN-предупреждения: snackbar, chat bottomsheet (int-режимы), call bottomsheet (int-режимы). Сервер контролирует интенсивность давления на пользователей с VPN.
