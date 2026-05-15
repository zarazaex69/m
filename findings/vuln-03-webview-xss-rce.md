# VULN-03: WebView/JavaScript Bridge Analysis — XSS-to-RCE Assessment

**App:** Max Messenger (one.me)  
**Source:** ~/max/decompiled/jadx/base/sources  
**Date:** 2025-05-15

---

## 1. WebView Instances Found

| Location | JS Enabled | File Access | JS Interface |
|----------|-----------|-------------|--------------|
| `WebAppRootScreen` (via `ahk.java`) | ✅ YES | ❌ `setAllowFileAccess(false)` | ✅ 3 interfaces |
| `FaqWebViewWidget` | ✅ YES | Not explicitly disabled | ❌ None |
| `VideoWebViewScreen` | ✅ YES | ❌ `setAllowFileAccess(false)` | ❌ None |

### Key Settings (ahk.java:74-77):
```java
v9gVar.getSettings().setJavaScriptEnabled(true);
v9gVar.getSettings().setDomStorageEnabled(true);
v9gVar.getSettings().setSupportMultipleWindows(true);
v9gVar.getSettings().setAllowFileAccess(false);
WebView.setWebContentsDebuggingEnabled(false);
```

---

## 2. addJavascriptInterface — Exposed Bridges

### 2.1 `"WebViewHandler"` → class `upk`
**File:** `p000/upk.java`

```java
@JavascriptInterface
public final void postEvent(String str, String str2) {
    this.f71476a.m10440A(str, str2, false);
}

@JavascriptInterface
public final void resolveShare(String str, byte[] bArr, String str2, String str3) {
    // Triggers share intent with attacker-controlled file data (bArr), filename (str2), mimeType (str3)
}
```

**Risk:** `resolveShare` accepts raw byte array from JS and creates a share intent with attacker-controlled filename and MIME type. Could be used to write arbitrary file content via share targets.

### 2.2 `"PrivateWebViewHandler"` → class `i6e`
**File:** `p000/i6e.java`  
**Condition:** Only added when `webAppRootScreen4.m17030s1().f27771b1` is true (private/trusted bots).

```java
@JavascriptInterface
public final void postEvent(String str, String str2) {
    this.f26580a.m10440A(str, str2, true);  // isPrivateEvent=true
}
```

**Risk:** Private events bypass the user-click timing check (3000ms window). If a bot is marked as "private/trusted," its JS code can invoke sensitive actions without user interaction.

### 2.3 `"AndroidPerf"` → class `hgk`
**File:** `p000/hgk.java`

```java
@JavascriptInterface
public final void trackFcp(long j) { /* performance tracking only */ }
```

**Risk:** LOW — Only sends performance metrics.

---

## 3. Event Actions Available via postEvent Bridge

The `postEvent` bridge (`ijk.m10440A`) dispatches events to native handlers. Events requiring user click within 3000ms (for non-trusted bots):

| Event Name | Action | Risk |
|-----------|--------|------|
| `WebAppOpenLink` | Opens arbitrary URL in external browser | Medium — phishing |
| `WebAppOpenMaxLink` | Opens internal deep link | Medium — deep link injection |
| `WebAppDownloadFile` | Downloads file from URL with specified filename | **HIGH** — arbitrary file download |
| `WebAppShare` | Triggers share dialog with content | Medium |
| `WebAppMaxShare` | Triggers Max-internal share | Medium |
| `WebAppRequestPhone` | Requests user's phone number | **HIGH** — PII exfiltration |
| `WebAppHapticFeedback*` | Haptic feedback | Low |
| `WebAppClose/Ready/SetupBackButton` | UI control | Low |

### Critical: WebAppDownloadFile
**File:** `p000/qbk.java` — Request contains: `requestId`, `url`, `fileName`  
The bot-controlled JS can specify an arbitrary URL and filename for download. No origin validation on the URL.

---

## 4. shouldOverrideUrlLoading — URL Scheme Handling

### 4.1 WebAppRootScreen (ppk.java:721)
```java
public boolean mo6702y(Uri uri) {
    if (scheme == "http" || scheme == "https") return false;  // allow navigation
    // For ALL other schemes: launch via ACTION_VIEW intent
    r16Var.f58701a.startActivity(new Intent("android.intent.action.VIEW", uri));
    return true;
}
```

**VULNERABILITY:** Any non-http(s) scheme from WebView content triggers `startActivity(ACTION_VIEW, uri)`. A malicious bot page could use:
- `intent://` scheme for intent injection
- `file://` scheme (though file access is disabled)
- Custom app schemes for cross-app exploitation

### 4.2 FaqWebViewWidget (nk6.java)
- Handles `mailto:` scheme
- Handles `max:` deep link scheme → calls `FaqWebViewWidget.m17037c1`
- Downloads files with specific MIME types

**Risk:** The `max:` scheme handler could be abused if deep link parsing has vulnerabilities.

---

## 5. file:// URI Loading

- `setAllowFileAccess(false)` is set in both `WebAppRootScreen` and `VideoWebViewScreen`
- `setAllowFileAccessFromFileURLs(true)` — **NOT FOUND** anywhere
- `setAllowUniversalAccessFromFileURLs(true)` — **NOT FOUND** anywhere
- `FaqWebViewWidget` does NOT explicitly disable file access (default is `true` on API < 30)

**Finding:** `FaqWebViewWidget` may have file access enabled by default on older Android versions.

---

## 6. Attacker-Controlled Content in WebViews

### 6.1 WebAppRootScreen — Bot Web Apps
- Loads URLs from bot configuration (`nfj.f43545a`)
- Bot developers control the web content entirely
- The JS bridge gives bot pages access to native functionality
- **This is the primary attack surface** — a malicious bot can execute JS bridge calls

### 6.2 VideoWebViewScreen
- Loads `videoUrl` from chat message attachments (`chat.media.viewer.attach_url`)
- URL comes from message data — **attacker-controlled** if sender is malicious
- JS is enabled but no JS interfaces are exposed

### 6.3 FaqWebViewWidget
- Loads URL from app config (`pzf.f55047J1`) — server-controlled, not user-controlled
- Lower risk

---

## 7. SSL Error Handling

**File:** `xyc.java:108-127`
```java
public void onReceivedSslError(WebView webView, SslErrorHandler sslErrorHandler, SslError sslError) {
    if (!((opk) this.f81673b).mo6696i()) {
        sslErrorHandler.cancel();  // Good: rejects by default
    } else {
        sslErrorHandler.proceed();  // BAD: bypasses SSL validation
    }
}
```

**VULNERABILITY:** `mo6696i()` calls `ri9.m19924S()` which reads a remote config flag. If this flag is enabled (e.g., for debugging), SSL errors are silently accepted, enabling MITM attacks on WebView traffic.

---

## 8. postMessage Bridge

No `WebMessagePort` or `postMessage` HTML5 bridge was found. Communication is exclusively through `addJavascriptInterface` and `evaluateJavascript` calls.

The app sends events TO the WebView via:
```java
evaluateJavascript("WebApp.sendEvent(%s, %s)")        // public events
evaluateJavascript("PrivateWebApp.sendEvent(%s, %s)") // private events
```

---

## 9. Summary of Vulnerabilities

| # | Finding | Severity | XSS-to-RCE Potential |
|---|---------|----------|---------------------|
| 1 | `resolveShare` accepts raw bytes from JS → file write via share | **HIGH** | Partial — write arbitrary files via share targets |
| 2 | `WebAppDownloadFile` — arbitrary URL/filename download | **HIGH** | Could overwrite app files if download path is predictable |
| 3 | Intent scheme injection via `shouldOverrideUrlLoading` | **HIGH** | `intent://` URIs can launch arbitrary activities with extras |
| 4 | SSL bypass via remote config flag | **MEDIUM** | Enables MITM → inject JS → trigger bridge calls |
| 5 | `WebAppRequestPhone` — phone number exfiltration | **MEDIUM** | PII leak, not RCE |
| 6 | Private bridge bypass for trusted bots (no click check) | **MEDIUM** | Amplifies other vulns |
| 7 | `FaqWebViewWidget` missing explicit `setAllowFileAccess(false)` | **LOW** | Only on API < 30 |

---

## 10. XSS-to-RCE Assessment

**Direct RCE: NOT achievable** through the current JS bridge alone. The exposed methods do not provide:
- ❌ `Runtime.exec()` or command execution
- ❌ Direct file system read/write
- ❌ Native code loading
- ❌ Reflection-based method invocation

**Indirect code execution paths:**
1. **Intent injection** via `shouldOverrideUrlLoading` accepting `intent://` URIs → could launch unexported activities or pass crafted extras
2. **File overwrite** via `resolveShare` byte array → if a share target writes to a predictable path that the app later loads (e.g., shared preferences, dex files)
3. **MITM + SSL bypass** → inject malicious JS into any WebView page → chain with bridge calls

**Recommended exploitation chain:**
```
Malicious bot page → JS bridge postEvent("WebAppOpenLink") 
  → intent:// URI with crafted extras → launch unexported activity → escalate
```
