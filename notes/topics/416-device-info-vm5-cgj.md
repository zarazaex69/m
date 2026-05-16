---
tags: [device-info, user-agent, android-id, fingerprint, surveillance, telemetry]
status: confirmed
sources:
  - work/jadx_base/sources/defpackage/vm5.java
  - work/jadx_base/sources/defpackage/cgj.java
related:
  - "[[04-telemetry-endpoints]]"
  - "[[385-mytracker-details]]"
  - "[[415-dps-client-zu5]]"
---

# vm5 + cgj — Device Info и Android ID

## vm5 — менеджер устройства

### android_id

```java
Settings.Secure.getString(contentResolver, "android_id")
```

Если пустой → генерирует `UUID.randomUUID()` и сохраняет в SharedPreferences.

### cgj j() — UserAgent объект

Собирает:
- `Build.SUPPORTED_ABIS` — поддерживаемые ABI
- `Build.VERSION.RELEASE` — версия Android
- `ej9.d(context).getLanguage()` — язык устройства
- `Build.MANUFACTURER + " " + Build.MODEL` — производитель + модель
- `displayMetrics.densityDpi` — DPI (ldpi/mdpi/hdpi/xhdpi/xxhdpi/xxxhdpi)
- `displayMetrics.widthPixels + "x" + displayMetrics.heightPixels` — разрешение экрана

## cgj — UserAgent

| Поле | Что |
|---|---|
| `a` | SDK_TYPE_STRING |
| `b` | версия приложения (`26.15.3`) |
| `c` | buildNumber |
| `d` | appKey |
| `e` | osVersion |
| `f` | locale |
| `g` | deviceLocale |
| `h` | deviceName (`MANUFACTURER MODEL`) |
| `i` | screen (`DPI WxH`) |
| `j` | pushDeviceType |
| `k` | TimeZone |

## Что важно

1. **`android_id`** — собирается через `Settings.Secure`. Если пустой — генерируется UUID и сохраняется.

2. **`Build.SUPPORTED_ABIS`** — список поддерживаемых ABI (arm64-v8a, armeabi-v7a, x86, x86_64).

3. **`TimeZone.getDefault()`** — часовой пояс устройства.

4. **`displayMetrics.widthPixels + "x" + displayMetrics.heightPixels`** — разрешение экрана.

5. **`Build.MANUFACTURER + " " + Build.MODEL`** — производитель и модель.

## Сводка

`vm5.a()` → `android_id` (Settings.Secure). `vm5.j()` → `cgj(appVersion, buildNumber, osVersion, locale, deviceLocale, deviceName, screen, pushDeviceType, timeZone)`.
