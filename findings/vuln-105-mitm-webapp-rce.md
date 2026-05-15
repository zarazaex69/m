# vuln-105: MITM + WebApp Bot → Intent Injection → RCE Chain

## Severity: CRITICAL (0-click from malicious bot, 1-click from MITM)

## Summary

Three interconnected vulnerabilities form a complete RCE chain:

1. **Trust-All TrustManager** (`qse.java`) — accepts ANY certificate
2. **WebApp JS Bridge → Intent Injection** — bot can open arbitrary URIs including `intent://`
3. **Conditional SSL bypass in WebView** — `isDisableWebAppSsl` SharedPrefs flag

## Finding 1: Trust-All X509TrustManager (CRITICAL)

**File**: `p000/qse.java`

```java
public final class qse implements X509TrustManager {
    public final void checkClientTrusted(X509Certificate[] x509CertificateArr, String str) {
        // EMPTY - accepts all
    }
    public final void checkServerTrusted(X509Certificate[] x509CertificateArr, String str) {
        // EMPTY - accepts all
    }
    public final X509Certificate[] getAcceptedIssuers() {
        return null;
    }
}
```

This is a production trust-all TrustManager. Any MITM can intercept all HTTPS traffic.

## Finding 2: WebApp Bot Intent Injection (CRITICAL)

**File**: `p000/ghk.java:148`

```java
} else if (nikVar instanceof xhk) {
    String str5 = ((xhk) nikVar).f80073a;
    if (str5.length() != 0) {
        try {
            webAppRootScreen.startActivity(new Intent("android.intent.action.VIEW", Uri.parse(str5)));
        } catch (ActivityNotFoundException e) { ... }
    }
}
```

**Attack path**:
1. Bot WebView loads attacker page
2. Page navigates to arbitrary URL
3. `shouldOverrideUrlLoading` in `xyc.java` (case 1) passes URL to `mo7685h()` → creates `xhk`
4. `ghk.java` calls `startActivity(ACTION_VIEW, Uri.parse(url))` with NO scheme validation

**Exploitable schemes**:
- `intent://` — launch arbitrary activities with extras
- `file:///data/data/ru.oneme.app/...` — access app private files
- `content://ru.oneme.app.provider/...` — access content providers
- Custom schemes of other installed apps

**No URL validation exists** — only `str5.length() != 0` check.

## Finding 3: WebView SSL Bypass Flag (HIGH)

**File**: `p000/ri9.java:171`

```java
this.f60398B0 = new bbi(i5, "web_app:ssl_check", bool, this.f17707e, zjf.m27080a(Boolean.class));
```

Property name: `isDisableWebAppSsl` (index 15 in property array)

When true, `xyc.java:123` calls `sslErrorHandler.proceed()` — WebView accepts invalid SSL certs.

**Stored in**: SharedPreferences (plaintext, no encryption)
**Exploitable via**: ADB backup, root, or if server can set this flag

## Finding 4: JS Bridge Events (MEDIUM)

**File**: `p000/ijk.java:235`

Allowed events requiring user click within 3000ms:
- `WebAppMaxShare` — share content within Max
- `WebAppShare` — share to external apps
- `WebAppDownloadFile` — download files to device
- `WebAppOpenLink` — open arbitrary URLs (→ intent injection!)
- `WebAppOpenMaxLink` — open Max deeplinks

**Private bridge** (`i6e.java`): `postEvent(name, data)` with `isPrivate=true` — bypasses click requirement for whitelisted bots.

## Finding 5: resolveShare — Bot File Write (MEDIUM)

**File**: `p000/upk.java:20`

```java
@JavascriptInterface
public final void resolveShare(String str, byte[] bArr, String str2, String str3) {
    // Writes arbitrary bytes to device
}
```

Bot can write arbitrary byte arrays to the device via the JS bridge.

## RCE Chain

```
Malicious Bot → WebApp WebView
  → JavaScript calls window.open("intent://...")
  → shouldOverrideUrlLoading passes to mo7685h()
  → xhk created with intent:// URL
  → ghk.java: startActivity(ACTION_VIEW, Uri.parse("intent://..."))
  → Arbitrary activity launch with controlled extras
  → RCE via exported activities or file:// access
```

## MITM Enhancement

With MITM (trivial due to qse.java trust-all):
1. Intercept bot WebApp HTTPS response
2. Inject malicious JavaScript
3. JS triggers intent:// navigation
4. 0-click RCE (bot auto-opens WebView on message)

## Impact

- **0-click** if victim has auto-preview for bot messages
- **1-click** if victim opens bot webapp
- Full intent injection (launch any activity)
- File system access via file:// scheme
- Arbitrary file write via resolveShare JS bridge
