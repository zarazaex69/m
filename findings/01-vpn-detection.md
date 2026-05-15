# Finding: VPN Detection & Reporting

## Summary
Max активно детектит VPN и отправляет эту информацию на сервер.

## Механизм детекции
Класс: `p000/jf4.java` (обфусцированный NetworkMonitor)

```java
// Метод mo2504e() — isVpnActive()
public final boolean mo2504e() {
    NetworkCapabilities caps = getNetworkCapabilities(activeNetwork);
    if (caps != null) {
        return caps.hasTransport(4); // TRANSPORT_VPN = 4
    }
    return false;
}
```

## Что происходит при детекте

### 1. Логирование
```java
String str3 = mo2504e() ? "(VPN detected)" : "";
// Логируется в сетевой мониторинг
```

### 2. Отправка на сервер
Класс `p000/b92.java`:
```java
ob2.m15756l(ob2Var, "BAD_CONNECTION_ALERT", str, "VPN", null, null, null, z, null, 376);
```
Отправляет событие "BAD_CONNECTION_ALERT" с типом "VPN".

### 3. UI предупреждения (контролируются сервером)
- `show-vpn-chat-bottomsheet` — показать предупреждение в чате
- `show-vpn-call-bottomsheet` — показать предупреждение при звонке
- `show-vpn-snackbar` — показать снэкбар

### 4. Экраны предупреждений
- `CALL_VPN_WARNING_SHEET` (id=312) — при звонке
- `CHAT_VPN_WARNING_SHEET` (id=356) — в чате

## Класс VpnConnectedWarningBottomSheet
Показывает bottom sheet с:
- Иконкой (80x80dp)
- Заголовком: `oneme_vpn_connected_title`
- Описанием: `oneme_vpn_connected_description`

## Выводы
1. VPN детектится через стандартный Android API (NetworkCapabilities)
2. Информация о VPN отправляется на сервер как "BAD_CONNECTION_ALERT"
3. Сервер решает показывать ли предупреждение (через PmsKey флаги)
4. Даже если UI не показывается — факт VPN всё равно репортится
