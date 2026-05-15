# Max Messenger Reverse Engineering - Overview

## Target
- **App**: MAX (мессенджер)
- **Package**: ru.oneme.app
- **Version**: 26.15.3 (versionCode 6695)
- **Developer**: Communication Platform LLC (дочка VK)
- **Min SDK**: 26 (Android 8.0)
- **Target SDK**: 35

## APK Structure
- `ru.oneme.app.apk` (28MB) — base APK (DEX + resources)
- `config.arm64_v8a.apk` (25MB) — native libs для arm64
- `config.xxhdpi.apk` (6MB) — ресурсы для xxhdpi

## Key Packages (decompiled)
| Package | Description |
|---------|-------------|
| `one.p011me.*` | Основной код Max (UI, логика) |
| `ru.p027ok.tamtam.android.*` | Код от ТамТам (прямое наследие) |
| `ru.p027ok.android.webrtc.*` | WebRTC реализация (звонки) |
| `ru.p027ok.android.externcalls.*` | Внешние звонки SDK |
| `one.video.calls.sdk.*` | Video calls SDK |
| `p000.*` | Обфусцированный код (ProGuard/R8) |

## Permissions (56 total, key ones)
- CAMERA, RECORD_AUDIO, READ_CONTACTS, WRITE_CONTACTS
- ACCESS_FINE_LOCATION, ACCESS_COARSE_LOCATION
- NFC, USE_BIOMETRIC
- SYSTEM_ALERT_WINDOW
- com.google.android.gms.permission.AD_ID
- CHANGE_NETWORK_STATE

## Initial Findings
1. **VPN Detection**: `NetworkCapabilities.hasTransport(4)` в классе `p000/jf4.java`
2. **VPN Warning UI**: `VpnConnectedWarningBottomSheet` — показывает предупреждение при VPN
3. **Server-side flags**: PmsKey содержит `show-vpn-chat-bottomsheet`, `show-vpn-call-bottomsheet`, `show-vpn-snackbar`
4. **Host Reachability**: `host-reachability` — проверка доступности внешних хостов
5. **GOST Check**: `gost-check-env` — проверка ГОСТ-окружения (сертификаты Минцифры?)
6. **MyTracker**: `mytracker-enabled` — трекер от VK/Mail.ru
7. **TamTam Heritage**: Весь пакет `ru.p027ok.tamtam.android` — прямой код ТамТам
