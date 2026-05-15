# VULN-57: SharedPreferences Security Analysis — Max Messenger

## Summary

Max messenger (OneMe/TamTam) stores highly sensitive data in SharedPreferences without encryption. While `MODE_WORLD_READABLE`/`MODE_WORLD_WRITEABLE` are not used, the app stores authentication tokens, proxy server lists, server connection parameters, and PIN hashes in plain-text SharedPreferences. On rooted devices or via ADB backup exploitation, these values can be read or injected to hijack sessions, redirect traffic, or bypass security controls.

## Severity: HIGH

---

## 1. SharedPreferences Files Identified

| Preference File | Purpose |
|---|---|
| `user.prefs` | Main user preferences (via custom `w6h` wrapper) |
| `webrtc-android-sdk-pref` | WebRTC SDK settings |
| `app_crash_prefs` | Crash tracking |
| `FirebaseHeartBeat` | Firebase heartbeat |
| `com.google.android.gms.appid` | GMS App ID |
| `com.google.mlkit.internal` | ML Kit instance ID |
| `com.google.firebase.common.prefs:*` | Firebase common |
| `in_app_review_prefs` | In-app review |
| `keyboard_prefs` | Keyboard settings |
| `dev_tools` | Developer tools |
| `mytracker_prefs` | Analytics tracker |
| `one.me.sdk.design.theme` | Theme settings |
| `permissions_prefs` | Permission tracking |
| `presences.pref` | Online presence |
| `tracer` / `tracer-*` | Performance tracing |
| `exc_count.prefs` | Exception counting |

All use `MODE_PRIVATE` (0) — no `MODE_WORLD_READABLE`/`MODE_WORLD_WRITEABLE` found.

---

## 2. MODE_WORLD_READABLE / MODE_WORLD_WRITEABLE

**Not found.** All `getSharedPreferences()` calls use mode `0` (MODE_PRIVATE). The custom `OneMeApplication.getSharedPreferences()` override ignores the mode parameter entirely and delegates to a `ConcurrentHashMap`-based cache (`w6h` class).

---

## 3. Sensitive Data in SharedPreferences (CRITICAL)

The `yag` class (extended by `ri9`) stores the following in **plain-text** SharedPreferences via the `z49` wrapper (which implements `SharedPreferences` but adds NO encryption):

### Authentication & Session
| Key | Type | Risk |
|---|---|---|
| `auth.token` | String | **Session hijack** — full auth token in plaintext |
| `user.okToken` | String | OK platform token |
| `user.fcmToken` | String | Push notification token |
| `user.Id` | Long | User identifier |
| `user.Phone` | String | User phone number |
| `user.Phone.Code` | String | Phone country code |
| `app.pin_<userId>` | String | **PIN/passcode hash** stored in prefs |
| `user.callSession` | String | Active call session IDs |
| `user.deviceAvatarPath` | String | File path — potential path traversal |

### Server Connection (Traffic Redirection Risk)
| Key | Type | Risk |
|---|---|---|
| `server.host` | String | **Server hostname** — injectable for MITM |
| `server.port` | String | **Server port** — injectable |
| `server.useTls` | Boolean | **TLS toggle** — can disable encryption |
| `app.currentProxyList` | String | Proxy server list |
| `app.pushProxyList` | String | Push proxy list |
| `app.lastSuccessProxy` | String | Last used proxy |
| `server.timeDelta` | Long | Server time delta |

### Feature Flags & Debug
| Key | Type | Risk |
|---|---|---|
| `app.debugHostRotation` | Boolean | Debug host rotation |
| `app.debugUaDnsEmulation` | Boolean | DNS emulation toggle |
| `app.privacy.online.show` | Boolean | Privacy setting bypass |
| `app.pinLock.screenshotEnabled` | Boolean | Screenshot protection bypass |
| `web_app:ssl_check` | Boolean | **SSL check toggle** |

---

## 4. Backup/Restore Injection Vector

**AndroidManifest.xml**: `android:allowBackup="false"` — backup is disabled.

**However**, the `z49` SharedPreferences wrapper does NOT use `EncryptedSharedPreferences` or any encryption layer. On a rooted device, preferences are directly accessible at:
```
/data/data/one.me.android/shared_prefs/
```

**Attack vector on rooted device:**
1. Read `auth.token` for session hijack
2. Modify `server.host`/`server.port` to redirect traffic
3. Set `server.useTls` to false to downgrade connections
4. Modify `web_app:ssl_check` to disable SSL validation
5. Inject malicious proxy via `app.currentProxyList`

---

## 5. Preference Values Used Without Validation

### 5.1 Server Host/Port → Network Connection (CRITICAL)
- `server.host` and `server.port` stored in `ri9` class are used to establish the main messaging connection
- No validation observed on these values before use in socket connections
- Injecting a malicious host redirects all messaging traffic

### 5.2 Proxy List → Network Routing (HIGH)
- `app.currentProxyList`, `app.pushProxyList`, `app.lastSuccessProxy` are String values
- Used in network proxy selection (`lcf.java:212` creates sockets with proxy)
- No URL/host validation visible in the decompiled code

### 5.3 deviceAvatarPath → File System (MEDIUM)
- `user.deviceAvatarPath` stored as String, used for file operations
- Potential path traversal if injected with `../../` sequences

### 5.4 Java Deserialization from SharedPreferences (HIGH)
- `AbstractC0432hr.m9702C()` reads Base64-encoded strings from SharedPreferences
- Decodes and passes to `ObjectInputStream.readObject()`
- **Classic Java deserialization vulnerability** — if an attacker can write to SharedPreferences, they can achieve arbitrary code execution via gadget chains

```java
// AbstractC0432hr.java - Dangerous deserialization
public Serializable m9702C(String str) {
    String string = ((SharedPreferences) this.f25188b).getString(str, null);
    if (string != null) {
        ByteArrayInputStream bais = new ByteArrayInputStream(Base64.decode(string, 0));
        ObjectInputStream ois = new ObjectInputStream(bais);
        Serializable serializable = (Serializable) ois.readObject(); // RCE vector
    }
}
```

---

## 6. user.prefs File Contents

The `user.prefs` SharedPreferences file (accessed via `OneMeApplication`) stores:

| Key | Purpose |
|---|---|
| `user.lang` | User language (default: "ru") |
| `app.lang.multilang` | Multi-language enabled |
| `app.extra.text.size.mode` | Text size setting |
| `app.extra.text.size.sp` | Text size in SP |
| `app.pinLock.screenshotEnabled` | Screenshot protection |
| `app.privacy.online.show` | Online status visibility |
| `app.privacy.content.level.access` | Content access level |
| `app.privacy.safe_mode` | Safe mode toggle |
| `app.calls.incoming.ringtone` | Ringtone path |
| `app.notification.chats.show` | Notification settings |
| `app.notification.dialogs.show` | Dialog notifications |
| `app.media.video.compress` | Video compression |
| `app.messages.enable.animations` | Animation toggle |
| `app.messages.enable.double.tap.reactions` | Reaction settings |
| `app.editor.color` / `app.editor.width` | Editor settings |
| `app.last.media_keyboard.page.id` | Last keyboard page |

The main user preferences file also contains all keys from `yag`/`ri9` hierarchy including auth tokens and server settings.

---

## 7. WebView Cookie/Storage Manipulation

No direct SharedPreferences → WebView cookie manipulation path found. However:

- `web_app:ssl_check` preference controls SSL validation behavior for web apps
- `webview-cache-enabled` PMS key controls WebView caching
- WebApp screens (`WebAppRootScreen`) read `app.pinLock.screenshotEnabled` from preferences
- The `webapp-ds-keys-count` and `webapp-ss-keys-count` PMS keys control WebApp storage limits

**Indirect attack**: Modifying `web_app:ssl_check` to `false` could allow MITM attacks on WebView content.

---

## 8. Attack Scenarios

### Scenario A: Session Hijack (Root Required)
1. Read `/data/data/one.me.android/shared_prefs/*.xml`
2. Extract `auth.token` value
3. Use token to impersonate user on another device

### Scenario B: Traffic Redirection (Root Required)
1. Modify `server.host` to attacker-controlled server
2. Set `server.useTls` to `false`
3. Set `web_app:ssl_check` to `false`
4. All messaging traffic routes through attacker

### Scenario C: Remote Code Execution via Deserialization (Root Required)
1. Craft malicious serialized Java object (gadget chain)
2. Base64-encode and write to SharedPreferences key read by `AbstractC0432hr.m9702C()`
3. App deserializes on next read → arbitrary code execution

### Scenario D: Privacy Bypass
1. Set `app.privacy.online.show` to `true` (force online visibility)
2. Set `app.pinLock.screenshotEnabled` to `true` (enable screenshots)
3. Modify `app.privacy.safe_mode` to bypass content restrictions

---

## 9. Recommendations

1. **Use EncryptedSharedPreferences** for all sensitive data (auth tokens, server config, PIN)
2. **Validate server.host/port** against allowlist before establishing connections
3. **Remove ObjectInputStream deserialization** from SharedPreferences — use JSON or Protobuf instead
4. **Store auth.token in Android Keystore** rather than SharedPreferences
5. **Integrity-check preference files** on app startup (HMAC)
6. **Add input validation** for proxy list values before use in network connections
7. **Remove debug preferences** (`app.debugHostRotation`, `app.debugUaDnsEmulation`) from production builds

---

## Files Analyzed

- `one/p011me/android/OneMeApplication.java` — SharedPreferences override, user.prefs access
- `p000/yag.java` — Main preference key definitions (auth, proxy, server)
- `p000/ri9.java` — Extended preferences (server.host, server.port, PIN)
- `p000/ugj.java` — UI/privacy preferences
- `p000/AbstractC0432hr.java` — Deserialization from SharedPreferences
- `p000/AbstractC0335f4.java` — Base preference class using z49
- `p000/z49.java` — SharedPreferences implementation (no encryption)
- `p000/w6h.java` — SharedPreferences cache manager
- `p000/sv3.java` — auth.token storage
- `p000/adc.java` — auth.token retrieval
- `p000/vw3.java` — PIN check from preferences
- `p000/xf0.java` — Token write operations
- `base/resources/AndroidManifest.xml` — allowBackup=false confirmed
