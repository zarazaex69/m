# VULN-103: Final Miscellaneous Vulnerability Findings

**App:** Max Messenger (decompiled)  
**Date:** 2026-05-15  
**Source:** ~/max/decompiled/jadx/base/sources

---

## 1. Insecure Random in Crypto/Security Context

### CRITICAL: `java.util.Random` used for security-sensitive byte generation

| File | Issue |
|------|-------|
| `p000/g6l.java:9` | **Static `new Random()` (unseeded)** shared across app |
| `p000/xcl.java:46` | `g6l.f20556a.nextBytes(bArr)` — generates 16 random bytes using insecure PRNG for **Google Sign-In nonce/state parameter** |
| `p000/pwl.java:26` | `g6l.f20556a.nextBytes(bArr)` — generates 16 random bytes encoded as Base64, used as **session identifier for GMS service connection** |
| `p000/xrf.java:9` | `new Random(System.currentTimeMillis())` — time-seeded, predictable |
| `p000/i6l.java:58` | `Math.random() * 255.0d` — used as **NTP packet byte** (low-entropy randomization) |
| `p000/j6l.java:71` | Same `Math.random()` pattern for NTP packet |
| `p000/uqd.java:11-23` | `ThreadLocalRandom.current()` — not cryptographically secure, used as general random source |

**Impact:** The static `java.util.Random` in `g6l` is used to generate what appear to be security tokens (Base64-encoded 16-byte values for Google Sign-In and GMS). `java.util.Random` uses a 48-bit linear congruential generator — an attacker who observes one output can predict all future outputs, enabling session hijacking or OAuth state parameter prediction.

**Severity:** HIGH  
**Recommendation:** Replace `g6l.f20556a` with `SecureRandom` for all security-sensitive byte generation.

---

## 2. Hardcoded IPs/Hosts Bypassing DNS

| File | IP | Context |
|------|-----|---------|
| `p000/tfb.java:86` | `0.0.0.0` | ICE candidate anonymization (WebRTC `raddr` replacement) |
| `p000/z48.java:450` | `127.0.0.1` | Localhost check in network code |
| `org/apache/http/conn/params/ConnRouteParams.java:15` | `127.0.0.255` | Apache HTTP "no-host" sentinel |

**Impact:** LOW — These are standard sentinel/placeholder IPs used in WebRTC ICE candidate processing and localhost detection. No attacker-targetable production server IPs were found hardcoded. The `0.0.0.0` in `tfb.java` is actually a privacy feature (anonymizing ICE candidates).

**Severity:** LOW (informational)

---

## 3. Unsafe Reflection

### Findings:

| File | Pattern | Risk |
|------|---------|------|
| `p000/kti.java:42` | `Class.forName(string)` where `string` comes from **AndroidManifest metadata** (`bundle.getString(str)`) | MEDIUM — manifest metadata is app-controlled but could be manipulated on rooted devices |
| `p000/ijj.java:50` | `Class.forName(str, true, ...)` where `str` is passed as parameter to `m10427c()` | MEDIUM — Parcelizer class loading from string name |
| `CallScreen.java:872-873` | `RecyclerView.class.getDeclaredField("o1")` + `setAccessible(true)` | LOW — hardcoded field name |
| `MediaBarPreviewLayout.java:579-586` | Multiple `getDeclaredField` + `setAccessible` on `TextView` internals | LOW — hardcoded field names |
| `ConversationFactory.java:659-660` | `Build.VERSION.class.getDeclaredField("SEM_PLATFORM_INT")` | LOW — Samsung-specific check |

**Key concern — `kti.java`:** Loads classes from AndroidManifest `<meta-data>` keys matching `ru.ok.tracer.startup.Initializer@*`. On a rooted/compromised device, an attacker could modify the manifest to inject arbitrary class names, achieving code execution within the app's process.

**Severity:** MEDIUM  
**Recommendation:** Validate loaded class names against an allowlist before calling `Class.forName()`.

---

## 4. Logging Sensitive Data

### Findings:

| File | Log Content | Risk |
|------|-------------|------|
| `p000/sb2.java:67` | `ct4.m4610E("CallsCredRepositoryTag", "Ok token will be expired in " + jM25813q)` | MEDIUM — logs token expiry time (aids timing attacks) |
| `p000/sb2.java:92` | `ct4.m4610E("CallsCredRepositoryTag", "Ok token updated.")` | LOW — confirms token refresh events |
| `FcmMessagingService.java:139` | `ct4.m4610E(this.f50439Z, "onNewToken")` | LOW — logs FCM token refresh event (not the token value itself) |
| `SettingsLocaleScreen.java:194` | `ct4.m4610E(..., "Restarting session")` | LOW — session lifecycle event |
| `NotificationTamService.java:1088` | Logs `pushId` and `eventKey` values | MEDIUM — push notification identifiers in logcat |

**Impact:** While the actual token *values* don't appear to be logged directly, the `sb2.java` class logs token lifecycle events with timing information. On Android < 4.1, any app could read logcat; on newer versions, ADB access or a compromised app with `READ_LOGS` permission could harvest this data. The `NotificationTamService` logs push IDs which could be used for notification replay.

**Severity:** MEDIUM  
**Recommendation:** Remove or gate all token/session-related logging behind `BuildConfig.DEBUG`.

---

## 5. Temporary File Race Conditions

### Findings:

| File | Pattern | Risk |
|------|---------|------|
| `p000/wd6.java:488` | `File.createTempFile("temp", "tmp")` | HIGH — **predictable prefix "temp" + default temp directory**, no custom directory specified |
| `p000/ur5.java:621` | `File.createTempFile("log_", ".txt")` | MEDIUM — log data extracted to predictable temp file |
| `p000/ura.java:140` | `File.createTempFile("PersistedInstallation", "tmp", ...)` | LOW — uses app's `filesDir` (private) |
| `p000/nc5.java:159` | `File.createTempFile(str + ".", ".tmp", file)` | LOW — uses specified directory |
| `net/jpountz/lz4/LZ4JNI.java:37` | `File.createTempFile("liblz4-java-", ...)` | MEDIUM — native library extracted to temp |
| `p000/orb.java:15` | `System.getProperty("java.io.tmpdir")` — lists and deletes temp files | LOW — cleanup routine |
| `com/facebook/soloader/C0163c.java:32` | `/data/local/tmp/exopackage/` — hardcoded world-readable path | HIGH — native libs loaded from world-readable location |

**Critical issues:**

1. **`wd6.java`** (ExifInterface): Creates temp file with predictable name in default temp directory without specifying the app's private directory. On shared storage, another app could race to create a symlink at the predicted path, potentially redirecting writes or reading image EXIF data.

2. **`C0163c.java` (Facebook SoLoader)**: Loads native libraries from `/data/local/tmp/exopackage/<package>/native-libs/`. This path is world-writable via ADB. An attacker with ADB access could replace native libraries with malicious ones, achieving code execution.

**Severity:** HIGH (for SoLoader path), MEDIUM (for temp file races)  
**Recommendation:** 
- Always specify app-private directories for temp files
- Remove or disable exopackage loading in release builds
- Use `context.getCacheDir()` instead of system temp directory

---

## Summary

| # | Category | Severity | Key Finding |
|---|----------|----------|-------------|
| 1 | Insecure Random | HIGH | `java.util.Random` generates security tokens (Google Sign-In nonce, GMS session ID) |
| 2 | Hardcoded IPs | LOW | Only sentinel/placeholder IPs found |
| 3 | Unsafe Reflection | MEDIUM | `Class.forName()` from manifest metadata — injectable on rooted devices |
| 4 | Sensitive Logging | MEDIUM | Token lifecycle and push IDs logged to logcat |
| 5 | Temp File Race | HIGH | Native libs loaded from world-writable path; predictable temp files in shared directories |
