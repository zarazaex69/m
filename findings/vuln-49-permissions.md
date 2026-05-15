# VULN-49: Permission Usage & Potential Abuse Analysis

**App:** Max Messenger (ru.oneme.app)  
**Date:** 2026-05-15  
**Severity:** Medium-High (over-privileged, sensitive data collection)

---

## 1. Complete Permission List (AndroidManifest.xml)

### Dangerous Permissions (Runtime)
| Permission | Justification |
|---|---|
| `ACCESS_FINE_LOCATION` | Location sharing in messages |
| `ACCESS_COARSE_LOCATION` | Location sharing fallback |
| `ACCESS_BACKGROUND_LOCATION` | Live location sharing (dynamically added on SDK 29+) |
| `CAMERA` | Photo/video capture, video calls, QR scanner |
| `RECORD_AUDIO` | Voice messages, audio/video calls |
| `READ_CONTACTS` | Contact sync |
| `WRITE_CONTACTS` | Contact sync (write-back) |
| `READ_PHONE_STATE` | Network type detection, call management |
| `READ_EXTERNAL_STORAGE` | Media access (maxSdkVersion=32) |
| `READ_MEDIA_IMAGES` | Photo gallery access |
| `READ_MEDIA_VIDEO` | Video gallery access |
| `READ_MEDIA_VISUAL_USER_SELECTED` | Partial media access (Android 14+) |
| `WRITE_EXTERNAL_STORAGE` | File saving (maxSdkVersion=29) |
| `BLUETOOTH_CONNECT` | Audio routing during calls |

### Normal/Signature Permissions
| Permission | Purpose |
|---|---|
| `INTERNET` | Network communication |
| `ACCESS_NETWORK_STATE` | Connectivity detection |
| `ACCESS_WIFI_STATE` | WiFi state for call quality |
| `CHANGE_NETWORK_STATE` | WebRTC network requests |
| `VIBRATE` | Notifications |
| `WAKE_LOCK` | Keep alive for calls/messages |
| `RECEIVE_BOOT_COMPLETED` | Auto-start for notifications |
| `FOREGROUND_SERVICE` | Background operations |
| `FOREGROUND_SERVICE_MEDIA_PROJECTION` | Screen sharing in calls |
| `FOREGROUND_SERVICE_MEDIA_PLAYBACK` | Audio playback |
| `FOREGROUND_SERVICE_PHONE_CALL` | VoIP calls |
| `FOREGROUND_SERVICE_MICROPHONE` | Audio recording in calls |
| `FOREGROUND_SERVICE_CAMERA` | Video calls |
| `FOREGROUND_SERVICE_DATA_SYNC` | Background sync |
| `NFC` | WebApp NFC service |
| `MODIFY_AUDIO_SETTINGS` | Audio routing during calls |
| `USE_BIOMETRIC` / `USE_FINGERPRINT` | App lock |
| `USE_FULL_SCREEN_INTENT` | Incoming call screen |
| `MANAGE_OWN_CALLS` | Telecom integration |
| `SYSTEM_ALERT_WINDOW` | **Declared but NO usage found in code** |
| `POST_NOTIFICATIONS` | Push notifications |
| `REQUEST_IGNORE_BATTERY_OPTIMIZATIONS` | Keep-alive |
| `BLUETOOTH` (maxSdkVersion=30) | Legacy Bluetooth |

### Third-Party/Vendor Permissions
- Badge permissions: Samsung, Sony, HTC, Huawei, Oppo, Majeur launcher
- `com.google.android.gms.permission.AD_ID` — Advertising ID access
- `com.google.android.c2dm.permission.RECEIVE` — FCM push
- `com.google.android.finsky.permission.BIND_GET_INSTALL_REFERRER_SERVICE` — Install attribution

---

## 2. Dangerous Permission Deep Analysis

### ACCESS_BACKGROUND_LOCATION — Conditionally Requested

**Location:** `p000/aed.java` (static initializer, line 79)

```java
if (Build.VERSION.SDK_INT >= 29) {
    objArrCopyOf[2] = "android.permission.ACCESS_BACKGROUND_LOCATION";
}
```

**Usage:** Tied to **live location sharing** feature (`Tasks.LocationRequest` with `liveLocation` field in `iqg.java`). The app requests background location only on Android 10+ to maintain live location updates when the app is backgrounded.

**Assessment:** Partially justified — live location sharing requires background access. However, the permission is bundled into the same request array as foreground location (`f1234l` / `f1235m`), meaning it may be requested even when the user only wants to share a one-time location pin. This is **over-requesting** — background location should only be requested when the user explicitly enables live location sharing.

---

### CAMERA — Usage Beyond Obvious Features

**Obvious uses:**
- Video calls (`ru.p027ok.android.externcalls.sdk.video.*`, WebRTC `Camera2Session`)
- Photo/video capture in chat
- QR code scanner (`one.p011me.qrscanner.QrScannerWidget`)

**Non-obvious uses:**
- Camera permission is grouped with `RECORD_AUDIO` + `READ_PHONE_STATE` in array `f1232j` — all three are requested together for calls, meaning **CAMERA is requested even for audio-only calls**
- CameraX library (`androidx.camera.camera2`) is bundled for QR scanning — this is a heavy dependency for a simple feature
- `f1238p` combines media gallery permissions with CAMERA — requesting camera when user only wants to pick existing photos

**Concern:** Permission grouping causes over-requesting. Audio-only calls should not require CAMERA permission.

---

### RECORD_AUDIO — Usage Analysis

**Used in:**
1. **Voice messages** — `kd0.java` (MediaRecorder for voice recording)
2. **VoIP calls** — WebRTC (`WebRtcAudioRecord.java`, `JavaAudioDeviceModule.java`)
3. **Video calls** — Combined with camera in `f1233k` and `f1240r`

**NOT used in:**
- Analytics/tracking
- Background recording
- Any non-communication feature

**Assessment:** RECORD_AUDIO usage is **justified and properly scoped** to voice messages and calls. No evidence of abuse.

---

### READ_PHONE_STATE — Data Accessed

**Usage locations and data accessed:**

1. **Network type detection** (`oad.java:960`, `r04.java:4078`):
   - `telephonyManager.getNetworkType()` — determines 2G/3G/4G/5G for bitrate adaptation
   - `telephonyManager.getDataNetworkType()` — same purpose, newer API

2. **Network operator info** (`f58.java`, `zu5.java`, `ilb.java`):
   - `telephonyManager.getNetworkOperator()` — MCC+MNC code
   - `telephonyManager.getNetworkOperatorName()` — carrier name

3. **SIM operator for analytics** (`yk2.java:139`):
   - `telephonyManager.getSimOperator()` — sent as `mcc_mnc` to **CctTransportBackend** (Google transport layer)

4. **Call management** (`aed.java`):
   - Grouped with CAMERA + RECORD_AUDIO for call permission requests

**Concern:** SIM operator data (`mcc_mnc`) is collected and sent to analytics backend. While MCC/MNC is not directly PII, it reveals the user's carrier and country. This data collection is not disclosed to the user during permission request.

---

## 3. Over-Privileged Permissions (Requested but Unused/Questionable)

| Permission | Status |
|---|---|
| `SYSTEM_ALERT_WINDOW` | **Declared but NO code references found** — no overlay windows, no `canDrawOverlays` checks. Potentially dead permission. |
| `AD_ID` | Used by `com.p006my.tracker` (MyTracker SDK) and Google GMS to collect advertising identifier. Privacy concern for a messenger. |
| `WRITE_CONTACTS` | Used by contact sync (`ndi.java`) — writes synced contacts back to device. Aggressive for a messenger. |

---

## 4. `call-permissions-interval` PmsKey Analysis

**Definition:** `PmsKey.java:407`
```java
public static final PmsKey f88601callpermissionsinterval = new PmsKey("call-permissions-interval", 127);
```

**Description (from debug UI in `nd1.java:114`):**
```
"0 - Используется старая логика"        (0 = old logic is used)
"> 0 - Время в секундах, через которое будет осуществлена проверка на включенные уведомления"
(> 0 = time in seconds after which a check for enabled notifications will be performed)
```

**Usage:** `qp6.java:645` — initialized as `xo6` (a remote config value holder) with initial value `0`.

**What it controls:** This is a **server-controlled interval** that determines how frequently the app checks whether the user has notifications enabled, specifically in the context of calls. When set to a value > 0, the app will periodically prompt/check notification permissions at the specified interval (in seconds) during or around call flows.

**Security concern:** This is a **server-side controlled permission nag mechanism**. The server can remotely configure how aggressively the app pesters users to enable notifications. Setting this to a low value (e.g., 5 seconds) would create an aggressive permission request loop during calls, potentially pressuring users into granting permissions.

---

## 5. Summary of Findings

### High Concern
1. **SYSTEM_ALERT_WINDOW declared but unused** — over-privileged manifest, or reserved for future undisclosed use
2. **Server-controlled permission request interval** (`call-permissions-interval`) — enables remote manipulation of permission request frequency
3. **CAMERA requested for audio-only calls** — permission grouping causes unnecessary access

### Medium Concern
4. **BACKGROUND_LOCATION bundled with foreground location** — over-requested for one-time location shares
5. **SIM operator data sent to analytics** without explicit user consent
6. **AD_ID permission** in a privacy-focused messenger — advertising identifier collected by MyTracker SDK
7. **WRITE_CONTACTS** — aggressive contact write-back

### Low Concern
8. **RECORD_AUDIO** — properly scoped to calls and voice messages
9. **NFC** — used by WebApp NFC service (justified)
10. **READ_PHONE_STATE** — primarily for network quality adaptation (justified)

---

## 6. Recommendations

1. Remove `SYSTEM_ALERT_WINDOW` if unused
2. Separate CAMERA from audio-only call permission requests
3. Request `ACCESS_BACKGROUND_LOCATION` only when user explicitly enables live location
4. Audit `call-permissions-interval` for abuse potential — add client-side minimum interval cap
5. Disclose SIM operator data collection in privacy policy
6. Consider removing AD_ID for a privacy-focused messenger
