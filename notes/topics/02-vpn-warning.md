---
tags: [anti-vpn, ux-pressure, server-control, surveillance]
status: confirmed
sources:
  - work/jadx_base/sources/one/me/vpnconnectedwarning/**
  - findings/raw/pms_keys.txt
  - work/jadx_base/sources/defpackage/ura.java
related:
  - "[[03-pms-server-flags]]"
  - "[[10-webapp-jsbridge]]"
  - "[[04-telemetry-endpoints]]"
---

# «Отключите VPN» — нагибание пользователя в сторону открытой сети

## Что показывает приложение

В `res/values-ru/strings.xml` (и базовом):

```
oneme_vpn_connected_title = «Отключите VPN»
oneme_vpn_connected_description = «Чтобы пользоваться MAX»
call_screen_snackbar_title = «Лучше без VPN»
```

## Где живёт UI

`one/me/vpnconnectedwarning/VpnConnectedWarningBottomSheet.java` — отдельный модуль `vpn-connected-warning_release`. UI-плашка с иконкой, заголовком, описанием.

Сценарии запуска плашки/snackbar (вызовы `new VpnConnectedWarningBottomSheet(...)` найдены в):

- `one/me/contactlist/ContactListWidget.java` — список контактов
- `one/me/calllist/ui/CallHistoryScreen.java` — история звонков
- `one/me/startconversation/...` (через `defpackage/doh.java`) — экран новой беседы (с `y6g.CALL_VPN_WARNING_SHEET`)
- `one/me/chats/...` (через `defpackage/nc3.java`) — открытие чата (`y6g.CHAT_VPN_WARNING_SHEET`), и снова call-вариант, в зависимости от контекста
- `defpackage/vwf.java` — call history page

То есть пользователю показывают плашку «отключите VPN» **в чате**, **в начале новой беседы**, **в истории звонков**, **в списке контактов** и **на экране звонка**. Это не один редкий screen, а везде где есть сетевая активность.

## Как детектится VPN

Файл `defpackage/jf4.java` — детектор сети:

```java
NetworkCapabilities networkCapabilitiesJ = ...;
if (networkCapabilitiesJ != null) {
    return networkCapabilitiesJ.hasTransport(4); // TRANSPORT_VPN = 4
}
return false;
```

И ещё:

```java
new gf4(z2,
    !networkCapabilitiesJ.hasCapability(...),
    ...,
    z,
    networkCapabilitiesJ.hasTransport(4));   // <- VPN flag
String str3 = e() ? "(VPN detected)" : "";
```

Внутренний лог явно пишет «(VPN detected)». Дальше состояние едет в `b4k` (`defpackage/b4k.java`).

## Как это управляется с сервера

`b4k.a()`:

```java
public final boolean a() {
    rtd rtdVar = (rtd) ((apg) this.b.getValue());
    int iIntValue = ((Number) rtdVar.d0.x(rtdVar, rtd.e0[45])).intValue();
    v49 v49Var = this.a;
    return iIntValue != 1
            ? iIntValue != 2
                ? iIntValue == 3 && ((bf4) v49Var.getValue()).e() && this.c
                : this.c
            : ((bf4) v49Var.getValue()).e();
}
```

`rtd.e0[45]` = `vpnCallBottomsheetEnabled` (Int).
`rtd.e0[44]` = `vpnChatBottomsheetEnabled` (Int).
В PmsKey:

- `show-vpn-chat-bottomsheet`
- `show-vpn-call-bottomsheet`
- `show-vpn-snackbar`

Это **серверно-управляемые** параметры. Логика:

| Значение | Поведение |
|---|---|
| `1` | плашка появляется всегда, как только видим TRANSPORT_VPN |
| `2` | только если внутренний флаг `this.c == true` (сервер в чате/звонке отдельно «попросил») |
| `3` | VPN активен И `this.c` |
| иное | плашка не показывается |

То есть: **сервер MAX в любой момент может «закрутить» плашку до уровня 1 — навязчивая блокировка-предупреждение при каждом открытии чата/звонка, пока пользователь не выключит VPN.**

## Что это говорит про продукт

1. MAX **активно** отслеживает наличие VPN-туннеля у клиента и реагирует UX-ом.
2. Реакция — **давить на отключение**: «Отключите VPN. Чтобы пользоваться MAX».
3. Показ управляется **с сервера**, без обновления приложения. Сценарии — все ключевые: чат, звонок, контакт-лист, история звонков, новая беседа.
4. Плюс к этому, в network-statistics (`defpackage/ura.java`) сам факт VPN сохраняется как отдельная категория сетевого транспорта (`"vpn"`) и едет в аналитику. То есть **факт использования VPN репортится в телеметрию**.

## Кратко для статьи

> «У MAX есть отдельный модуль `vpn-connected-warning_release` с готовой плашкой "Отключите VPN. Чтобы пользоваться MAX". Она вызывается на пяти разных экранах: при открытии чата, при звонке, в истории звонков, в списке контактов и на экране начала новой беседы. Решает, показывать ли её, серверный флаг `show-vpn-chat-bottomsheet` / `show-vpn-call-bottomsheet` / `show-vpn-snackbar`: значение `1` — навязчивый показ при каждом действии. Параллельно факт активного TRANSPORT_VPN на телефоне отдельно записывается в сетевую статистику и улетает в аналитику.»
