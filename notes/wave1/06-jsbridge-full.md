# JS-Bridge Full Enumeration — MAX 26.15.3

## Architecture

The JS-bridge between WebView (mini-apps) and native code uses three `@JavascriptInterface` objects:

| JS Object | Java Class | Purpose |
|-----------|-----------|---------|
| `WebViewHandler` | `upk.java` | Public bridge — `postEvent(name, data)` + `resolveShare(requestId, bytes, mimeType, fileName)` |
| `PrivateWebViewHandler` | `i6e.java` | Private bridge — `postEvent(name, data)` with `isPrivate=true` |
| `AndroidPerf` | `hgk.java` | Performance tracking — `trackFcp(long)` |

Events flow: JS → `postEvent(name, json)` → `ijk.A(name, data, isPrivate)` → `gz8` dispatcher → individual `lz8` (JsDelegate) implementations.

Native → JS: `WebApp.sendEvent(name, data)` or `PrivateWebApp.sendEvent(name, data)` via `evaluateJavascript()`.

## Private Bridge Access Control

- `b1` field in `ijk.java` determines if private bridge is enabled for a given bot
- Controlled by server-side whitelist: `((ynb) qp6Var.H1.x(qp6Var, qp6.S2[119])).d(botId)` — PmsKey #119
- **Only whitelisted bots get `PrivateWebViewHandler` injected** (ahk.java:101)
- The `e()` method on `jek` interface marks events as private (only `WebAppVerifyMobileId` returns true)

## Bot Whitelist for Specific Methods

The `a()` method on `jek` returns an Integer — the PmsKey index that controls which bots can use that method. `null` = any bot.

## Complete Handler Table

### Interface: `jek` (event type enums)

| # | WebApp Name | snake_case Event | PmsKey Whitelist | Private | Class | Risk |
|---|-------------|-----------------|-----------------|---------|-------|------|
| 1 | `WebAppVerifyMobileId` | `verify_mobile_id` | null (any) | **YES** | `zij.java` | 🔴 CRITICAL — returns MSISDN via cleartext HTTP to operator |
| 2 | `WebAppDownloadFile` | `download_file` | PmsKey[12] | no | `dbk.java` | 🟡 Medium — arbitrary file download |
| 3 | `WebAppNfcGetInfo` | `nfc_get_info` | PmsKey[40] | no | `gfk.java` | 🟡 Medium — NFC state enumeration |
| 4 | `WebAppNfcEmulateNfcTag` | `nfc_emulate_nfc_tag` | PmsKey[43] | no | `gfk.java` | 🔴 HIGH — JS controls NFC HCE card emulation |
| 5 | `WebAppNfcOpenSystemSettings` | `nfc_open_system_settings` | PmsKey[42] | no | `gfk.java` | 🟢 Low |
| 6 | `WebAppOpenCodeReader` | `open_code_reader` | PmsKey[30] | no | `oak.java` | 🟡 Medium — opens camera for QR/barcode scan |
| 7 | `WebAppBiometryGetInfo` | `biometry_get_info` | PmsKey[18] | no | `t8k.java` | 🟡 Medium — fingerprint/face info |
| 8 | `WebAppBiometryRequestAccess` | `biometry_request_access` | PmsKey[9] | no | `t8k.java` | 🟡 Medium |
| 9 | `WebAppBiometryUpdateToken` | `biometry_update_token` | PmsKey[10] | no | `t8k.java` | 🟡 Medium — stores token in biometry-protected storage |
| 10 | `WebAppBiometryRequestAuth` | `biometry_request_auth` | PmsKey[11] | no | `t8k.java` | 🔴 HIGH — triggers biometric auth, returns stored token |
| 11 | `WebAppBiometryOpenSettings` | `biometry_open_settings` | PmsKey[13] | no | `t8k.java` | 🟢 Low |
| 12 | `WebAppShare` | `web_app_share` | PmsKey[14] | no | `alk.java` | 🟡 Medium — share content to other apps |
| 13 | `WebAppMaxShare` | `web_app_max_share` | PmsKey[15] | no | `alk.java` | 🟡 Medium — share within MAX ecosystem |
| 14 | `WebAppGetViewportSize` | `get_viewport_size` | null (any) | no | `aok.java` | 🟢 Low |
| 15 | `WebAppHapticFeedbackImpact` | `haptic_feedback_impact` | PmsKey[8] | no | `edk.java` | 🟢 Low |
| 16 | `WebAppHapticFeedbackNotification` | `haptic_feedback_notification` | PmsKey[20] | no | `edk.java` | 🟢 Low |
| 17 | `WebAppHapticFeedbackSelectionChange` | `haptic_feedback_selection_change` | PmsKey[16] | no | `edk.java` | 🟢 Low |
| 18 | `WebAppReady` | `ready` | null (any) | no | `enk.java` | 🟢 Low |
| 19 | `WebAppClose` | `close` | null (any) | no | `enk.java` | 🟢 Low |
| 20 | `WebAppSetupBackButton` | `setup_back_button` | null (any) | no | `enk.java` | 🟢 Low |
| 21 | `WebAppSetupClosingBehavior` | `setup_closing_behaviour` | null (any) | no | `enk.java` | 🟢 Low |
| 22 | `WebAppBackButtonPressed` | `back_button_pressed` | null (any) | no | `enk.java` | 🟢 Low |
| 23 | `WebAppSetupScreenCaptureBehavior` | `setup_screen_capture_behavior` | null (any) | no | `enk.java` | 🔴 HIGH — enables/disables FLAG_SECURE bypass |
| 24 | `WebAppOpenLink` | `open_link` | null (any) | no | `xdk.java` | 🟡 Medium — opens arbitrary URL |
| 25 | `WebAppOpenMaxLink` | `open_max_link` | null (any) | no | `xdk.java` | 🟡 Medium — opens MAX deeplink |
| 26 | `WebAppRequestPhone` | `request_phone` | PmsKey[55] | no | `ogk.java` | 🔴 HIGH — requests user's phone number |
| 27 | `WebAppSecureStorageSaveKey` | `secure_storage_save_key` | PmsKey[1] | no | `lmk.java` | 🟡 Medium |
| 28 | `WebAppSecureStorageGetKey` | `secure_storage_get_key` | PmsKey[2] | no | `lmk.java` | 🟡 Medium |
| 29 | `WebAppSecureStorageClear` | `secure_storage_clear` | PmsKey[4] | no | `lmk.java` | 🟡 Medium |
| 30 | `WebAppDeviceStorageSaveKey` | `device_storage_save_key` | PmsKey[5] | no | `lmk.java` | 🟡 Medium — persistent local storage |
| 31 | `WebAppDeviceStorageGetKey` | `device_storage_get_key` | PmsKey[6] | no | `lmk.java` | 🟡 Medium |
| 32 | `WebAppDeviceStorageClear` | `device_storage_clear` | PmsKey[7] | no | `lmk.java` | 🟡 Medium |
| 33 | `WebAppChangeScreenBrightness` | `change_screen_brightness` | null (any) | no | `aak.java` | 🟡 Medium — sets max brightness |

### Additional: `resolveShare` (direct @JavascriptInterface, not via postEvent)

| # | Method | Class | Risk |
|---|--------|-------|------|
| 34 | `resolveShare(requestId, byte[], mimeType, fileName)` | `upk.java` | 🔴 HIGH — JS passes raw binary data + arbitrary filename to native file system |

### Additional: `unsupported_method_handler` (catch-all)

| # | Handler | Class | Risk |
|---|---------|-------|------|
| 35 | `unsupported_method_handler` | `mnk.java` | 🟡 Medium — logs all unknown method calls with requestId, reports to server |

## JsDelegate Implementations (lz8 interface)

| Class | Handles | Notes |
|-------|---------|-------|
| `rnk.java` | WebAppVerifyMobileId | Private bridge only; makes cleartext HTTP to operator HE endpoints |
| `bek.java` | WebAppOpenLink, WebAppOpenMaxLink | Opens URLs; OpenMaxLink navigates within MAX |
| `jlk.java` | WebAppShare, WebAppMaxShare | Share content; MaxShare sends within MAX chats |
| `nbk.java` | WebAppDownloadFile | Downloads arbitrary URL to device storage |
| `sgk.java` | WebAppRequestPhone | Returns user's phone number to mini-app |
| `rfk.java` | WebAppNfcGetInfo, WebAppNfcEmulateNfcTag, WebAppNfcOpenSystemSettings | Full NFC HCE control |
| `tak.java` | WebAppOpenCodeReader | Camera-based QR/barcode scanner |
| `eok.java` | WebAppGetViewportSize | Viewport dimensions |
| `eak.java` | WebAppChangeScreenBrightness | Screen brightness control |
| `odk.java` | HapticFeedback (3 types) | Vibration motor |
| `vmk.java` | SecureStorage (3) + DeviceStorage (3) | Persistent key-value storage |
| `k9k.java` | Biometry (5 methods) | Fingerprint/face auth + token storage |
| `knk.java` | WebAppBackButtonPressed | Back button event |
| `mnk.java` | unsupported_method_handler | Catch-all for unknown methods |

## Critical Security Findings

### 1. 🔴 `resolveShare` — Raw Binary Injection from JS (NEW)

`upk.java` exposes `resolveShare(String requestId, byte[] data, String mimeType, String fileName)` as a `@JavascriptInterface`. This allows any mini-app JS code to:
- Pass **arbitrary binary data** directly to native code
- Specify **arbitrary filename** and MIME type
- The native handler (`vik.java`) processes this without visible sanitization

This is a **file write primitive** from JavaScript — a mini-app can potentially write arbitrary files to the app's storage.

### 2. 🔴 `WebAppVerifyMobileId` — MSISDN Exfiltration via Private Bridge

Only handler marked `e() = true` (private). Makes cleartext HTTP request to operator Header Enrichment endpoints. The response (containing MSISDN) is returned to the calling mini-app's JavaScript. Server controls which bots get private bridge access via PmsKey[119].

### 3. 🔴 `WebAppNfcEmulateNfcTag` — JS Controls NFC Card Emulation

Via `WebAppNfcService` (extends `HostApduService`), a mini-app can:
- Set arbitrary NDEF data bytes for NFC tag emulation
- The service responds to APDU commands from NFC readers
- Effectively turns the phone into a programmable NFC card controlled by remote JS

### 4. 🔴 `WebAppSetupScreenCaptureBehavior` — FLAG_SECURE Bypass

Any bot (no PmsKey restriction, `a()` returns null) can call this to enable screen capture of the WebView, bypassing Android's FLAG_SECURE protection. This means a mini-app can programmatically allow screenshots/recording of content that should be protected.

### 5. 🔴 SSL Certificate Validation Bypass (`isDisableWebAppSsl`)

In `xyc.java:onReceivedSslError()`:
```java
if (!((opk) this.b).i()) {
    sslErrorHandler.cancel();  // normal: reject bad cert
} else {
    sslErrorHandler.proceed();  // BYPASS: accept any cert!
}
```

The `isDisableWebAppSsl` preference (stored in SharedPreferences, togglable via DevMenu which is present in release builds) causes ALL SSL errors to be silently accepted for WebApp WebViews. Combined with the DevMenu being accessible in production, this creates a trivial MITM vector.

### 6. 🔴 `WebAppRequestPhone` — Phone Number Disclosure

Returns the user's phone number to the requesting mini-app. Gated by PmsKey[55] but the server controls which bots are whitelisted.

### 7. 🟡 User Interaction Bypass for Whitelisted Bots

In `ijk.A()` (line 295):
```java
if (!lw.s0(this.b, (long[]) qp6Var.L0.x(qp6Var, qp6.S2[76])) 
    && L1.contains(str) 
    && System.currentTimeMillis() - this.I1 >= 3000) {
    // BLOCKED: no user click in last 3000ms
}
```

Bots in PmsKey[76] whitelist **bypass the 3-second user interaction requirement** for sensitive operations (Share, MaxShare, DownloadFile, OpenLink, OpenMaxLink). This means whitelisted bots can trigger file downloads and link opens without any user gesture.

## Additional Context

### WebAppContactData (yr7.java)
The `GetWebAppContactDataUseCase` provides mini-apps with contact display names and avatar URLs. Registered in DI container (mel.java:704).

### VPN Detection
`WebAppHttpClient` (sdk.java) forces requests over cellular network. If VPN is detected, throws `WebAppHasVpnException` which is reported back to JS as error code `has_vpn` (qnk.java:40). This ensures operator Header Enrichment headers are injected.

### Biometry Token Storage
Biometric tokens are stored in SQLite table `webapp_biometry` with columns: `user_id`, `bot_id`, `token`, `access_requested`, `access_granted`. A compromised bot could store/retrieve arbitrary tokens protected by biometric auth.

### WebTransport Socket (xjd.java case 17)
The bridge includes a WebTransport implementation (`lpk.java`) that compresses and streams data over a bidirectional socket. This provides a raw data channel between JS and native beyond the standard postEvent mechanism.

## Summary Statistics

- **Total unique WebApp events**: 33 (via jek enum) + 1 direct method (resolveShare) + 1 catch-all = **35 handlers**
- **Private-only events**: 1 (WebAppVerifyMobileId)
- **PmsKey-gated events**: 20 (require server-side bot whitelist)
- **Unrestricted events (any bot)**: 12 (Ready, Close, BackButton, ClosingBehavior, BackButtonPressed, ScreenCaptureBehavior, OpenLink, OpenMaxLink, GetViewportSize, ChangeScreenBrightness, resolveShare, unsupported_method)
- **Camera access**: 1 (OpenCodeReader)
- **NFC control**: 3 (GetInfo, EmulateNfcTag, OpenSystemSettings)
- **Phone number access**: 2 (RequestPhone, VerifyMobileId)
- **Biometric auth**: 5 methods
- **Storage access**: 6 methods (3 secure + 3 device)

## Source Files

- Event type enums: `zij.java`, `dbk.java`, `gfk.java`, `oak.java`, `t8k.java`, `alk.java`, `aok.java`, `edk.java`, `enk.java`, `lmk.java`, `ogk.java`, `xdk.java`, `aak.java`
- JsDelegate implementations: `rnk.java`, `bek.java`, `jlk.java`, `nbk.java`, `sgk.java`, `rfk.java`, `tak.java`, `eok.java`, `eak.java`, `odk.java`, `vmk.java`, `k9k.java`, `knk.java`, `mnk.java`
- JS interfaces: `upk.java` (WebViewHandler), `i6e.java` (PrivateWebViewHandler), `hgk.java` (AndroidPerf)
- Main dispatcher: `ijk.java` (610 lines), `gz8.java` (JsBridge container)
- WebView setup: `ahk.java` (injects JS interfaces)
- Event delivery to WebView: `ghk.java` (evaluateJavascript)
- SSL bypass: `xyc.java:97` + `ppk.java:258` + `ri9.java:162`
- VPN detection: `sdk.java`, `qdk.java`
- NFC service: `one/me/webapp/util/WebAppNfcService.java`
