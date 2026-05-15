# VULN-29: QR Code Login Session Hijacking Analysis

## Summary

Max messenger implements QR code-based login for linking additional devices (desktop/web). The mobile app scans a QR code displayed on the web client and approves the session via the `AUTH_QR_APPROVE` API endpoint. Analysis reveals several security concerns in the implementation.

## QR Login Flow Architecture

### Flow Overview
1. Web/desktop client generates a QR code containing a link (the "qrLink")
2. Mobile app opens QR scanner in `LOGIN` mode (`zqe.f87835c`, mode=2)
3. User scans the QR code → text extracted as `h4g.f23388a` (Success text)
4. `SettingsDevicesScreen.mo4578B()` receives the scanned text
5. `eyg.f17201A0` dispatches the auth job via `cyg` coroutine
6. `byg` coroutine calls `hg0.m9501a(qrLink)` — the actual auth API
7. `hg0` sends `AUTH_QR_APPROVE` (API command 290) with parameter `qrLink=<scanned_text>`
8. Server responds with success (`eg0`) or error (`dg0`: InvalidQr/TrackExpired/Unknown)

### Key Classes
| Class | Role |
|-------|------|
| `QrScannerWidget` | Camera-based QR scanner UI |
| `SettingsDevicesScreen` | Device management, initiates QR scan |
| `eyg` (ViewModel) | Orchestrates QR auth flow, has `authQrJob` |
| `cyg` | Coroutine handling auth result routing |
| `byg` | Coroutine calling the auth API |
| `hg0` | Repository: calls `AUTH_QR_APPROVE` API |
| `zf0` | Analytics/stats for QR login events |
| `QrAuthHintBottomSheet` | Confirmation hint dialog |

## Findings

### 1. QR Code Content — What Data Is Encoded

The QR code contains a URL/link string (`qrLink`). This is passed directly to the server via:
```java
// hg0.m9501a()
dbc dbcVar = new dbc(u0d.f69179o3, 16);  // AUTH_QR_APPROVE
dbcVar.m18553j("qrLink", str);  // The scanned QR content sent as-is
```

The QR content is a link (likely `https://max.me/...` or similar domain) containing a session track identifier. The server validates this link and either approves the session or returns errors like `qr_link.invalid` or `track.not.found`.

### 2. Token Expiry — TrackExpired Error

**Server-side expiry exists.** The error `bg0` ("TrackExpired") indicates the server enforces a TTL on QR tokens:
```java
// Error handling in hg0.m9501a()
if (zm0.m27177c(str3, "track.not.found")) {
    obj = bg0.f5490a;  // TrackExpired
}
```

The string resource `settings_devices_expired_login_qr_error` (btc.f6645n) is shown to the user. However, **no client-side expiry enforcement** was found — the token lifetime is entirely server-controlled. The actual TTL duration is not visible in the client code.

### 3. Can an Attacker Generate a QR Code That Steals Sessions?

**Partially — via social engineering.** The attack vector:

1. Attacker opens Max web client → gets a QR code with their session track
2. Attacker screenshots/forwards this QR code to victim
3. If victim scans it with Max's QR scanner (in LOGIN mode), the attacker's web session gets authenticated with the victim's account

**Critical finding:** The `AUTH_QR_APPROVE` call in `hg0.m9501a()` sends the scanned link directly to the server. There is **no validation on the client side** that the QR code originated from a legitimate Max web login page. Any string matching the expected URL format will be submitted.

### 4. Confirmation Required After Scanning

**YES — A confirmation dialog exists but with weaknesses:**

The `QrAuthHintBottomSheet` shows a confirmation dialog with:
- Accept button (`settings_devices_auth_hint_accept_button_title`)
- Deny button (`settings_devices_auth_hint_deny_button_title`)
- Description text (`settings_devices_auth_hint_description_full`)

However, examining the flow in `SettingsDevicesScreen.mo4578B()`:
```java
if (j4gVar instanceof h4g) {
    zf0VarM6877v.m26943a(zf0VarM6877v, 5, 0, null, 6);  // qr_scan_succeeded analytics
    eygVarM16976c1.f17201A0.mo2430y(..., new cyg(eygVarM16976c1, ((h4g) j4gVar).f23388a, null));
}
```

The auth API call (`cyg`/`byg`) is dispatched **immediately upon successful scan** from the `SettingsDevicesScreen`. The `QrAuthHintBottomSheet` appears to be shown as a **post-scan informational hint**, not as a pre-authorization gate. The actual `AUTH_QR_APPROVE` API call happens without waiting for explicit user confirmation in the dialog.

**Vulnerability:** The scan-to-approve flow appears to be automatic — scanning equals approving.

### 5. QR Code Phishing Vectors

**Multiple vectors identified:**

1. **Fake QR overlay attack:** Attacker places a malicious QR code over a legitimate one (e.g., in a shared workspace)
2. **Social engineering via image:** QR code sent as image in chat — if user opens QR scanner from gallery (`isPickFromGalleryEnabled`), they could scan a malicious QR from a received image
3. **Deep link exploitation:** The QR scanner can be opened via deeplink (`:qr-scanner?mode=2`), potentially triggered by malicious links
4. **No domain validation visible:** The client sends whatever URL is scanned to `AUTH_QR_APPROVE` — no visible allowlist of valid QR code domains/formats on the client side
5. **Gallery scanning:** `QrScannerWidget` supports picking images from gallery (mode 0 with `can_select_file=true`), enabling scanning of QR codes received via messages

### 6. New Session vs Shared Session

**Creates a NEW session.** Evidence:

- The feature lives in `settings/devices/` package — device management
- `SettingsDevicesScreen` shows a list of active sessions with ability to terminate them
- The `prg` objects in `eyg.m6880y()` represent individual device sessions with properties: ID (`f54242a`), name (`f54243b`), date (`f54244c`), description (`f54245d`)
- String resources: `settings_devices_current_sessions`, `settings_devices_finished_all` (terminate all), `settings_devices_dialog_finished_session_finish_btn`
- The success event (`sx3`) triggers a "close" navigation, returning to the device list

The QR login creates an **independent session** for the web/desktop client. The original mobile session remains unaffected. Each session can be individually terminated from the device management screen.

## Risk Assessment

| Risk | Severity | Description |
|------|----------|-------------|
| Auto-approve on scan | **HIGH** | No explicit confirmation gate before API call |
| QR phishing via gallery | **MEDIUM** | Users can scan QR images from chat messages |
| No client-side URL validation | **MEDIUM** | Any URL format accepted and sent to server |
| Deeplink-triggered scanner | **LOW** | Scanner can be opened via deeplink in LOGIN mode |
| Server-side expiry only | **LOW** | No client-side freshness check; relies on server TTL |

## Recommendations

1. **Add explicit confirmation dialog BEFORE calling AUTH_QR_APPROVE** — show device info, location, and require tap to approve
2. **Validate QR code URL format client-side** — allowlist expected domains before sending to API
3. **Show session details in confirmation** — display requesting device's IP/location/browser
4. **Add rate limiting** — prevent rapid scanning of multiple QR codes
5. **Warn on gallery-sourced QR codes** — extra warning when QR is scanned from an image rather than live camera
6. **Push notification on new session** — alert user when a new device session is created via QR

## Files Analyzed

- `one/p011me/settings/devices/SettingsDevicesScreen.java`
- `one/p011me/settings/devices/hintdialog/QrAuthHintBottomSheet.java`
- `one/p011me/qrscanner/QrScannerWidget.java`
- `p000/eyg.java` (SettingsDevices ViewModel)
- `p000/cyg.java` (Auth result handler coroutine)
- `p000/byg.java` (Auth API call coroutine)
- `p000/hg0.java` (QR auth repository — AUTH_QR_APPROVE)
- `p000/zf0.java` (Auth QR analytics/stats)
- `p000/zqe.java` (QrScannerMode enum: WEBAPP/LOGIN)
- `p000/ag0.java` (InvalidQr error)
- `p000/bg0.java` (TrackExpired error)
- `p000/eg0.java` (Success result)
- `p000/fre.java` (QR scanner ViewModel)
- `p000/ghj.java` (QR code URL builder with utm_source)
