# VULN-08: Certificate Pinning Analysis

## Summary

Max messenger implements certificate validation through multiple layers (custom TrustManagers, OkHttp hostname verification, custom SSLSocketFactory), but contains several bypass vectors including a trust-all TrustManager in QUIC connections, a remotely-configurable SSL validation flag, and a WebView SSL bypass controlled by a local preference.

## Severity: HIGH

---

## 1. TrustManager Implementations

### 1.1 Trust-All TrustManager — `qse.java` (CRITICAL)

```java
// p000/qse.java - loaded from classes3.dex
public final class qse implements X509TrustManager {
    public final void checkClientTrusted(X509Certificate[] x509CertificateArr, String str) { }
    public final void checkServerTrusted(X509Certificate[] x509CertificateArr, String str) { }
    public final X509Certificate[] getAcceptedIssuers() { return null; }
}
```

**Impact**: Completely disables certificate validation. Empty `checkServerTrusted()` accepts ANY certificate.

**Usage** (`z68.java:826`): Used in QUIC/HTTP3 connections via the `kwik` library:
```java
qse qseVar = new qse();
tpiVar.f67981s = qseVar;  // Sets trust-all on QUIC TLS engine
```
The code even prints a warning: `"SECURITY WARNING: INSECURE configuration! Server certificate validation is disabled; QUIC connections may be subject to man-in-the-middle attacks!"`

### 1.2 Composite TrustManager — `e94.java`

Chains multiple TrustManagers with OR logic — if ANY one trusts the cert, it passes. Used in `l88.java` to combine system trust store with a custom root CA (`rootca_ssl_rsa2022` from raw resources). This is standard practice but expands the trust surface.

### 1.3 Host-Aware TrustManager — `tfi.java`

Custom TrustManager used by `zih.java` (SSLSocketFactory for "tamtam" connections). Uses `X509TrustManagerExtensions.checkServerTrusted()` with hostname parameter when available, falls back to standard validation otherwise. Properly throws on failure.

---

## 2. OkHttp Certificate Pinning

### 2.1 CertificatePinner — `am2.java`

The OkHttp CertificatePinner (`am2`) is initialized as **EMPTY** by default:
```java
public static final am2 f1925c = new am2(q04.m18512h1(new ArrayList()), null);
// Empty pin set = NO PINNING
```

The pin set `f1926a` is an empty `Set`. In `lcf.java`, the pinning check iterates over `am2Var2.f1926a` — with an empty set, the iterator has no elements, so **no pins are ever checked**.

### 2.2 Hostname Verification in `lcf.java`

The connection setup in `lcf.java:m13250g()` does verify hostname via `c0417hc.f23941d.verify()`, but the CertificatePinner step is effectively a no-op with the empty pin set.

---

## 3. NetworkSecurityConfig

**File**: `~/max/decompiled/apktool/base/res/xml/network_security_config.xml`

```xml
<network-security-config>
    <domain-config cleartextTrafficPermitted="true">
        <domain includeSubdomains="false">mobileid.megafon.ru</domain>
        <domain includeSubdomains="false">idgw.mobileid.mts.ru</domain>
        <domain includeSubdomains="false">hhe.mts.ru</domain>
        <domain includeSubdomains="false">he-mc.tele2.ru</domain>
        <domain includeSubdomains="false">he-mc.t2.ru</domain>
        <domain includeSubdomains="false">balance.beeline.ru</domain>
    </domain-config>
</network-security-config>
```

**Findings**:
- **No certificate pinning** defined in NSC (no `<pin-set>` elements)
- **No `<trust-anchors>`** restricting to system CAs only
- **Cleartext HTTP allowed** to 6 carrier domains (mobile ID/balance services)
- No `<debug-overrides>` section — user CAs not explicitly allowed on debug builds via NSC
- No base-config restricting cleartext globally

---

## 4. Conditions Where Pinning is Disabled

### 4.1 QUIC Connections (Always Disabled)
All QUIC/HTTP3 connections use `qse` (trust-all). No certificate validation whatsoever.

### 4.2 Empty CertificatePinner
The OkHttp `am2` CertificatePinner is instantiated with an empty pin set, meaning standard TLS connections rely solely on system CA trust store — no pin enforcement.

### 4.3 Cleartext Domains
Six carrier domains allow unencrypted HTTP traffic.

---

## 5. Remote Config Flag — `net-ssl-session-validate`

**PmsKey**: `f88777netsslsessionvalidate` → `"net-ssl-session-validate"` (index 246)

- Controlled via remote PMS (Parameter Management Service) configuration
- Read in `qp6.java` as `ro6` type (boolean remote config)
- Value sent in session parameters (`ubi.java:74`): `"net-ssl-session-validate"` = `String.valueOf(qp6.m19054R())`
- Default appears to be `Boolean.TRUE` based on `ezd.java` debug settings UI

**Impact**: This flag can be toggled server-side to enable/disable SSL session validation. If set to `false`, SSL session validation is skipped for network connections.

---

## 6. HostnameVerifier Implementations

### 6.1 `xac.java` — Standard OkHttp HostnameVerifier
Proper implementation checking SAN (Subject Alternative Names) and wildcard matching. Used as default verifier. No bypass vectors found.

### 6.2 `bch.java` — Custom HostnameVerifier (WEAK)

```java
public final boolean verify(String str, SSLSession sSLSession) {
    String str2 = (String) this.f5187a.invoke();  // Gets configured hostname
    List list = (List) this.f5188b.invoke();      // Gets allowed hostname list
    return (str2 == null || (!str.equals(str2) && (list == null || !list.contains(str))))
        ? HttpsURLConnection.getDefaultHostnameVerifier().verify(str, sSLSession)
        : HttpsURLConnection.getDefaultHostnameVerifier().verify(str2, sSLSession);
}
```

**Weakness**: When the requested hostname matches a configured hostname or is in the allowed list, it verifies the session against `str2` (the configured hostname) instead of the actual hostname. This allows hostname substitution — if an attacker controls DNS, they could redirect traffic to a different host that has a valid cert for `str2`.

### 6.3 `mg4.java` — ConscryptHostnameVerifier
Singleton with no visible `verify()` method in decompiled output. Set on Conscrypt TrustManager in `ng4.java:96`. Likely delegates to default behavior.

---

## 7. WebView SSL Handling

### 7.1 `xyc.java` — WebViewClient with SSL Bypass

```java
public void onReceivedSslError(WebView webView, SslErrorHandler sslErrorHandler, SslError sslError) {
    if (!((opk) this.f81673b).mo6696i()) {
        sslErrorHandler.cancel();  // Reject SSL errors
    } else {
        sslErrorHandler.proceed();  // BYPASS: Accept invalid certificates!
    }
}
```

**Control**: `mo6696i()` returns `ri9.m19924S()` which reads the **local preference** `"web_app:ssl_check"` (Boolean).

**Impact**: When `web_app:ssl_check` is `true`, ALL SSL errors in WebViews are silently accepted. This is a developer/debug toggle stored in SharedPreferences.

### 7.2 WebView Does NOT Use App Pinning
WebView connections use Android's system WebView SSL stack, completely independent of the app's OkHttp/custom TrustManager setup. No pinning is applied to WebView traffic.

### 7.3 Affected WebView Screens
- `VideoWebViewScreen.java` — Video playback
- `ahk.java` — Web apps
- `syc.java` — General web content

---

## 8. Custom CA Addition Vectors

### 8.1 Bundled Root CA — `rootca_ssl_rsa2022`

In `l88.java`, the app loads a custom root CA from raw resources (`R.raw.rootca_ssl_rsa2022`) and adds it to the trust store alongside system CAs:

```java
Certificate cert = CertificateFactory.getInstance("X509")
    .generateCertificate(resources.openRawResource(m2f.rootca_ssl_rsa2022));
keyStore.setCertificateEntry(resourceEntryName, (X509Certificate) cert);
// Combined with system TrustManager via e94 (OR-logic)
```

**Impact**: If this bundled CA's private key is compromised, all connections using this path can be MITMed.

### 8.2 No Intent/File-Based CA Addition
No evidence of dynamic CA loading via intents, file system paths, or content providers. The custom CA is compiled into the APK as a raw resource.

---

## Bypass Vectors Summary

| Vector | Difficulty | Impact |
|--------|-----------|--------|
| QUIC connections (trust-all `qse`) | Low — force QUIC negotiation | Full MITM on QUIC traffic |
| Empty CertificatePinner | Low — standard proxy setup | MITM with any valid CA cert |
| `web_app:ssl_check` preference | Medium — requires root/backup to set pref | WebView MITM |
| `net-ssl-session-validate` remote flag | Medium — requires server compromise or MITM of config endpoint | Disable SSL session validation |
| `bch` hostname substitution | Medium — DNS + valid cert for target domain | Redirect specific connections |
| Cleartext carrier domains | Low — network position | Sniff/modify carrier auth traffic |
| Bundled `rootca_ssl_rsa2022` key compromise | High — requires CA key | Full MITM |

---

## Recommended Bypass Approach (For Security Testing)

1. **Easiest**: Use standard MITM proxy (Burp/mitmproxy) with user CA installed — no pinning enforcement exists in OkHttp layer
2. **QUIC traffic**: Already has no validation; just intercept
3. **WebView**: Set `web_app:ssl_check` to `true` in SharedPreferences via `adb shell` or backup manipulation
4. **Frida hook**: Patch `tfi.checkServerTrusted()` for the custom SSLSocketFactory path
