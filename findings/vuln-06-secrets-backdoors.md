# Vulnerability Report: Secrets, Backdoors & Hidden Functionality

**App:** Max Messenger (ru.oneme.app)  
**Date:** 2026-05-15

---

## 1. Hardcoded API Keys

### Google Maps API Key (HIGH)
- **Location:** `AndroidManifest.xml`
- **Value:** `AIzaSyDJbuC3fODS_aR7jcOkoP6qWIsQen9XARI`
- **Risk:** Exposed API key can be abused for quota theft, billing attacks. Key is unrestricted in manifest metadata.

### Yandex Geocoding API Key (MEDIUM)
- **Location:** `p000/xwk.java` (line 50), `p000/jw7.java` (line 64)
- **Detail:** API key passed as constructor parameter (`f81250b`) and appended to geocoding URLs (`https://geocode-maps.yandex.ru/v1?...&apikey=`). Key value loaded at runtime but the pattern exposes it in network traffic.

### Tracer/Crash Reporting Endpoint (LOW)
- **Location:** `ru/p027ok/tracer/upload/SampleUploadWorker.java`, `p000/ati.java`
- **URL:** `https://sdk-api.apptracer.ru` with `sampleToken` and `uploadToken` parameters
- **Risk:** Crash data upload endpoint; tokens generated at runtime.

---

## 2. Debug Flags & Developer Options

### Developer Settings Class (`p000/ri9.java`) - (MEDIUM)
Extensive debug/dev options accessible in the app:
- `isDisableWebAppSsl` — **Can disable SSL for WebApps**
- `isCallsDebugMenuEnabled` — Hidden calls debug menu
- `allowLogSensitiveData` — Logs sensitive user data
- `isDebugProfileInfoEnabled` — Debug profile info overlay
- `isDebugFresco` — Image loading debug
- `isVideoDebugViewAvailable` — Video debug overlay
- `leakCanaryEnabledStateFlow` — Memory leak detector
- `areMockCommentsEnabled` — Fake/mock comments
- `isDevOptionsRoaming` — Dev roaming simulation

### Remote Config Flags (`p000/rtd.java`) - (MEDIUM)
Server-controlled feature flags include:
- `needToLogSensitive` — Server can enable sensitive data logging
- `isFakeChatsEnabled` — Enable fake chats
- `isDebugProfileInfoEnabled` — Remote debug toggle
- `isFullLogEnabled` — Full verbose logging
- `useLogcatLogger` — Logcat output (leaks data in shared logs)

### Debug Host Rotation (`p000/yag.java`) - (LOW)
- `isDebugHostRotationEnabled` — Debug server rotation
- `isDebugUaDnsEmulationEnabled` — DNS emulation for debugging

---

## 3. Hidden Activities (Non-Launcher)

Activities exported but not in LAUNCHER category:
- `one.me.android.deeplink.LinkInterceptorActivity` — exported, handles deep links with `autoVerify`
- `one.me.android.deeplink.NewWidgetActivity` — not exported, no launcher
- `one.me.android.calls.CallNotifierFixActivity` — shows on lock screen, turns screen on
- `com.google.android.gms.auth.api.signin.internal.SignInHubActivity` — Google sign-in (standard)

---

## 4. Cleartext Traffic Allowed (MEDIUM)

**File:** `res/xml/network_security_config.xml`

Cleartext HTTP permitted to Russian telecom domains:
- `mobileid.megafon.ru`
- `idgw.mobileid.mts.ru`
- `hhe.mts.ru`
- `he-mc.tele2.ru` / `he-mc.t2.ru`
- `balance.beeline.ru`

**Risk:** Authentication/identity data sent over unencrypted HTTP to mobile carrier endpoints. Susceptible to MITM on carrier networks.

---

## 5. Hardcoded Encryption Keys / IVs

No hardcoded symmetric keys or IVs found in the codebase. Crypto operations use standard Android `SecretKeySpec`/`IvParameterSpec` patterns with runtime-generated keys. TLS cipher suites referenced (`TLS_AES_128_GCM_SHA256`, etc.) are standard.

---

## 6. Backdoor-Like Code & Hidden Features

### SSL Bypass Toggle (HIGH)
- `isDisableWebAppSsl` in `ri9.java` — developer option that can disable SSL validation for WebApp content, enabling MITM attacks if toggled.

### Sensitive Data Logging (MEDIUM)
- `allowLogSensitiveData` (local) and `needToLogSensitive` (remote) — can be toggled to log sensitive user data. Remote flag means server can enable this silently.

### Mock/Fake Data Features (LOW)
- `isFakeChatsEnabled`, `areMockCommentsEnabled`, `isFakeInAppReviewEnabled` — test features that could be used to display fake content to users if enabled remotely.

### No Secret Codes Found
No `*#*#` patterns, hidden gesture detectors, or special user ID bypasses were identified.

---

## 7. Dynamic Code Loading (LOW)

### PathClassLoader Subclass (`p000/v7l.java`)
- Custom `PathClassLoader` that overrides `loadClass` — used for class loading customization.
- No evidence of `DexClassLoader` or `InMemoryDexClassLoader` usage for remote code.

### Class.forName Usage
- Standard reflection for platform compatibility checks (Android version detection, Conscrypt, Jetty ALPN, system properties).
- No server-controlled class name loading detected.

---

## 8. Analytics & Data Collection

### Device Metadata Collection (`p000/yk2.java`) - (MEDIUM)
Collects and transmits to `sdk-api.apptracer.ru`:
- Device model, hardware, manufacturer, fingerprint
- OS build, SDK version
- Network type and mobile subtype
- **SIM operator (MCC/MNC)** via `TelephonyManager.getSimOperator()`
- Timezone offset, locale, country
- App version code

### Contacts Access (MEDIUM)
- App requests `READ_CONTACTS` and `WRITE_CONTACTS` permissions
- `ContactsContract` usage for contact sync (`p000/ndi.java`, `p000/a40.java`)
- Contacts synced to server (evidenced by `contactsLastSync`, `contactSortLastSync` fields)

### No IMEI/SMS/Call Log Collection Found
- No `getDeviceId()`/`getImei()` calls for analytics purposes (only for VoIP call SDK device identification)
- No `READ_SMS` or `READ_CALL_LOG` permissions requested
- `getSimOperator()` collects carrier info but not subscriber identity

---

## Summary of Risk Levels

| Finding | Severity |
|---------|----------|
| Hardcoded Google Maps API key | HIGH |
| SSL disable toggle (dev option) | HIGH |
| Remote sensitive data logging flag | MEDIUM |
| Cleartext traffic to carrier endpoints | MEDIUM |
| Debug menus and dev options in release | MEDIUM |
| SIM operator + device fingerprint collection | MEDIUM |
| Contacts sync to server | MEDIUM |
| Yandex API key in URL construction | MEDIUM |
| Fake/mock content remote toggles | LOW |
| Custom ClassLoader (no RCE evidence) | LOW |
