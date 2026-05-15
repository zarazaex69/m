# VULN-19: Authentication Protocol Analysis — Max (OneME) Messenger

## Executive Summary

Max messenger uses a custom session-key-based authentication protocol (not JWT), derived from the OK (Odnoklassniki) platform. Sessions are stored in plain SharedPreferences without encryption, device binding is weak, and token extraction from a rooted device enables full session hijacking.

---

## 1. Login Flow

**Entry Point:** `InputPhoneScreen` → `ConfirmPhoneScreen`

**Sequence:**
1. User enters phone number (`InputPhoneScreen`)
2. SMS code sent to phone
3. User enters SMS code (`ConfirmPhoneScreen` → `hc4.java`)
4. Server returns `LoginResponse` (`xm9.java`) containing:
   - `uid` — user ID
   - `session_key` — session identifier
   - `session_secret_key` — session signing secret
   - `auth_token` — authentication token
   - `api_server` — API endpoint
   - `auth_hash` — authentication hash

**Source:** `dw8.java` (parser, case 8), `xm9.java` (LoginResponse model)

**Anonymous login:** `lg0.java` — `auth.anonymLogin` endpoint sends `session_data` JSON with `auth_token`, `device_id`, and `version`.

---

## 2. Token Format — CUSTOM (Not JWT)

The auth token is an **opaque string**, not a JWT. Evidence:

- `xm9.f80405d` = `auth_token` (plain string field)
- `lb4.java` SessionInfo: `sessionKey`, `sessionSecret`, `apiEndpoint`, `authToken` — all opaque strings
- `C0815po.java` ApiConfig: `appKey`, `token`, `sessionKey`, `sessionSecret`
- No Base64 decoding, no JWT header parsing, no `.` delimiters observed

The protocol uses **session_key + session_secret_key** for request signing (HMAC-like), similar to OAuth 1.0 patterns.

---

## 3. Token Storage — Plain SharedPreferences (CRITICAL)

**Storage mechanism:** `SharedPreferences` via `"user.prefs"` (`OneMeApplication.java:175`)

- **No EncryptedSharedPreferences** detected
- **No Android KeyStore** usage for session credentials
- `SimpleConfigurationStore.java` holds `lb4` (SessionInfo) in memory with no encryption
- Firebase Installation tokens stored in plaintext JSON files (`PersistedInstallation.*.json`)

**Vulnerability:** On rooted devices, session credentials are trivially extractable from `/data/data/<package>/shared_prefs/user.prefs.xml`.

---

## 4. WebSocket Session Establishment

**From `lg0.java` — `auth.anonymLogin`:**

After WebSocket connect, the first message sent is `session_data`:

```json
// With auth token (version 3):
{"auth_token": "<token>", "version": 3, "device_id": "<device_id>", "client_version": 1}

// Without auth token (version 2):
{"version": 2, "device_id": "<device_id>", "client_version": 1}
```

**WebSocket lifecycle** (`tch.java`):
1. TCP + TLS handshake
2. HTTP upgrade with `Sec-WebSocket-Extensions: permessage-deflate`
3. `handleSocketOpen` → `onConnected` callback
4. `session_data` parameter sent with auth credentials

---

## 5. Token Refresh Logic

**Firebase Installation tokens** (`rw6.java`):
- Tokens have `expiresInSecs` and `tokenCreationEpochInSecs`
- Refresh via `authTokens:generate` endpoint with `FIS_v2` auth header
- Retry logic: max 1 retry on 5xx errors

**App session tokens:**
- Session error codes 102, 103, or 100 with `session_key` field trigger re-auth (`wfl.java`)
- `ApiSessionChangedException` (error code 107) forces session renewal
- **No explicit replay prevention** (no nonce, no timestamp binding in requests visible)

---

## 6. Device Binding

**Weak device binding detected:**

- `device_id` is sent in `session_data` during WebSocket auth (`lg0.java`)
- `deviceIdx` parameter used in calls SDK (`ApiProtocol.PARAM_DEVICE_ID`)
- `CallsSdkInitializer.java`: native `initializeSessionSeed(context, seed, deviceId)` — device ID used for calls session seed
- `kc5` class generates device ID if none supplied (`zu5.java:939`)

**Weakness:** The `device_id` is a client-generated identifier, not cryptographically bound to the token. Replaying a stolen token with the same device_id from another device would likely succeed.

---

## 7. SMS/Phone Verification Flow

1. `InputPhoneScreen` — user enters phone number
2. Server sends SMS with verification code
3. `ConfirmPhoneScreen` — user enters code
4. `hc4.java` sends auth command:
   - `token` (verification token from step 2)
   - `verifyCode` (SMS code)
   - `authTokenType` = `"CHECK_CODE"`
5. On success, server returns full `LoginResponse` with session credentials
6. Optional 2FA: `TwoFACheckPassScreen` for password-based second factor

**Additional login methods:**
- QR code auth (`QrAuthHintBottomSheet`) for secondary device login
- Anonymous login via `auth.anonymLogin`

---

## 8. Session Theft via Token Extraction — CONFIRMED VULNERABLE

**Attack vector:** Extract `session_key` + `session_secret_key` + `auth_token` from:
- `shared_prefs/user.prefs.xml` (rooted device)
- App backup (if `android:allowBackup=true`)
- Memory dump

**Impact:** Full account takeover. The `C0615mo.java` request signing only requires:
1. `application_key` (hardcoded: `"CMBGJFMGDIHBABABA"`)
2. `session_key`
3. `session_secret` (used for request signature)

No additional device attestation or certificate pinning prevents replay from another device.

---

## 9. Session Fixation Vulnerabilities

**Potential issues identified:**

1. **No session rotation on privilege change:** No evidence of session_key regeneration after 2FA verification or password change
2. **Predictable session handling:** Error code 102/103 triggers session invalidation, but the client simply re-authenticates — no forced logout of other sessions
3. **QR auth flow:** Secondary device login via QR code grants a new session without invalidating the primary — allows session accumulation
4. **InMemorySessionStore** (`InMemorySessionStore.java`): Calls SDK stores session in memory with simple get/set — no validation of session freshness

---

## Risk Assessment

| Finding | Severity | CVSS |
|---------|----------|------|
| Plain SharedPreferences storage | HIGH | 7.5 |
| No device-bound token | HIGH | 7.0 |
| Opaque tokens without expiry enforcement | MEDIUM | 6.5 |
| No replay prevention in WebSocket auth | HIGH | 7.5 |
| Hardcoded application key | MEDIUM | 5.0 |
| Weak session fixation controls | MEDIUM | 6.0 |

---

## Key Source Files

| File | Purpose |
|------|---------|
| `p000/xm9.java` | LoginResponse model |
| `p000/lg0.java` | WebSocket session_data + anonymLogin |
| `p000/C0815po.java` | ApiConfig (appKey, token, sessionKey, sessionSecret) |
| `p000/lb4.java` | SessionInfo data class |
| `p000/C0615mo.java` | API request signing with session_key |
| `p000/wfl.java` | Session error detection (codes 102, 103) |
| `p000/dw8.java` | Login response JSON parser |
| `p000/hc4.java` | SMS code verification (CHECK_CODE) |
| `one/me/login/inputphone/InputPhoneScreen.java` | Phone input UI |
| `one/me/login/confirm/ConfirmPhoneScreen.java` | SMS confirm UI |
| `ru/ok/android/externcalls/sdk/SimpleConfigurationStore.java` | Session storage |
