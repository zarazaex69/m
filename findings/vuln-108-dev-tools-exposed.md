# vuln-108: Dev Tools Exposed + Test Server Redirect + API Server List in SharedPrefs

## Severity: HIGH

## Finding 1: Test API Servers Accessible in Production (HIGH)

**File**: `p000/r58.java:37`

```java
List listM19379L = r04.m19379L("api.oneme.ru", "api-test.oneme.ru", "api-tg.oneme.ru", "api-test2.oneme.ru");
```

Production app contains hardcoded test server URLs. Server selection stored in `dev_tools` SharedPreferences.

## Finding 2: Deep Links Accept Test Servers (MEDIUM-HIGH)

**File**: `p000/C0414h9.java:716`

```java
if (!uri.getScheme().equalsIgnoreCase("max") 
    ? TextUtils.isEmpty(host) || pathSegments.size() <= 0 || 
      (!host.equalsIgnoreCase("max.ru") && 
       !host.equalsIgnoreCase("api-test.oneme.ru") && 
       !host.equalsIgnoreCase("api-tg.oneme.ru"))
    : ...)
```

Deep link handler accepts `api-test.oneme.ru` and `api-tg.oneme.ru` as valid hosts.
An attacker who controls these test domains (or DNS) can craft deep links that redirect to attacker infrastructure.

## Finding 3: Dev Tools SharedPreferences (MEDIUM)

**File**: `p000/r58.java:34`

```java
this.f59034o = context.getSharedPreferences("dev_tools", 0);
```

Developer tools configuration stored in SharedPreferences. On rooted device or via ADB backup (if enabled), attacker can:
- Switch API server to attacker-controlled server
- Enable debug features
- Modify app behavior

## Finding 4: Server Host Stored in User Preferences (MEDIUM)

**File**: `p000/yag.java` properties include:
- `currentProxyList` — proxy server list
- `pushProxyList` — push proxy list  
- `lastSuccessProxy` — last used proxy
- `isDebugHostRotationEnabled` — debug host rotation
- `isDebugUaDnsEmulationEnabled` — DNS emulation
- `useTls` — TLS toggle (can be disabled!)

These are stored in SharedPreferences and can be modified to redirect all traffic.

## Finding 5: `useTls` Toggle (CRITICAL if exploitable)

The `useTls` property in SharedPreferences controls whether TLS is used for connections.
If set to false, ALL traffic goes in cleartext. Combined with the trust-all QUIC TrustManager,
this means the app has MULTIPLE paths to completely unencrypted communication.

## Impact

- Test servers may have weaker security, different auth, or be decommissioned (DNS takeover)
- Deep links can redirect users to test infrastructure
- SharedPrefs modification (root/ADB) can redirect all traffic to attacker
- `useTls=false` disables encryption entirely
