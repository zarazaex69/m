---
tags: [root-detection, network-type, emulator-detection, surveillance, security]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/r04.java
related:
  - "[[338-root-detection-impl]]"
  - "[[361-host-reachability-bzk-config]]"
---

# r04 — Root detection + Network type detection

## r04.G() — Root detection

```java
// Проверки:
1. Build.PRODUCT == "sdk" || "google_sdk" → эмулятор
2. android_id == null → эмулятор
3. Build.TAGS содержит "test-keys" → root
4. /system/app/Superuser.apk существует → root
5. /system/xbin/su существует → root
```

Возвращает `true` если устройство рутировано.

## r04.z() — Network type detection

| Тип | Что |
|---|---|
| `NONE` | нет сети |
| `WIFI` | WiFi (transport=1) |
| `ETHERNET` | Ethernet (transport=3) |
| `BLUETOOTH` | Bluetooth (transport=2) |
| `VPN` | VPN (transport=4) |
| `CELLULAR` | мобильная сеть (transport=0) |
| `UNKNOWN` | неизвестно |

Для CELLULAR дополнительно определяет тип (2G/3G/4G/5G) через `TelephonyManager.getDataNetworkType()`.

## Что важно

1. **Root detection** — проверяет `/system/xbin/su` и `/system/app/Superuser.apk`.

2. **Emulator detection** — `Build.PRODUCT == "sdk"` или `android_id == null`.

3. **VPN detection** — `transport=4` → `"VPN"`. Используется для VPN-предупреждений.

4. **`TelephonyManager.getDataNetworkType()`** — тип мобильной сети (2G/3G/4G/5G).

## Сводка

`r04.G()`: root detection (su/Superuser.apk/test-keys). `r04.z()`: network type (NONE/WIFI/ETHERNET/BLUETOOTH/VPN/CELLULAR).
