# Vulnerability Report: Deeplinks & Intent Injection in Max Messenger

**Package:** `ru.oneme.app`  
**Date:** 2026-05-15

---

## 1. Attack Surface: Exported Components & Deeplink Schemes

### Exported Activities
| Component | Launch Mode | Notes |
|-----------|-------------|-------|
| `one.me.android.MainActivity` | singleTask | Handles SEND/SEND_MULTIPLE with `*/*` MIME |
| `one.me.android.deeplink.LinkInterceptorActivity` | singleTop | **Primary deeplink handler** - exported, handles all deeplinks |

### Registered Deeplink Schemes
- `max://max.ru/*` (custom scheme)
- `https://max.ru/*` (verified app links with `android:autoVerify="true"`)
- `http://max.ru/*`

### Registered Internal Deeplink Paths (120+ paths)
Key security-relevant paths:
- `:auth` - Authentication flow
- `:webapp:root?bot_id=X&entry_point=Y` - Opens arbitrary WebApps
- `:settings/devices` - Device management
- `:settings/privacy/pincode` - PIN code settings
- `:external_callback` - External callback handler
- `:link-intercept` - Link interception
- `:dialogs/file-download-warning` - File download dialog

---

## 2. CRITICAL: Unvalidated URI-to-Browser Redirect (Open Redirect)

**Location:** `p000/gb9.java` → `m8366a()` (line ~469)  
**Severity:** HIGH

When a deeplink URI cannot be matched to any internal route, the app falls through to an "openBrowser" action (`x99` class) that opens the URI in an external browser via `ACTION_VIEW`:

```java
// gb9.java - fallback when no deeplink matches
ct4.m4647W0(gb9Var.f20977s, "parse deeplink openBrowser: " + uri4, new Object[0]);
x99 x99Var = new x99(uri4);  // Opens in browser
```

**In `kb9.java` (line ~107):**
```java
} else if (la9Var instanceof x99) {
    String string = ((x99) la9Var).f79378a.toString();
    Intent intent = new Intent("android.intent.action.VIEW");
    intent.setData(Uri.parse(string));
    intent.setFlags(268435456);
    context.startActivity(intent);
}
```

**Impact:** Any crafted `max://` or `https://max.ru/` deeplink that doesn't match internal routes gets opened in the browser. An attacker can craft:
```
max://max.ru/nonexistent?redirect=https://evil.com
```
The URI normalization in `yc9.m25902e()` converts `max://` to `https://` scheme, potentially allowing phishing redirects.

---

## 3. HIGH: WebApp Deeplink Allows Loading Arbitrary Bot WebViews

**Location:** `p000/ar9.java`, `one.p011me.webapp.rootscreen.WebAppRootScreen`  
**Severity:** HIGH

The deeplink path `:webapp:root` accepts `bot_id` and `entry_point` parameters:
```
max://max.ru/:webapp:root?bot_id=ATTACKER_BOT_ID&entry_point=menu
```

The WebApp loads with JavaScript enabled and exposes JavaScript interfaces:
- `WebViewHandler` → `postEvent(name, data)` 
- `PrivateWebViewHandler` → `postEvent(name, data, isPrivate=true)` (restricted to whitelisted bots)
- `AndroidPerf` → performance interface

**Attack:** An attacker can create a malicious bot, then distribute deeplinks that auto-open the bot's WebApp. The WebApp has access to `postEvent` which can trigger app actions.

**Mitigation gap:** The `PrivateWebViewHandler` is restricted (`f27771b1` flag), but the regular `WebViewHandler` is available to all bots.

---

## 4. HIGH: MyTracker Deeplink Rewriting Allows URI Manipulation

**Location:** `one.p011me.android.deeplink.LinkInterceptorActivity.onCreate()`  
**Severity:** HIGH

The `LinkInterceptorActivity` passes the deeplink URI to `MyTracker.handleDeeplink()` which can **rewrite the URI entirely**:

```java
string = MyTracker.handleDeeplink(getIntent());  // Returns potentially different URI
// ... strips mt_ params but keeps all others
Uri uri = Uri.parse(string);  // Parses the tracker-rewritten URI
// This rewritten URI is then used for all navigation
```

If MyTracker's redirect resolution follows attacker-controlled redirects, the final URI passed to the deeplink router could be arbitrary.

---

## 5. MEDIUM: Mutable PendingIntent in Media Session (Pre-Android 12)

**Location:** `p000/xfa.java` (line 460), `p000/v3l.java`  
**Severity:** MEDIUM (Android < 12 only)

```java
// xfa.java - MediaSession button receiver
pendingIntent = PendingIntent.getBroadcast(oneMeMediaSessionService, 0, intent2, 
    Build.VERSION.SDK_INT >= 31 ? 33554432 : 0);  // FLAG_MUTABLE on 31+, NO FLAGS on <31
```

**`v3l.java`:**
```java
f72713a = Build.VERSION.SDK_INT >= 31 ? 33554432 : 0;  // 0 = no immutability flag!
```

On Android < 31 (pre-Android 12), PendingIntents are created with **no flags** (implicitly mutable). A malicious app could intercept and modify these PendingIntents.

**Other PendingIntents** use `201326592` (FLAG_IMMUTABLE | FLAG_UPDATE_CURRENT) which is secure.

---

## 6. MEDIUM: `startapp` Parameter Injection in Deeplinks

**Location:** `p000/gb9.java` (line ~229)  
**Severity:** MEDIUM

```java
String queryParameter = uri4.getQueryParameter("startapp");
if (queryParameter != null) {
    int iIndexOf = queryParameter.indexOf(38); // '&'
    if (iIndexOf != -1) {
        queryParameter = queryParameter.substring(0, iIndexOf);
    }
    wc9Var = new sc9(uri4.buildUpon().clearQuery().build(), queryParameter);
}
```

The `startapp` parameter from deeplinks is passed directly to WebApp initialization. While `&` is stripped, the parameter value itself is not validated and flows into `WebAppRootScreen.onUpdateArgs()` where it triggers a WebView URL reload:

```java
// WebAppRootScreen.java
ijk.m10437H(ijkVarM17030s1, string, 2);  // Reloads with new startParam
```

---

## 7. MEDIUM: `externalCallback` Parameter Enables Callback Hijacking

**Location:** `p000/gb9.java` → `m8369d()` (line ~1008)  
**Severity:** MEDIUM

```java
public final String m8369d(Uri uri) {
    if (uri != null) {
        String queryParameter = uri.getQueryParameter("externalCallback");
        if (queryParameter != null && queryParameter.equals("1")) {
            return uri.toString();  // Full URI stored as callback
        }
    }
    return null;
}
```

When `externalCallback=1` is present in a deeplink, the **entire deeplink URI** is stored and later passed to `ExternalCallbackWidget`. This could allow an attacker to inject callback data that gets displayed or processed by the app.

---

## 8. LOW: FileProvider Exposes Entire External Storage

**Location:** `res/xml/provider_paths.xml`  
**Severity:** LOW (provider is not exported)

```xml
<paths>
    <external-path name="external_files" path="." />  <!-- ALL external storage -->
    <cache-path name="cache" path="." />              <!-- ALL cache -->
    <cache-path name="copy_media" path="copy/media/" />
</paths>
```

The FileProvider grants access to the **entire external storage** and cache. While the provider itself is not exported (`android:exported="false"`), any URI grant given to another app via intent would expose all files.

---

## 9. LOW: Implicit Broadcast Intents (Mitigated)

**Location:** `MainActivity.java`, `LocaleBottomSheet.java`, `SettingsLocaleScreen.java`  
**Severity:** LOW (mitigated with `setPackage()`)

All broadcast intents (`action.LOCALE_CHANGED`, `action.CONFIGURATION_UPDATED`) correctly set the package:
```java
intent.setPackage(context.getPackageName());
```
This prevents interception by other apps. **No vulnerability here.**

---

## 10. Fragment Injection Assessment

**Result:** NOT VULNERABLE

The app uses a custom Widget/Screen architecture (not Android's `PreferenceActivity` pattern). Deeplink routing goes through `o95` → `i95` which matches paths against a **hardcoded registry** of `m95` objects. There is no dynamic fragment class loading from intent extras.

---

## 11. File Download Path Traversal Assessment

**Result:** LOW RISK

The `FileDownloadWarningBottomSheet` accepts `chatId`, `messageId`, `fileId`, `fileName`, `fileUrl`, and `fileSize` parameters. However, file downloads appear to go through the app's internal download system rather than writing directly to user-specified paths. The download warning dialog is shown before any download occurs.

---

## Summary of Findings

| # | Severity | Finding |
|---|----------|---------|
| 2 | HIGH | Open redirect via unmatched deeplinks falling through to browser |
| 3 | HIGH | Arbitrary WebApp loading via `:webapp:root` deeplink |
| 4 | HIGH | MyTracker URI rewriting can redirect deeplink navigation |
| 5 | MEDIUM | Mutable PendingIntent on Android < 12 |
| 6 | MEDIUM | `startapp` parameter injection into WebApp |
| 7 | MEDIUM | `externalCallback` parameter stores full URI as callback |
| 8 | LOW | Overly broad FileProvider paths |
| 9 | LOW | Implicit broadcasts (mitigated) |

---

## Recommended Exploits for PoC

1. **Open Redirect:** `adb shell am start -a android.intent.action.VIEW -d "max://max.ru/../../evil.com"`
2. **WebApp Force-Open:** `adb shell am start -a android.intent.action.VIEW -d "max://max.ru/:webapp:root?bot_id=MALICIOUS_BOT&entry_point=menu"`
3. **Auth Deeplink:** `adb shell am start -a android.intent.action.VIEW -d "https://max.ru/:auth"`
