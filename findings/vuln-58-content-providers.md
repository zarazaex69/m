# VULN-58: Content Providers Deep Analysis — Max Messenger (ru.oneme.app)

## Summary

Max messenger declares **8 content providers** in its AndroidManifest.xml. All are marked `exported="false"`, meaning they cannot be directly accessed by third-party apps. However, two providers grant URI permissions (`grantUriPermissions="true"`), which allows temporary access when URIs are shared via Intents. The FileProvider exposes overly broad paths including the entire external storage root.

---

## 1. Complete List of Content Providers

| # | Authority | Class | Exported | grantUriPermissions | Notes |
|---|-----------|-------|----------|---------------------|-------|
| 1 | `ru.oneme.app.androidx-startup` | `androidx.startup.InitializationProvider` | false | — | AndroidX Startup library |
| 2 | `ru.oneme.app.tracer-startup` | `androidx.startup.InitializationProvider` | false | — | Duplicate authority (see #7) |
| 3 | `ru.oneme.app.provider` | `androidx.core.content.FileProvider` | false | **true** | File sharing provider |
| 4 | `ru.oneme.app.firebaseinitprovider` | `com.google.firebase.provider.FirebaseInitProvider` | false | — | Disabled (`enabled="false"`) |
| 5 | `ru.oneme.app.notifications` | `one.me.android.notifications.NotificationsImagesProvider` | false | **true** | Notification images |
| 6 | `ru.oneme.app.dps-init-provider` | `ru.trace_flow.dps.internal.DpsInitProvider` | false | — | Disabled (`enabled="false"`) |
| 7 | `ru.oneme.app.tracer-startup` | `ru.ok.tracer.startup.InitializationProvider` | false | — | Tracer performance init |
| 8 | `ru.oneme.app.ApplicationProvider` | `ru.ok.android.commons.app.ApplicationProvider` | false | — | App context holder |

---

## 2. FileProvider Paths Analysis

**File:** `res/xml/provider_paths.xml`

```xml
<paths>
    <external-path name="external_files" path="." />
    <cache-path name="cache" path="." />
    <cache-path name="copy_media" path="copy/media/" />
</paths>
```

### Findings — MEDIUM RISK

| Path Type | Name | Path | Risk |
|-----------|------|------|------|
| `external-path` | `external_files` | `.` (root) | **HIGH** — Exposes entire external storage |
| `cache-path` | `cache` | `.` (root) | MEDIUM — Exposes entire app cache directory |
| `cache-path` | `copy_media` | `copy/media/` | LOW — Scoped to media copies |

**Issue:** The `external-path` with `path="."` maps to the root of external storage (`/sdcard/`). Any component that receives a granted URI from this FileProvider can access **any file on external storage**, not just app-specific files. If an Intent with `FLAG_GRANT_READ_URI_PERMISSION` is sent to a malicious activity (e.g., via intent redirection), the attacker gains read access to all external storage files.

**Attack scenario:** If any exported activity or deep link handler passes a FileProvider URI to an attacker-controlled component (via `startActivityForResult` or similar), the attacker can read arbitrary files from external storage.

---

## 3. NotificationsImagesProvider Analysis

**Class:** `one.me.android.notifications.NotificationsImagesProvider`  
**Authority:** `ru.oneme.app.notifications`  
**URI Pattern:** `content://ru.oneme.app.notifications/message_image/{uri}/{loadFromNetwork}`

### Can it read arbitrary files?

**No — mitigated by internal URI check.**

The `openFile()` method:
1. Only supports read mode (`"r"`) — throws `SecurityException` otherwise
2. Validates URI matches pattern `message_image/*/*` via UriMatcher
3. Extracts a URL string from path segment 1 and converts it to a URI via `trb.m22145J()`
4. **Critical check:** Calls `jel.m11375k()` which resolves symlinks (up to 10 iterations), canonicalizes paths, and checks if the resolved path falls within `/data/data/{packageName}` (the app's private directory)
5. If the URI points to an internal path → throws `SecurityException("Internal uri detected")`
6. Otherwise, looks up the image in disk cache and returns a `ParcelFileDescriptor`

### Security Controls in `jel.m11375k` / `jel.m11376l`:
- Resolves symlinks via `Os.readlink()` (up to 10 iterations)
- Canonicalizes path (removes `./`, `//`, resolves `..`)
- Checks if resolved path contains `/data/data/{packageName}`
- If it does → blocks access

### Residual Risk — LOW

- The provider is **not exported**, so external apps cannot directly query it
- With `grantUriPermissions="true"`, if the app grants a URI permission to another app, that app could attempt to read cached images
- The URI is fetched from network if not in cache (`loadFromNetwork=true`), which could be abused for SSRF-like behavior if an attacker controls the URL parameter — but this requires same-app access
- The internal URI check does NOT block reading from external storage paths or other apps' cache directories (only blocks the app's own `/data/data/` directory)

---

## 4. grantUriPermissions Analysis

Two providers have `grantUriPermissions="true"`:

| Provider | Authority | Risk |
|----------|-----------|------|
| FileProvider | `ru.oneme.app.provider` | Can grant read/write to any file in configured paths (entire external storage + cache) |
| NotificationsImagesProvider | `ru.oneme.app.notifications` | Can grant read access to cached notification images |

**Impact:** When the app uses `Intent.FLAG_GRANT_READ_URI_PERMISSION` or `FLAG_GRANT_WRITE_URI_PERMISSION` with these providers, the receiving component gets temporary access. If an intent redirection vulnerability exists elsewhere in the app, an attacker could leverage these grants.

---

## 5. Cursor Column Data Leakage

### FileProvider `query()` method:
Returns a `MatrixCursor` with columns:
- `_display_name` — filename or custom display name from `displayName` query parameter
- `_size` — file size in bytes

**Risk:** LOW — Standard FileProvider behavior. The `displayName` query parameter allows overriding the displayed filename, which could be used for social engineering (showing a different name than the actual file).

### Other providers:
- `NotificationsImagesProvider.query()` → returns `null` (no cursor data)
- `ApplicationProvider.query()` → throws `UnsupportedOperationException`
- All initialization providers → throw `UnsupportedOperationException` or return `null`

**No cursor-based data leakage identified.**

---

## 6. Tracer-Startup Provider Security Analysis

**Two providers share authority `ru.oneme.app.tracer-startup`:**

### Provider A: `androidx.startup.InitializationProvider` (line 155)
Standard AndroidX Startup library provider — initializes lifecycle and profile installer components.

### Provider B: `ru.ok.tracer.startup.InitializationProvider` (line 247)
Custom tracer initialization that:
1. Reads `tracer_is_disabled` boolean resource
2. Reads metadata keys matching `ru.ok.tracer.startup.Initializer@*` from app manifest
3. Instantiates classes via `Class.forName()` from metadata values
4. Calls initialization on each

**Security Implications — LOW:**
- Not exported, so external apps cannot trigger initialization
- Uses `Class.forName()` on values from the app's own manifest metadata — not user-controlled
- Initializes performance monitoring (PerformanceMetrics, Logger, Tracer, DiskUsage, HeapDump)
- No data is exposed via query/insert/update/delete (all throw `UnsupportedOperationException`)

### DpsInitProvider (`ru.oneme.app.dps-init-provider`):
- **Disabled** (`enabled="false"`)
- Uses obfuscated string decryption (`z8f.m26706a()`) to read resource identifiers and metadata keys
- Initializes some analytics/tracking SDK from manifest metadata
- No security risk since it's disabled and not exported

---

## 7. Duplicate Authority Conflict

Both `androidx.startup.InitializationProvider` and `ru.ok.tracer.startup.InitializationProvider` declare authority `ru.oneme.app.tracer-startup`. On Android, only one provider per authority can be registered. The last one declared (line 247) will win. This is likely a build configuration issue but has no security impact.

---

## 8. Overall Risk Assessment

| Finding | Severity | Exploitability |
|---------|----------|----------------|
| FileProvider exposes entire external storage (`path="."`) | **MEDIUM** | Requires intent redirection chain |
| NotificationsImagesProvider with grantUriPermissions | LOW | Not exported; internal URI check present |
| No exported providers | Positive | N/A — good security posture |
| Google Maps API key exposed in manifest | INFO | `AIzaSyDJbuC3fODS_aR7jcOkoP6qWIsQen9XARI` — may be restricted |
| Duplicate authority declaration | INFO | No security impact |

---

## 9. Recommendations

1. **Restrict FileProvider paths** — Replace `<external-path name="external_files" path="." />` with specific subdirectories needed for file sharing
2. **Audit all Intent grants** — Search for `FLAG_GRANT_READ_URI_PERMISSION` / `FLAG_GRANT_WRITE_URI_PERMISSION` usage to ensure no intent redirection can abuse the broad FileProvider paths
3. **Verify API key restrictions** — The exposed Google Maps API key should be restricted to the app's package name and signing certificate
4. **Fix duplicate authority** — Resolve the `ru.oneme.app.tracer-startup` conflict

---

*Analysis date: 2025-05-15*  
*Package: ru.oneme.app (Max Messenger)*  
*Source: Decompiled APK (jadx + apktool)*
