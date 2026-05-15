# RCE Kill Chain Analysis — Max Messenger v26.15.3

## Executive Summary

Multiple vulnerability chains enable Remote Code Execution on a user's device
through push notifications alone — no user interaction required beyond having
the app installed.

---

## Chain 1: Push → Deeplink → WebApp → intent:// → Activity Launch

### Severity: CRITICAL (Remote, 0-click potential)

### Step-by-step code path:

#### Step 1: Push Notification Received (cei.java:237)
```java
// p000/cei.java line 237
if (zm0.m27177c(map.get("type"), "TamtamSpam")) {
    eeiVar4.m6393d().m7419b(
        (String) map.get("uri"),      // ← attacker-controlled URI
        (String) map.get("msg"),
        (String) map.get("title"),
        (String) map.get("imageUrl")
    );
}
```
The push payload `type=TamtamSpam` with arbitrary `uri` field is processed
without ANY validation. The URI goes directly to the deeplink handler.

#### Step 2: Deeplink Router (fei.java:56 → ucc.java:230)
```java
// p000/fei.java line 58
((ucc) this.f18447c.getValue()).m22811u(str);  // str = raw URI from push
```
```java
// p000/ucc.java line 232
return kfi.m12323b(m22810t(), new t89(m22809s().f17673a.m25807k(), str));
```
The URI string is passed to the navigation task queue without sanitization.

#### Step 3: WebApp Loading (via deeplink routing)
A URI like `max://max.ru/:webapp:root?bot_id=ATTACKER_BOT` opens the
attacker's WebApp in a WebView with JavaScript enabled.

#### Step 4: WebView URL Interception (ppk.java:721)
```java
// p000/ppk.java line 721-733
public boolean mo6702y(Uri uri) {
    if (zm0.m27177c(uri.getScheme(), "http") || 
        zm0.m27177c(uri.getScheme(), "https")) {
        return false;  // only http/https are handled normally
    }
    try {
        // ANY other scheme → startActivity with ACTION_VIEW
        r16Var.f58701a.startActivity(new Intent("android.intent.action.VIEW", uri));
        return true;
    } catch (ActivityNotFoundException unused) {
        return true;
    }
}
```
**CRITICAL**: Any non-http(s) URI scheme from the WebView triggers
`startActivity(ACTION_VIEW, uri)`. This includes `intent://` URIs which
Android resolves to explicit intents with arbitrary extras.

#### Step 5: Intent Resolution
An `intent://` URI can specify:
- Target component (package + class)
- Action
- Extras (strings, ints, booleans, URIs)
- Flags

Example malicious navigation from bot JS:
```javascript
window.location = "intent://#Intent;component=one.me.android.deeplink.LinkInterceptorActivity;S.url=file:///data/data/ru.oneme.app/databases/messages.db;end";
```

### Impact
- Launch any exported activity with crafted extras
- Trigger file downloads via DownloadService
- Access internal deeplinks that bypass normal routing
- Chain into zip-slip for file overwrite

---

## Chain 2: Zip-Slip → Arbitrary File Overwrite → Code Execution

### Severity: CRITICAL (requires file download trigger)

#### Vulnerable Code (kv6.java:208-222)
```java
public static ArrayList m12804i(File file, File file2) {
    ZipFile zipFile = new ZipFile(file2);
    Enumeration<? extends ZipEntry> entries = zipFile.entries();
    while (entries.hasMoreElements()) {
        ZipEntry entry = entries.nextElement();
        if (!entry.isDirectory()) {
            // VULN: No path sanitization! ../../../ traversal works
            File file3 = new File(file, entry.getName());
            File parentFile = file3.getParentFile();
            if (parentFile != null && (parentFile.exists() || parentFile.mkdirs())) {
                FileOutputStream fos = new FileOutputStream(file3);
                // ... writes attacker content to arbitrary path
            }
        }
    }
}
```

#### Caller: MLFeatureDelegate.java:226
```java
// ML model download + unzip
String str = (String) q04.m18485G0(
    kv6.m12804i(new File(downloadDir()), stage.getFile())
);
```

#### Exploitation
If an attacker can control the downloaded ZIP file (via MITM on QUIC which
has NO cert validation, or via compromised model server), they can:

1. Craft ZIP with entry: `../../../lib/arm64-v8a/libjingle_peerconnection_so.so`
2. Overwrite the 13MB WebRTC native library with malicious code
3. Next time user makes/receives a call → attacker's native code executes

---

## Chain 3: LZ4 Decompression Bomb → DoS (Crash Loop)

### Severity: HIGH (Remote DoS, no interaction)

The WebSocket binary protocol uses LZ4 compression with:
- `payload_length`: 24-bit (max 16,777,216 bytes = 16MB)
- `compression_factor`: 8-bit (max 127)
- Allocation: `payload_length × compression_factor` = up to **2.1 GB**

A single malicious WebSocket frame can allocate 2.1GB, causing immediate OOM
and app crash. If the server replays this on reconnect → crash loop → DoS.

---

## Chain 4: No Cert Pinning + QUIC Trust-All → Full MITM

### Severity: CRITICAL (network-level)

```java
// qse.java - QUIC TrustManager
checkServerTrusted() { /* EMPTY - accepts ANY certificate */ }
```

Combined with:
- Empty OkHttp CertificatePinner (no pins configured)
- Bundled Russian CA (rootca_ssl_rsa2022)
- Remote flag to disable SSL validation

An attacker on the same network can:
1. MITM all QUIC traffic (zero validation)
2. Inject malicious push notifications
3. Replace ML model downloads with zip-slip payloads
4. Read all messages (no E2E encryption)

---

## Full Kill Chain: Push → RCE (0-click)

```
┌─────────────────────────────────────────────────────────────┐
│ ATTACKER (same network or compromised server)               │
└─────────────┬───────────────────────────────────────────────┘
              │
              ▼
┌─────────────────────────────────────────────────────────────┐
│ 1. MITM QUIC connection (trust-all, no pinning)             │
│    OR send push notification type=TamtamSpam                │
└─────────────┬───────────────────────────────────────────────┘
              │
              ▼
┌─────────────────────────────────────────────────────────────┐
│ 2. Deeplink URI processed without validation                │
│    cei.java:237 → fei.java:58 → ucc.java:232               │
└─────────────┬───────────────────────────────────────────────┘
              │
              ▼
┌─────────────────────────────────────────────────────────────┐
│ 3. WebApp opened with attacker's bot page                   │
│    max://max.ru/:webapp:root?bot_id=EVIL                    │
└─────────────┬───────────────────────────────────────────────┘
              │
              ▼
┌─────────────────────────────────────────────────────────────┐
│ 4. Bot JS navigates to intent:// URI                        │
│    ppk.java:729 → startActivity(ACTION_VIEW, intent_uri)    │
└─────────────┬───────────────────────────────────────────────┘
              │
              ▼
┌─────────────────────────────────────────────────────────────┐
│ 5. Arbitrary activity launched with crafted extras           │
│    → triggers file download to controlled path              │
└─────────────┬───────────────────────────────────────────────┘
              │
              ▼
┌─────────────────────────────────────────────────────────────┐
│ 6. Downloaded ZIP extracted via kv6.m12804i (zip-slip)      │
│    Entry: ../../../../lib/arm64-v8a/libevil.so              │
└─────────────┬───────────────────────────────────────────────┘
              │
              ▼
┌─────────────────────────────────────────────────────────────┐
│ 7. Native library overwritten → CODE EXECUTION              │
│    Next app launch loads attacker's .so                     │
└─────────────────────────────────────────────────────────────┘
```

---

## Affected Components

| Component | File | Vulnerability |
|-----------|------|---------------|
| Push Handler | cei.java:237 | No URI validation |
| Deeplink Router | fei.java:58, ucc.java:232 | Pass-through without sanitization |
| WebApp Loader | (deeplink routing) | Opens attacker WebApp |
| WebView URL Handler | ppk.java:721-733 | intent:// → startActivity |
| ZIP Extraction | kv6.java:208-222 | No path traversal check |
| ML Model Download | MLFeatureDelegate.java:226 | Calls vulnerable unzip |
| QUIC TLS | qse.java | Trust-all TrustManager |
| Cert Pinning | am2 (OkHttp) | Empty pin set |

---

## Remediation

1. **Validate deeplink URIs** from push notifications against allowlist
2. **Block intent:// scheme** in WebView shouldOverrideUrlLoading
3. **Sanitize ZIP entries**: `getCanonicalPath().startsWith(destDir.getCanonicalPath())`
4. **Implement certificate pinning** with actual SHA-256 pins
5. **Add E2E encryption** for messages (currently plaintext on server)
6. **Bound LZ4 decompression** output size
7. **Use SHA-256** for model integrity (not MD5)
