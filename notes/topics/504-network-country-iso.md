---
tags: [network-country-iso, telephony, locale, surveillance, telemetry]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/bhj.java
related:
  - "[[459-device-data-yk2]]"
  - "[[458-network-operator-collection]]"
---

# bhj.x() — getNetworkCountryIso для определения страны

`bhj.x(context)` — получение кода страны через `TelephonyManager.getNetworkCountryIso()`.

## Логика

```java
String networkCountryIso = telephonyManager.getNetworkCountryIso();
if (!TextUtils.isEmpty(networkCountryIso)) {
    return networkCountryIso.toUpperCase();
}
return Locale.getDefault().getCountry().toUpperCase();
```

Fallback: `Locale.getDefault().getCountry()`.

## Что важно

1. **`getNetworkCountryIso()`** — код страны сети (MCC-based).

2. **Fallback на Locale** — если нет сети, используется локаль устройства.

3. Используется для определения страны пользователя.

## Сводка

`bhj.x()`: `getNetworkCountryIso()` → код страны. Fallback: `Locale.getDefault().getCountry()`.
