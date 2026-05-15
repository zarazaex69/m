# VULN-81: Max Messenger Login/Registration Flow Deep Reconstruction

## 1. Key Screens Found

- **InputPhoneScreen**: `one.p011me.login.inputphone.InputPhoneScreen` — phone number entry
- **ConfirmPhoneScreen**: `one.p011me.login.confirm.ConfirmPhoneScreen` — SMS code verification
- **ViewModels**: `do8` (InputPhoneViewModel), `mc4` (ConfirmPhoneViewModel)

## 2. Complete Auth Flow

### Step 1: Phone Input → SMS Request
```
InputPhoneScreen → do8 (ViewModel) → vn8 (coroutine handler)
```

**Command sent**: `AUTH_REQUEST` (u0d.f69056C0, command ID 17)
```java
// vn8.java - sends phone to server
dbc dbcVar = new dbc(u0d.f69056C0, 17);
dbcVar.m18553j("phone", formattedPhone);
dbcVar.m18553j("type", "START_AUTH");  // AbstractC0627n.m14552a(1)
```

**Server response** (`ig0` class):
- `f27275c` → verifyToken (String)
- `f27276d` → codeLength (int)
- `f27277o` → codeResendMillis (long) — cooldown timer
- `f27273X` → unknown long
- `f27274Y` → unknown int

### Step 2: Navigate to ConfirmPhoneScreen
After AUTH_REQUEST succeeds, emits `fn8` event (PhoneConfirmScreen data):
```java
new fn8(verifyToken, phone, codeLength, codeResendMillis, countryNameCode)
```
ConfirmPhoneScreen constructor params: `(verifyToken, phone, codeLength, codeResendMillis, countryNameCode, scopeId)`

### Step 3: Code Verification
```
ConfirmPhoneScreen → mc4 (ViewModel) → hc4 (coroutine handler)
```

**Command sent**: `AUTH` (u0d.f69060D0, command ID 18)
```java
// hc4.java - verifies SMS code
dbc dbcVar = new dbc(u0d.f69060D0, 10);
dbcVar.m18553j("token", verifyToken);       // from AUTH_REQUEST response
dbcVar.m18553j("verifyCode", smsCode);      // user-entered code
dbcVar.m18553j("authTokenType", "CHECK_CODE");
```

### Step 4: Server Response Branching (`kf0` class)
Response contains `LinkedHashMap` with token types and optional `bj6` (2FA data):

- **If `bj6 != null`**: 2FA required → navigate to password check screen
- **If map contains "LOGIN"**: Existing user → extract login token → session creation
- **If map contains "REGISTER"**: New user → navigate to registration flow

### Step 5: Session Creation
**LOGIN command** (`tl9` class, u0d.f69063E0, command ID 19):
```java
// tl9.java
m18553j("token", loginToken);
m18546c("interactive", true);
// + sync timestamps: chatsSync, contactsSync, presenceSync, callsSync, draftsSync, bannersSync
// + configHash, chatCacheFingerprint, exp data
```

## 3. Data Sent During Registration/Auth

### Anonymous Login (`lg0` class - `auth.anonymLogin`):
```json
{
  "referrer": null,
  "device_id": "<device_id>",
  "verification_supported": true,
  "verification_token": null,
  "verification_supported_v": "1",
  "client": "test",
  "gen_token": true,
  "session_data": "{\"auth_token\": \"<token>\", \"version\": 3, \"device_id\": \"<id>\", \"client_version\": 1}"
}
```

### Account preferences stored (`yag` class):
- `deviceId`, `pushToken`, `pushDeviceType`, `installationMarket`, `installationInfoVersion`
- `systemLang`, `lang`, `firstLoginTime`, `lastLoginTime`

## 4. Client-Side Rate Limiting on SMS Requests

**YES** — implemented via countdown timer in `lc4` class:

```java
// lc4.java - countdown timer for resend
// Decrements from codeResendMillis (in seconds) every 1000ms
// Resend button only enabled when timer reaches 0
```

- Timer value comes from server response (`ig0.f27277o` = codeResendMillis)
- Converted to seconds: `wy5.m24813s(codeResendMillis, cz5.SECONDS)`
- **Resend uses type "RESEND"**: `AbstractC0627n.m14552a(2)` in `ic4.java`
- **Client-side only** — no evidence of server-enforced lockout beyond the timer

### Resend Flow (`ic4.java`):
```java
dbc dbcVar = new dbc(u0d.f69056C0, 17);  // Same AUTH_REQUEST command
dbcVar.m18553j("phone", phone);
dbcVar.m18553j("type", "RESEND");  // AbstractC0627n.m14552a(2)
```

## 5. CHECK_CODE Auth Token Type

In `hc4.java`, `authTokenType = "CHECK_CODE"` is sent with the AUTH command when verifying an SMS code. It signals the server that this is a code verification request (as opposed to PHONE or PHONE_CONFIRM types used for binding/rebinding).

**Validation**: Code throws `IllegalArgumentException` if `verifyCode` is null when `authTokenType` is `PHONE` or `PHONE_CONFIRM`, but CHECK_CODE also requires it.

**Other authTokenType values** (from `yxa.java`):
- `PHONE_CONFIRM` (4) — phone confirmation
- `PHONE_REBINDING` (3) — rebinding phone
- `PHONE_BINDING` (2) — binding phone
- `UNKNOWN` (1)

## 6. SMS Verification Bypass Analysis

**No client-side bypass found.** However:

- Rate limiting is **server-provided but client-enforced** — the timer value comes from server but UI enforcement is client-side only
- The `verifyToken` is the only server-side binding between phone and session
- No evidence of CAPTCHA or device attestation before SMS send
- No evidence of attempt counting on client side (only timer-based cooldown)
- The `type` field distinguishes "START_AUTH" vs "RESEND" — server may have different limits

**Potential weaknesses**:
- Client-side timer bypass: patching the app to ignore `codeResendMillis` allows unlimited resend requests
- No visible client-side brute-force protection on code entry (server-side unknown)
- `processingCode` check (`mc4.f40042J0`) only prevents re-submitting the same code

## 7. QR Auth Alternative Flow

### Components:
- `QrAuthHintBottomSheet` — hint dialog for QR login
- `AUTH_QR_APPROVE` command (u0d.f69179o3, command ID 290)
- `zf0` — QR auth statistics/analytics
- `hg0` — QR auth approve handler

### QR Auth Flow:
```java
// hg0.java - QR approve
dbc dbcVar = new dbc(u0d.f69179o3, 16);
dbcVar.m18553j("qrLink", scannedQrContent);
```

### QR Error States:
- `qr_link.invalid` → invalid QR code
- `track.not.found` → expired QR session

### QR Analytics Events (`zf0.java`):
1. `qr_login_button_click`
2. `permission_prompt_shown` (camera)
3. `permission_decision`
4. `qr_scan_failed`
5. `qr_scan_succeeded`
6. `qr_not_auth_ui_shown`

## 8. Complete Auth State Machine

```
┌─────────────────────────────────────────────────────────────┐
│                    INITIAL STATE                              │
│                   (LoginScreen)                               │
└──────────────────────┬──────────────────────────────────────┘
                       │
                       ▼
┌─────────────────────────────────────────────────────────────┐
│              INPUT_PHONE STATE                                │
│         (InputPhoneScreen / do8 ViewModel)                   │
│  - User enters phone number                                  │
│  - "Continue" sends AUTH_REQUEST(phone, type="START_AUTH")    │
└──────────────────────┬──────────────────────────────────────┘
                       │ Server returns: verifyToken, codeLength,
                       │                 codeResendMillis
                       ▼
┌─────────────────────────────────────────────────────────────┐
│            CONFIRM_PHONE STATE                               │
│       (ConfirmPhoneScreen / mc4 ViewModel)                   │
│  - User enters SMS code                                      │
│  - Countdown timer for resend (lc4)                          │
│  - Resend: AUTH_REQUEST(phone, type="RESEND")                │
│  - Submit: AUTH(token, verifyCode, authTokenType="CHECK_CODE")│
└───────┬──────────────────┬──────────────────┬───────────────┘
        │                  │                  │
        ▼                  ▼                  ▼
┌──────────────┐  ┌──────────────┐  ┌──────────────────────┐
│ LOGIN STATE  │  │REGISTER STATE│  │   2FA STATE          │
│(existing user)│  │ (new user)   │  │(password required)   │
│              │  │              │  │                      │
│ Token: LOGIN │  │Token:REGISTER│  │ bj6: track_id,      │
│              │  │              │  │ hint, email,         │
│ → tl9 LOGIN │  │ → Registration│  │ password constraints │
│   command    │  │   flow       │  │                      │
└──────┬───────┘  └──────┬───────┘  └──────────┬───────────┘
       │                 │                      │
       ▼                 ▼                      ▼
┌─────────────────────────────────────────────────────────────┐
│                   SESSION ACTIVE                              │
│  - LOGIN cmd with token, sync timestamps                     │
│  - session_data: {auth_token, version:3, device_id,          │
│                   client_version:1}                           │
└─────────────────────────────────────────────────────────────┘

         ┌────────────────────────────────────┐
         │     QR AUTH (alternative flow)      │
         │  - Scan QR from another device      │
         │  - AUTH_QR_APPROVE(qrLink)          │
         │  - Errors: qr_link.invalid,         │
         │            track.not.found           │
         └────────────────────────────────────┘
```

## Key Security Observations

1. **Timer bypass**: `codeResendMillis` is enforced client-side only — trivial to bypass
2. **No CAPTCHA**: No evidence of CAPTCHA before SMS send
3. **Token-based flow**: `verifyToken` binds phone to session; if predictable, could be exploited
4. **CHECK_CODE has no visible retry limit on client**: Server may enforce, but client doesn't
5. **QR auth**: Single `qrLink` parameter — if QR content is interceptable, session hijack possible
6. **Anonymous login exists**: `auth.anonymLogin` endpoint with minimal device binding
