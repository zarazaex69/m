# Host Reachability & GOST Environment Check

## A) Host Reachability

### Overview
The app periodically checks if push notification servers and its own API are reachable. If push hosts are unreachable but `api.oneme.ru` is reachable, it starts a foreground service (`BackgroundListenService`) to maintain connectivity via a persistent socket — a "carpet mode" workaround for blocked push services.

### Feature Flag
- PMS key: `host-reachability` (PmsKey.f88709hostreachability)
- Remote toggle: `isHostReachabilitySendEnabled` in `qp6.java`
- Config class: `gn0` (BackgroundWakeConfig) with fields:
  - `checkBackgroundIntervalMinutes`
  - `suggestionIntervalMinutes`  
  - `checkForegroundIntervalSec`

### Hosts Checked

Decoded from obfuscated int arrays in `xpe.java`:

| Field | Host | Purpose |
|-------|------|---------|
| f80726a | `mtalk.google.com` | Google FCM/GCM push |
| f80728c | `pushtrs.push.hicloud.com` | Huawei HMS push |
| f80730e | `pushtrs1.push.hicloud.com` | Huawei HMS push (backup) |
| f80732g | `token-drcn.push.dbankcloud.com` | Huawei push token service |

The oneMe API host is hardcoded: `api.oneme.ru`

```java
// xpe.java - obfuscated host arrays
public static final int[] f80726a = {109, 116, 97, 108, 107, 46, 103, 111, 111, 103, 108, 101, 46, 99, 111, 109};
// = "mtalk.google.com"

public static final int[] f80728c = {112, 117, 115, 104, 116, 114, 115, 46, 112, 117, 115, 104, 46, 104, 105, 99, 108, 111, 117, 100, 46, 99, 111, 109};
// = "pushtrs.push.hicloud.com"
```

### Push Type Selection (wpe enum → host list)

```java
// wpe.java
public enum wpe {
    HUAWEI("HUAWEI"),  // ordinal 0 → checks all 3 Huawei hosts
    GCM("GCM"),        // ordinal 1 → checks mtalk.google.com only
    RUSTORE("RUSTORE");
}
```

From `v48.java`:
```java
// If HUAWEI push type (ordinal 0 → switch maps to 1):
//   check pushtrs.push.hicloud.com, pushtrs1.push.hicloud.com, token-drcn.push.dbankcloud.com
// Otherwise (GCM):
//   check mtalk.google.com only
List listM19379L = u48.$EnumSwitchMapping$0[1] == 1 
    ? r04.m19379L(xpe.f80729d.getValue(), xpe.f80731f.getValue(), xpe.f80733h.getValue()) 
    : Collections.singletonList(xpe.f80727b.getValue());
```

### Check Mechanism

1. **Push hosts** — TCP socket connect to port 443 with 3000ms timeout (`s48.java`):
```java
((Socket) xjfVar.f80196a).connect(new InetSocketAddress(str, 443), 3000);
```

2. **oneMe host** — First tries DNS resolution + ICMP ping (`uq5.m23041a`), falls back to socket connect:
```java
// hu7.java case 4:
return Boolean.valueOf(((x48) obj).f79042a.m23041a(x48.f79041e, "api.oneme.ru"));
```

3. **Result** — `t48` class:
```java
// t48.java
public final String toString() {
    return "Result(pushReachable=" + this.f66293a + ", oneMeReachable=" + this.f66294b + ")";
}
// shouldRun = !pushReachable && oneMeReachable
public final boolean m21647c() { return !this.f66293a && this.f66294b; }
```

### Action on Unreachable Push

From `on0.java` — if push unreachable but oneMe reachable:
```java
if (z3) {  // shouldRun = true
    ct4.m4610E("KeepBackground", "reachabilityCheck: ENTERING foreground");
    ok9.m16044h(pn0Var.f53859Y.m21369a(), "BACKGROUND_MODE", "carpet_mode_on", null, 12);
    jil.m11441b(pn0Var.f53861a);  // start BackgroundListenService
}
```

### Analytics Reporting

Stats sent via `ok9.m16049g()` with:
- type: `"BACKGROUND_MODE"`
- events: `"carpet_mode_on"`, `"work_in_background_permission"`, `"snack_shown"`, `"snack_click_on"`, `"snack_hidden"`

The stat event includes session ID and timestamp, sent to server analytics pipeline.

### Scheduling

`pn0.java` schedules exact alarms via `AlarmManager.setExactAndAllowWhileIdle()` using `BackgroundCheckReceiver`. The interval comes from `gn0.f21894b` (checkBackgroundIntervalMinutes).

---

## B) GOST Environment Check

### Overview
The app has infrastructure for GOST (Russian state cryptographic standard) but it appears **disabled in this build** (`UseNarnia: false, Gost: false`). A `gostEnvironmentCheckFlags` integer bitmask is received from server via PMS.

### Feature Flags
- PMS key: `gost-check-env` (PmsKey.f88705gostcheckenv) → `gostEnvironmentCheckFlags` (int bitmask)
- Local pref: `gostLicenseCheckEnabled` in `ri9.java` (dev/debug toggle)
- Build config in `pr4.java`:
```java
sbM25894y.append("\n    UseNarnia: false\n    Gost: false\n    UsePersonalCloud: false\n    BuildTime: ");
```

### Implementation

In `kag.java` (DI factory, case 29):
```java
Context context = (Context) abstractC1363z5.m26509c(23);
qp6 qp6Var = (qp6) ((ho6) abstractC1363z5.m26509c(41));
((Number) qp6Var.f57365B2.mo677x(qp6Var, qp6.f57357S2[155])).intValue();
return new bmh(context);
```

The `gostEnvironmentCheckFlags` value is read but the result is discarded — `bmh` is created unconditionally. This suggests the flags may control behavior inside `bmh` at runtime or the check was simplified in this build.

### SSL/TLS Implementation (bmh.java)

`bmh` is the app's custom SSL socket factory manager. It creates `zih` (SSLSocketFactory subclass) which uses:
- `SSLCertificateSocketFactory` with session caching (`tamtam_sslcache` dir)
- Custom `tfi` TrustManager for certificate validation

```java
// bmh.java
public final SSLSocketFactory m2839a(String str) throws SSLException {
    zih zihVar = new zih(this.f6032c, str);
    // ...
    return zihVar;
}
```

### Certificate Validation (tfi.java)

The `tfi` class is a custom `X509TrustManager` that:
1. Uses system default trust store (`TrustManagerFactory.init(null)`)
2. Performs hostname verification via `X509TrustManagerExtensions.checkServerTrusted()`
3. On failure, logs certificate chain details and specifically looks for **Comodo** certificates in the trust store

```java
// tfi.java - checkServerTrusted error handling
if ((name != null && name.toLowerCase(locale).contains("comodo")) || 
    (name2 != null && name2.toLowerCase(locale).contains("comodo"))) {
    z5l.m26532G(sb2, x509Certificate);
}
```

### GOST Category in Dev Menu

From `C0528ke.java` — GOST is listed as a feature toggle category (case 8):
```java
case 8:
    str2 = "GOST";
    break;
```

### Mintsifry (Ministry of Digital Development) Certificates

No direct reference to "Mintsifry" certificates found in the codebase. The GOST infrastructure appears to be **prepared but not active** in this build:
- `UseNarnia: false` and `Gost: false` in build config
- The `gostEnvironmentCheckFlags` is read from server but the SSL factory (`bmh`) doesn't branch on it
- The TrustManager uses the **system default** trust store, meaning it would trust Mintsifry root CAs only if they're installed on the device's system trust store

### Conclusion

The GOST support is a server-controlled feature that can be enabled via the `gost-check-env` PMS flag. When enabled (in a different build variant or via server toggle), it likely switches to GOST-compatible TLS cipher suites. The current public build has it disabled.
