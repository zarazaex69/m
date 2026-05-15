# VULN-99: Miscellaneous Vulnerabilities

## 1. Regex DoS (ReDoS)

### 1a. URL Pattern Matching — Critical ReDoS in `t9d.java`

**File:** `p000/t9d.java`

The URL detection regex is extremely complex with nested groups and quantifiers applied to user-controlled message text:

```java
// Nested repetition: (?:[charset]{0,61}[charset]){0,1}\.)+  applied to user input
Pattern.compile("(((?:(?i:http|https|rtsp)://...)?(?:" + domainPattern + 
    ")(?:\\:\\d{1,5})?)([/\\?](?:(?:[large_charset];/\\?:@&=#~...)])|(?:%[a-fA-F0-9]{2}))*)?(?:\\b|$|^))")
```

**Impact:** A crafted message containing a near-match URL (e.g., many dots followed by almost-valid TLD characters) forces the regex engine into exponential backtracking. Since this runs on every message displayed, a single malicious message can freeze the UI thread.

**PoC concept:** Send a message like `http://aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.` followed by characters that almost match the TLD list but don't.

### 1b. Dynamic Regex from User Input — `BadEndpointException.java`

**File:** `ru/p027ok/android/webrtc/signaling/transport/exception/BadEndpointException.java`

```java
Pattern.compile("(?<=[?&])(" + q04.m18490L0(setM13758T0, "|", ...) + ")=[^&]*");
```

The set is hardcoded (`token|auth_data|credential|auth_token|session_data`), so this specific instance is safe. However, the pattern demonstrates a code style where regex is built from concatenation — if any future parameter name contains regex metacharacters, it becomes injectable.

### 1c. SDP Parsing Regex — `zzk.java`

```java
Pattern.compile("^a=rtpmap:(\\d+) ([a-zA-Z0-9-]+)(/\\d+)+[\\r]?$");
```

The `(/\\d+)+` with nested repetition on SDP content received from remote peers could cause backtracking on malformed SDP payloads like `a=rtpmap:96 opus/////////...` (many slashes without digits).

---

## 2. XML Parsing — Limited XXE Surface

**Files:** `XmlPullParser` usage in drawable inflation (`SavedGroupCallIconDrawable.java`, `SavedMessagesIconDrawable.java`, `BlockedGhostAvatarDrawable.java`)

**Finding:** XML parsing uses Android's `XmlPullParser` exclusively for resource inflation (drawables from APK resources). No `DocumentBuilderFactory`, `SAXParserFactory`, or `TransformerFactory` usage was found processing external/network data.

**Risk:** LOW — Android's built-in `XmlPullParser` for resource inflation does not support external entities. No XXE vector identified in current code.

---

## 3. Zip Bomb — ML Model Extraction

**File:** `p000/kv6.java` (method `m12804i`)

```java
public static ArrayList m12804i(File file, File file2) {
    ZipFile zipFile = new ZipFile(file2);
    Enumeration<? extends ZipEntry> entries = zipFile.entries();
    while (entries.hasMoreElements()) {
        ZipEntry zipEntry = entries.nextElement();
        if (!zipEntry.isDirectory()) {
            File file3 = new File(file, zipEntry.getName());
            FileOutputStream fos = new FileOutputStream(file3);
            InputStream is = zipFile.getInputStream(zipEntry);
            m12803h(is, fos);  // copies all bytes, no size limit
        }
    }
}
```

**Called from:** `MLFeatureDelegate.unzipModel()` — downloads ML models from server and extracts them.

**Vulnerabilities:**
1. **No decompressed size check** — A zip bomb (e.g., 42.zip: 42KB compressed → 4.5PB decompressed) will fill disk until the device runs out of storage.
2. **No entry count limit** — Millions of tiny files could exhaust inodes.
3. **Path traversal** — `zipEntry.getName()` is used directly in `new File(file, zipEntry.getName())`. A crafted entry name like `../../data/data/one.me/shared_prefs/evil.xml` could write outside the intended directory.

**Impact:** If the ML model download server is compromised or MITM'd, attacker can fill device storage or overwrite app files.

---

## 4. Clipboard Data Injection

**File:** `p000/tzg.java` (CopyToClipboard), `p000/hgl.java` (clipboard paste handling)

```java
// CopyToClipboard stores raw text with no sanitization
public tzg(String str, xji xjiVar) {
    this.f68994a = str;  // textToCopy — raw message text
}
```

**Vulnerability:** When a user copies a message, the raw text is placed on the clipboard without filtering invisible Unicode characters:

- **U+202E** (Right-to-Left Override): `Transfer 1000$ to account ‮1234‬5678` displays as "Transfer 1000$ to account 8765 1234" when pasted
- **U+200B** (Zero-Width Space): Makes strings appear identical but differ programmatically
- **U+2060** (Word Joiner): Invisible characters that break copy-paste into code editors
- **U+FEFF** (BOM): Can corrupt file headers when pasted

**Impact:** An attacker sends a message containing invisible Unicode. When the victim copies and pastes it (e.g., into a banking app, terminal, or code editor), the pasted text has different semantic meaning than what was visually displayed.

---

## 5. Deep Link Parameter Pollution

**File:** `one/p011me/android/deeplink/LinkInterceptorActivity.java` (line 131)

```java
for (String str5 : queryParameterNames) {
    if (!w0i.m24048k0(str5, "mt_", false)) {
        builderBuildUpon.appendQueryParameter(str5, uri.getQueryParameter(str5));
    }
}
```

**File:** `one/p011me/android/deeplink/LinkInterceptorWidget.java` (line 164)

```java
Iterator<T> it = uri.getQueryParameters(str).iterator();
while (it.hasNext()) {
    builderClearQuery.appendQueryParameter(str, (String) it.next());
}
```

**Vulnerability:** Inconsistent parameter handling between the two deep link processors:

1. `LinkInterceptorActivity` uses `getQueryParameter(str5)` — returns **first** value only
2. `LinkInterceptorWidget` uses `getQueryParameters(str)` — iterates **all** values

For a URL like `oneme://chat?id=legitimate&id=attacker`:
- Activity path: uses `id=legitimate` (first wins)
- Widget path: forwards both values, downstream consumer picks one

**Impact:** Parameter pollution can bypass validation. If security checks use the first value but the action handler uses the last (or vice versa), an attacker can craft a deep link that passes validation but executes with different parameters. The `webappChatId` parameter is specifically filtered, suggesting awareness of this issue for that parameter, but other parameters remain vulnerable.

---

## Summary

| # | Vulnerability | Severity | Exploitability |
|---|---|---|---|
| 1a | ReDoS in URL pattern matching | High | Easy — send crafted message |
| 1c | ReDoS in SDP parsing | Medium | Requires call setup |
| 3 | Zip bomb in ML model extraction | High | Requires server compromise/MITM |
| 3 | Path traversal in ZIP extraction | Critical | Requires server compromise/MITM |
| 4 | Clipboard Unicode injection | Medium | Easy — send message with invisible chars |
| 5 | Deep link parameter pollution | Medium | Easy — craft malicious link |
