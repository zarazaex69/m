# VULN-64: WebView Deep Configuration Analysis

## Summary

Three distinct WebView instances found in Max messenger. The **WebApp (Bot/MiniApp) WebView** is the weakest due to JavaScript bridges exposing sensitive native functionality, SSL bypass capability, and bot-controlled URL loading.

---

## 1. All WebView Instances Found

### A. WebApp WebView (WebAppRootScreen → v9g/vyc)
- **Location**: `p000/ahk.java` (configuration), `one/me/webapp/rootscreen/WebAppRootScreen.java`
- **Type**: `v9g` extends `vyc` extends `android.webkit.WebView`

### B. FAQ WebView (FaqWebViewWidget → vyc)
- **Location**: `one/me/webview/FaqWebViewWidget.java`
- **Type**: `vyc` (OneMeWebView)

### C. Video WebView (VideoWebViewScreen → vyc)
- **Location**: `one/me/chatmedia/viewer/VideoWebViewScreen.java`
- **Type**: `vyc` (OneMeWebView)

---

## 2. Configuration Comparison

| Setting | WebApp (ahk.java) | FAQ WebView | Video WebView |
|---------|-------------------|-------------|---------------|
| setJavaScriptEnabled | **true** | **true** | **true** |
| setDomStorageEnabled | **true** | **true** | **true** |
| setAllowFileAccess | false | **NOT SET** (default=true on API<30) | false |
| setAllowContentAccess | NOT SET | NOT SET | **true** |
| setAllowFileAccessFromFileURLs | NOT SET | NOT SET | NOT SET |
| setAllowUniversalAccessFromFileURLs | NOT SET | NOT SET | NOT SET |
| setMixedContentMode | NOT SET | NOT SET | 0 (MIXED_CONTENT_ALWAYS_ALLOW) |
| setSupportMultipleWindows | **true** | NOT SET | NOT SET |
| setMediaPlaybackRequiresUserGesture | NOT SET | NOT SET | **false** |
| WebContentsDebuggingEnabled | false | false | false |

---

## 3. WEAKEST WebView: WebApp (Bot/MiniApp) WebView

**Risk: HIGH** — Most permissive configuration with native bridges.

### Why it's the weakest:

1. **Three JavaScript-to-Native Bridges**:
   - `WebViewHandler` → `upk.java` — exposes `postEvent(name, data)` and `resolveShare(str, bytes, str2, str3)`
   - `AndroidPerf` → `hgk.java` — exposes `trackFcp(long)`
   - `PrivateWebViewHandler` → `i6e.java` — exposes `postEvent(name, data, isPrivate=true)` (conditionally added for privileged bots)

2. **Extensive Bridge Event Surface** (via postEvent → ijk.m10440A):
   - `open_link` / `open_max_link` — open arbitrary URLs
   - `biometry_get_info` / `biometry_request_access` / `biometry_request_auth` / `biometry_update_token` — biometric access
   - `secure_storage_save_key` / `secure_storage_get_key` / `secure_storage_clear` — secure storage access
   - `device_storage_save_key` / `device_storage_get_key` / `device_storage_clear` — device storage
   - `nfc_get_info` / `nfc_emulate_nfc_tag` — NFC hardware access
   - `request_phone` — request user's phone number
   - `download_file` — trigger file downloads
   - `setup_screen_capture_behavior` — control screen capture
   - `verify_mobile_id` — mobile ID verification
   - `change_screen_brightness` — hardware control

3. **SSL Error Bypass (Conditional)**:
   - `xyc.onReceivedSslError()` calls `opk.mo6696i()` → `ri9.m19924S()`
   - Controlled by remote config flag `"web_app:ssl_check"`
   - If flag is true: `sslErrorHandler.proceed()` — **bypasses SSL validation entirely**

4. **Bot-Controlled URL Loading**:
   - URL loaded via `fhk.java` from `nfj.f43545a` (UrlState)
   - URL originates from bot entry point data (`ojkVar.f47472d`)
   - Bot developers control what URL is loaded in the WebView

5. **setSupportMultipleWindows(true)** with `onCreateWindow` creating new WebViews with no additional restrictions

---

## 4. User-Controlled URL Loading

| WebView | URL Source | User-Controlled? |
|---------|-----------|-----------------|
| WebApp | Bot-provided URL via entry point | **Yes** — bot developer controls URL |
| FAQ | Hardcoded resource string (`pzf.f55047J1`) with query params | No — app-controlled |
| Video | Video URL from chat message (`videoUrl` parameter) | **Partially** — from message content |

---

## 5. WebView Debugging

All three WebViews explicitly set `WebView.setWebContentsDebuggingEnabled(false)`. **No debugging enabled in release.**

---

## 6. WebView-to-Native Bridges

### WebViewHandler (upk.java) — All WebApp bots
```java
@JavascriptInterface
public void postEvent(String name, String data)  // Routes to ijk.m10440A → event dispatcher

@JavascriptInterface
public void resolveShare(String str, byte[] data, String str2, String str3)  // Share resolution
```

### PrivateWebViewHandler (i6e.java) — Privileged bots only
```java
@JavascriptInterface
public void postEvent(String name, String data)  // Same as above but with isPrivate=true flag
```
- Gated by `ijk.f27771b1` which checks if botId is in `webAppGestureExceptions` remote config list

### AndroidPerf (hgk.java) — All WebApp bots
```java
@JavascriptInterface
public void trackFcp(long timestamp)  // Performance tracking
```

### Security Controls on Bridge:
- User-click timing check: events in `f27730L1` set require user interaction within 3000ms
- Private events rejected if bot not in allowlist
- But `postEvent` itself has no input validation on event name/data

---

## 7. File URI Access

| WebView | file:// Access |
|---------|---------------|
| WebApp | `setAllowFileAccess(false)` — **blocked** |
| FAQ | **NOT explicitly set** — defaults to `true` on API < 30 |
| Video | `setAllowFileAccess(false)` — blocked, but `setAllowContentAccess(true)` allows content:// |

**FAQ WebView is vulnerable to file:// access on older Android versions** (API < 30) since `setAllowFileAccess` is never called.

---

## 8. Key Vulnerabilities

### Critical: WebApp SSL Bypass via Remote Config
- **File**: `p000/ri9.java:171`, `p000/xyc.java:119`
- Remote flag `web_app:ssl_check` can enable `sslErrorHandler.proceed()`
- Enables MITM attacks against WebApp content if flag is toggled server-side

### High: Extensive JS Bridge Attack Surface
- **File**: `p000/ahk.java:102-105`
- Bot WebApps can invoke biometry, NFC, secure storage, phone number access
- A compromised/malicious bot URL could abuse these capabilities

### Medium: FAQ WebView Missing File Access Restriction
- **File**: `one/me/webview/FaqWebViewWidget.java:151-156`
- No `setAllowFileAccess(false)` call
- On Android < 10 (API 29), file:// URIs accessible by default

### Medium: Video WebView Mixed Content
- **File**: `one/me/chatmedia/viewer/VideoWebViewScreen.java:293`
- `setMixedContentMode(0)` = MIXED_CONTENT_ALWAYS_ALLOW
- HTTP resources loaded alongside HTTPS without warning
