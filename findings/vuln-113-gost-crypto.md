# VULN-113: GOST Cryptography Implementation Analysis

## Summary

Max messenger (ru.oneme.app, version 26.15.3) includes infrastructure for GOST TLS via BouncyCastle JSSE provider. The GOST support is currently **disabled in this build** (`Gost: false` in build info) but the code paths exist and can be activated server-side via PmsKey `gost-check-env`.

## Key Findings

### 1. GOST Purpose: TLS Transport Layer

GOST is used exclusively for **TLS connections** (transport security), not for message encryption or signing. The implementation is in the OkHttp platform provider selection chain (`pqd.java`).

The TLS provider selection logic in `pqd.java` (static initializer):
```
if ("BC".equals(Security.getProviders()[0].getName())) {
    boolean z3 = n31.f42383d;  // Check if BCJSSE class exists
    pqdVarM12971a = m31.m13913a();  // Create BouncyCastle JSSE provider
}
```

### 2. BouncyCastle JSSE Provider Setup (`n31.java`)

The `n31` class extends `pqd` (OkHttp platform abstraction) and:
- Creates `BouncyCastleJsseProvider` instance
- Provides `SSLContext.getInstance("TLS", this.f42384c)` — TLS via BC provider
- Uses `TrustManagerFactory.getInstance("PKIX", "BCJSSE")` for certificate validation
- **Does NOT call `Security.addProvider()` or `Security.insertProvider()`** — the provider is used directly without registering globally

The provider is only activated if `BouncyCastleJsseProvider` class is loadable AND the system's first security provider is named "BC".

### 3. Server-Controlled GOST Activation (`gost-check-env` PmsKey)

**PmsKey**: `gost-check-env` (enum ordinal 332)
**Field**: `f88705gostcheckenv` → stored in `qp6.f57365B2`
**Type**: Integer (default value: **15** — bitmask/flags)
**Accessor**: `getGostEnvironmentCheckFlags()` returns `int`

The value is consumed in `kag.java:237`:
```java
((Number) qp6Var.f57365B2.mo677x(qp6Var, qp6.f57357S2[155])).intValue();
// Used to create: new bmh(context)
```

This creates a `bmh` instance — a custom SSL socket factory wrapper that:
- Uses `SSLCertificateSocketFactory` (Android system)
- Creates SSL session cache in `tamtam_sslcache` directory
- Sets custom `TrustManagers` via `tfi` class
- Performs hostname verification via `HttpsURLConnection.getDefaultHostnameVerifier()`

**The flags value (default 15 = 0xF) likely controls which environment checks to perform** before enabling GOST TLS. Since it's server-pushed via PMS, the server can modify these flags remotely.

### 4. GOST License Check (`gostLicenseCheckEnabled`)

Found in `ri9.java` (local preferences/settings class):
- Property: `gostLicenseCheckEnabled` (boolean, stored in SharedPreferences)
- Accessor: `getGostLicenseCheckEnabled()Z`
- This is a **client-side preference** (not server-pushed PmsKey)
- Likely controls whether to verify CryptoPro CSP license before using GOST ciphers

### 5. GOST as Feature Toggle Category

In the dev menu feature toggles system (`C0528ke.java`), GOST is category **8** in the `gog` feature toggle classification:
```java
case 8: str2 = "GOST"; break;
```

This means there are feature toggles specifically categorized under "GOST" that can be searched/filtered in the developer menu.

### 6. Build Configuration

From `pr4.java` (build info screen):
```
Version: 26.15.3(6695)
AppId: ru.oneme.app
Gost: false
UseNarnia: false
UsePersonalCloud: false
```

**GOST is a compile-time build variant flag** — this Google Play Store build has it disabled. A separate build (likely for Russian government/enterprise distribution) would have `Gost: true`.

### 7. TLS Provider Selection Chain

The full provider priority in `pqd.java`:
1. **Android 29+ native** (`C0017ag`) — if Dalvik VM detected
2. **Conscrypt** (`ng4`) — if first provider is "Conscrypt"
3. **BouncyCastle JSSE** (`n31`) — if first provider is "BC" (GOST-capable)
4. **OpenJSSE** (`l1d`) — if first provider is "OpenJSSE"
5. **Fallback chain** (`zu8`, `AbstractC1279ww`, base `pqd`)

## Security Concerns

### Server-Side Control of Crypto
- The `gost-check-env` PmsKey allows the server to push integer flags controlling GOST environment checks
- Default value is 15 (all flags set), but server can change to 0 (no checks)
- This could potentially be used to **downgrade** TLS security if GOST provider has weaker cipher suites

### GOST TLS Weaknesses in This Implementation
1. **No cipher suite restriction**: `n31` does not override `getDefaultCipherSuites()` or `getSupportedCipherSuites()` — it accepts whatever BC provider offers
2. **ALPN not configured**: `p31.mo2529a()` always returns `false` — ALPN protocol negotiation is never applied for BC sockets
3. **No certificate pinning**: The BCJSSE path uses standard PKIX trust with no additional pinning
4. **Session cache on disk**: `zih` stores SSL sessions in `tamtam_sslcache` directory — extractable on rooted devices

### Potential Attack Vector
If a GOST-enabled build is deployed:
- Server pushes `gost-check-env = 0` (disable all environment checks)
- BC provider becomes active if system has BC as first security provider
- GOST cipher suites (which may have implementation weaknesses in BouncyCastle) become available
- No ALPN negotiation means potential protocol confusion

## Files Analyzed

| File | Role |
|------|------|
| `p000/pqd.java` | TLS platform provider selection (OkHttp) |
| `p000/n31.java` | BouncyCastle JSSE TLS provider wrapper |
| `p000/m31.java` | Factory for n31 (BCJSSE provider) |
| `p000/p31.java` | BCJSSE ALPN handler (disabled) |
| `p000/qp6.java` | PMS settings including gost-check-env |
| `p000/no6.java` | PmsKey value reader (case 12 = Integer, default 15) |
| `p000/kag.java` | Consumer of gostEnvironmentCheckFlags |
| `p000/bmh.java` | Custom SSL socket factory with GOST awareness |
| `p000/zih.java` | SSL socket factory using SSLCertificateSocketFactory |
| `p000/ri9.java` | Local prefs with gostLicenseCheckEnabled |
| `p000/C0528ke.java` | Dev menu feature toggle categories (GOST = 8) |
| `p000/pr4.java` | Build info showing Gost: false |
| `PmsKey.java` | gost-check-env PmsKey definition (ordinal 332) |

## Risk Assessment

- **Current risk**: LOW — GOST is disabled in this build variant
- **Potential risk**: MEDIUM — If GOST build is deployed, server can weaken environment checks
- **Architecture risk**: The server has unilateral control over crypto environment flags without client consent
