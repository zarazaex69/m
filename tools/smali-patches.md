# Smali Patches — Max Messenger Privacy Protection

## For modifying YOUR OWN copy of the app to protect privacy.

## Prerequisites

```bash
apktool d base.apk -o max-patched
# Edit smali files below
apktool b max-patched -o max-patched.apk
# Sign with your key
apksigner sign --ks my.keystore max-patched.apk
```

---

## 1. Disable VPN Detection

**File**: `smali/p000/jf4.smali`
**Method**: `mo2504e()Z` (isVpnConnected)

```smali
# Original: checks hasTransport(4)
# Patch: always return false

.method public final mo2504e()Z
    .locals 1
    const/4 v0, 0x0
    return v0
.end method
```

## 2. Disable Contact Sync

**File**: Find the contact upload task class
**Method**: The method that calls SYNC opcode 21

```smali
# Patch: return immediately from contact sync method
.method public final <sync_method>()V
    .locals 0
    return-void
.end method
```

## 3. Disable Draft Sync

**File**: `smali/p000/<DraftSave_class>.smali`
**Method**: The execute/run method

```smali
# Patch: no-op the draft save task
# Find the class that implements DraftSave and patch its execute:
    return-void
```

## 4. Disable Telemetry (MyTracker + AppTracer)

**File**: `smali/com/my/tracker/MyTracker.smali`
**Method**: `init` or `trackEvent`

```smali
# Patch MyTracker.init to no-op:
.method public static init(...)V
    .locals 0
    return-void
.end method
```

**File**: `smali/ru/ok/tracer/Tracer.smali`
```smali
# Patch AppTracer init to no-op:
.method public static init(...)V
    .locals 0
    return-void
.end method
```

## 5. Disable VPN Warning UI

**File**: `smali/one/me/vpnconnectedwarning/VpnConnectedWarningBottomSheet.smali`

```smali
# Patch constructor to immediately dismiss:
# Or patch the caller in ContactListWidget/CallHistoryScreen
# to skip the VPN check before showing the sheet
```

## 6. Force Disable Location Push

**File**: `smali/p000/cei.smali` (around line 260 in Java)

```smali
# Find the "LocationRequest" string comparison
# Patch: skip the location acquisition call
# Replace invoke of m6664f with nop/return
```

## 7. Strip EXIF Before Upload

**File**: Find the image upload preparation class
**Method**: Before upload, add EXIF stripping

```smali
# Add call to ExifInterface.setAttribute("GPSLatitude", null) etc.
# Or patch NativeJpegTranscoder to strip GPS tags
```

---

## Notes

- After patching, the app will fail server-side signature check
  (the server verifies APK signature — see vuln-09)
- Use Magisk + LSPosed + module approach instead for non-invasive patches
- Frida hooks (tools/frida-hooks.js) achieve the same without repackaging
