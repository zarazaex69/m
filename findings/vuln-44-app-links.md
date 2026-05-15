# VULN-44: App Links / Universal Links — Open Redirect & Phishing Analysis

## Summary

Max messenger (ru.oneme.app) registers verified App Links for `max.ru` with `autoVerify=true`. The deeplink handling pipeline includes MyTracker URI rewriting that introduces an **open redirect vulnerability** via the `mt_deeplink` parameter. The whitelist-based link warning system can be bypassed for domains on the server-controlled whitelist.

## 1. Verified App Links (AndroidManifest.xml)

```xml
<activity android:name="one.me.android.deeplink.LinkInterceptorActivity"
          android:exported="true"
          android:launchMode="singleTop">
    
    <!-- Verified App Link (autoVerify=true) -->
    <intent-filter android:autoVerify="true" android:label="max">
        <action android:name="android.intent.action.VIEW"/>
        <category android:name="android.intent.category.DEFAULT"/>
        <category android:name="android.intent.category.BROWSABLE"/>
        <data android:pathPattern="/..*"/>
        <data android:scheme="http"/>
        <data android:scheme="https"/>       <!-- @string/web_scheme -->
        <data android:host="max.ru"/>        <!-- @string/app_host -->
    </intent-filter>
    
    <!-- Custom scheme (no autoVerify) -->
    <intent-filter android:label="max">
        <action android:name="android.intent.action.VIEW"/>
        <category android:name="android.intent.category.DEFAULT"/>
        <category android:name="android.intent.category.BROWSABLE"/>
        <data android:scheme="max"/>         <!-- @string/app_scheme -->
        <data android:host="max.ru"/>        <!-- @string/app_host -->
    </intent-filter>
</activity>
```

**Resolved string values:**
- `app_host` = `max.ru`
- `app_scheme` = `max`
- `web_scheme` = `https`

## 2. URL Patterns Handled

| Scheme | Host | Path | Purpose |
|--------|------|------|---------|
| `https` | `max.ru` | `/..*` (any path) | Verified App Link |
| `http` | `max.ru` | `/..*` (any path) | Verified App Link |
| `max` | `max.ru` | any | Custom scheme deeplink |

**Additional schemes queried (not handled directly):**
- `yandexmaps://` — map intent queries
- `yandexnavi://` — navigation intent queries
- `dgis://` — 2GIS maps
- `petalmaps://` — Huawei maps

## 3. Open Redirect via MyTracker `mt_deeplink` Parameter

### Vulnerability

**Class:** `one.me.android.deeplink.LinkInterceptorActivity`  
**Flow:** `onCreate()` → `MyTracker.handleDeeplink(intent)` → `C0212k.m4430a(intent)`

The `LinkInterceptorActivity` processes incoming deeplinks through MyTracker's `handleDeeplink()` method. This method extracts the `mt_deeplink` query parameter and **returns it as the new URI to navigate to**:

```java
// C0212k.m4430a (DeeplinkHandler)
public String m4430a(Intent intent) {
    Uri data = intent.getData();
    String queryParameter = data.getQueryParameter("mt_deeplink");
    if (queryParameter != null) {
        // Returns mt_deeplink value as the new deeplink URL!
        return queryParameter;
    }
    // Falls back to original URI
    return data.toString();
}
```

After MyTracker returns the rewritten URL, `LinkInterceptorActivity` strips `mt_` prefixed query params and passes the result to `LinkInterceptorWidget` → `gb9.m8371f()` for routing.

### Exploit Scenario

An attacker can craft a URL like:
```
https://max.ru/anything?mt_deeplink=https://evil.com/phishing&mt_click_id=abc123
```

**What happens:**
1. Android opens `LinkInterceptorActivity` (verified app link for max.ru)
2. `MyTracker.handleDeeplink()` extracts `mt_deeplink=https://evil.com/phishing`
3. The `mt_` params are stripped from the returned URL
4. The app attempts to route `https://evil.com/phishing`
5. Since the host is NOT `max.ru`, the deeplink parser falls through to `openBrowser` action (`x99` class)
6. The app opens `https://evil.com/phishing` in browser/CustomTab

**Impact:** HIGH — A legitimate-looking `max.ru` link redirects the user to an attacker-controlled site. This bypasses Android's App Link verification since the initial URL is genuinely on `max.ru`.

### Post-MyTracker Cleanup (Partial Mitigation)

The app strips `mt_` prefixed query parameters from the rewritten URL:
```java
for (String str5 : queryParameterNames) {
    if (!w0i.m24048k0(str5, "mt_", false)) {
        builderBuildUpon.appendQueryParameter(str5, uri.getQueryParameter(str5));
    }
}
```
This only removes tracking params from the *rewritten* URL, not the redirect itself.

## 4. White-List-Links (PmsKey)

**PmsKey:** `white-list-links` (field `f88892whitelistlinks`, ordinal 209)

**Usage in `v7b.java`:**
```java
lowerCase = new URL(str).getHost().toLowerCase(Locale.ROOT);
listM20177t = rtdVar.m20177t(PmsKey.f88892whitelistlinks, p56.f52304a);
// Default: empty list (p56.f52304a = immutable empty list)

for (String str3 : listM20177t) {
    if (!zm0.m27177c(str3, lowerCase)) {       // exact match
        if (lowerCase.endsWith("." + str3)) {  // subdomain match
        }
    }
    z = true;  // whitelisted!
}
```

**Behavior:**
- The whitelist is **server-controlled** (fetched via PMS/remote config)
- Default value is an **empty list** (no domains whitelisted by default)
- Matching logic: exact domain match OR subdomain match (`.example.com`)
- If a link's host is on the whitelist, **no warning is shown**

**Security implication:** If an attacker compromises the PMS config or if the whitelist is overly broad, link warnings can be silently bypassed.

## 5. Show-Warning-Links (PmsKey)

**PmsKey:** `show-warning-links` (field `f88852showwarninglinks`, ordinal 182)

**Usage:**
- `qp6.java`: Creates a `no6` boolean config wrapper for this key
- `C0293e.java`: `spl.m21400a(abstractC1363z5, "show-warning-links", PmsKey.f88852showwarninglinks, false)`
  - Default value: `false` (warnings disabled by default!)

**Warning mechanism (`vah` class):**
```java
// vah = "ShowWarningLinkBottomSheet"
public final class vah implements y3b {
    // toString: "ShowWarningLinkBottomSheet(link=...)"
}
```

When `show-warning-links` is enabled AND a link is NOT on the whitelist, the app shows a `ShowWarningLinkBottomSheet` dialog warning the user about the external link.

**Key finding:** The warning feature defaults to `false` — it must be explicitly enabled server-side. Users may not see any warning for external links.

## 6. Bypassing Link Warnings

**Bypass methods:**

1. **Whitelist bypass:** Any domain on the `white-list-links` PMS config skips warnings entirely. The matching includes subdomain wildcarding (`lowerCase.endsWith("." + str3)`).

2. **Feature flag disabled:** `show-warning-links` defaults to `false`. If the server hasn't enabled it, no warnings are shown for ANY external link.

3. **URL matching logic (`p8h.m17774a`):** The URL comparison uses regex replacement:
   ```java
   str.equals(((ckf) this.f52648b.getValue()).f8866a.matcher(
       o0i.m15482W0(str2).toString()).replaceFirst(""));
   ```
   This strips some URL component (likely protocol/host prefix) before comparison. If the regex is imprecise, crafted URLs could bypass matching.

4. **mt_deeplink redirect:** The MyTracker rewriting happens BEFORE the whitelist check, so the redirected URL is what gets checked — but if it opens in browser directly (non-max.ru host), the warning logic in `v7b.java` may not apply.

## 7. Market/Play Store Links (Phishing Vector)

**Found in code:**

```java
// p000/xt7.java - Google Play Services check
Uri.parse("market://details").buildUpon()
    .appendQueryParameter("id", "com.google.android.gms");

// p000/C0803pc.java - App store redirect
"https://play.google.com/store/apps/details?id=ru.oneme.app"
```

**Assessment:** The app uses `market://` links only for Google Play Services installation prompts and self-referencing Play Store links. These are hardcoded and not user-controllable. **Low phishing risk** from these specific usages.

However, the `mt_deeplink` open redirect could be used to redirect to a fake Play Store page:
```
https://max.ru/update?mt_deeplink=https://evil.com/fake-play-store
```

## 8. MyTracker URI Rewriting (Deeplink Flow)

### Complete Flow

```
1. User clicks: https://max.ru/chat?mt_deeplink=<PAYLOAD>&mt_click_id=xyz
2. Android → LinkInterceptorActivity.onCreate()
3. MyTracker.handleDeeplink(intent)
   → C0191b.m4297a(intent)
   → C0212k.m4430a(intent)
     - Extracts mt_deeplink query param
     - Returns PAYLOAD as new URL string
4. LinkInterceptorActivity strips mt_ params from returned URL
5. Result passed to LinkInterceptorWidget(Uri.parse(string))
6. gb9.m8371f(uri) routes the deeplink:
   - If host == "max.ru": internal navigation (chat, profile, etc.)
   - If host != "max.ru": x99(uri) → opens in browser (OPEN REDIRECT)
   - Special paths: :auth, :share-self-out, :folder, stickerset, join, joincall
```

### MyTracker Attribution Handler

```java
// AttributionHandler.handleReferrerAttribution
String queryParameter = Uri.parse(
    "https://tracker-api.vk-analytics.ru/?" + URLDecoder.decode(str, "UTF-8")
).getQueryParameter("mt_deeplink");
```

The attribution system also processes `mt_deeplink` from install referrers, connecting to `tracker-api.vk-analytics.ru`.

## Risk Assessment

| Finding | Severity | CVSS Est. |
|---------|----------|-----------|
| Open redirect via mt_deeplink | HIGH | 7.4 |
| Link warnings disabled by default | MEDIUM | 5.3 |
| Server-controlled whitelist bypass | MEDIUM | 4.3 |
| Subdomain wildcard in whitelist matching | LOW | 3.1 |

## Proof of Concept

### Open Redirect
```
adb shell am start -a android.intent.action.VIEW \
  -d "https://max.ru/chat?mt_deeplink=https%3A%2F%2Fattacker.com%2Fphishing" \
  -n ru.oneme.app/one.me.android.deeplink.LinkInterceptorActivity
```

### Custom Scheme Redirect
```
adb shell am start -a android.intent.action.VIEW \
  -d "max://max.ru/x?mt_deeplink=https%3A%2F%2Fattacker.com%2Ffake-login"
```

## Recommendations

1. **Validate mt_deeplink target:** Only allow redirects to `max.ru` or whitelisted domains after MyTracker rewriting
2. **Enable link warnings by default:** Change `show-warning-links` default to `true`
3. **Restrict mt_deeplink scheme:** Only allow `max://` or `https://max.ru` as valid mt_deeplink values
4. **Add user confirmation:** Show an interstitial before redirecting to non-max.ru domains from deeplinks
5. **Pin the whitelist:** Don't rely solely on server-controlled config for security-critical domain lists
