# VULN-50: App Update Mechanism — Forced Update & Downgrade Attack Surface

## Summary

Max messenger implements a server-controlled forced update mechanism that can completely lock users out of the app. The server sends an `app-update-type` field during session initialization; when set to `1`, the app immediately enters a force-update state that blocks all functionality including incoming calls and push notifications. The update redirects to a URL that is partially server-controlled, with no client-side signature verification of the update payload.

## Severity: HIGH

The server has unilateral ability to:
1. Lock any user out of the app entirely
2. Redirect users to arbitrary update URLs
3. Block incoming calls and notifications silently

## Technical Analysis

### 1. Server-Controlled Force Update Trigger

**File:** `p000/isg.java` (SessionInit handler)
**File:** `p000/ksg.java` (Session response parser)

The server sends an `app-update-type` field in the session initialization response:

```java
// ksg.java - parses server response
case "app-update-type":
    this.f34983o = j8h.m11181Q(kxaVar, 0);  // integer from server
    break;
```

When `app-update-type == 1`, the force update is triggered:

```java
// isg.java - processes the session init
if (ksgVar.f34983o == 1) {
    // Store the force update version in preferences
    ri9Var.f60403G0.mo2430y(ri9Var, ri9.f60396f1[20], "26.15.3");
    // Trigger force update UI
    ((mec) c0854qp2.f57312j.getValue()).m14098b();
    // Log session state as SESSION_FORCE_UPDATE (code 110)
    wm9Var.m24516y(rm9.SESSION_FORCE_UPDATE, null);
}
```

### 2. Force Update Logic (mec class)

**File:** `p000/mec.java`

```java
// m14097a() - isNeedForceUpdate check
public final boolean m14097a() {
    if (((int) ((rtd) this.f40258c).m20170m(hbg.f23921a, 0)) == 1) {
        ri9 ri9Var = (ri9) this.f40257b;
        String str = (String) ri9Var.f60403G0.mo677x(ri9Var, ri9.f60396f1[20]);
        // Compares stored version with hardcoded "26.15.3"
        if (zm0.m27177c(str, "26.15.3")) {
            return true;
        }
    }
    return false;
}
```

The `versionForceUpdateReceived` preference (index 20 in ri9.f60396f1) stores the version string received from the server. When it matches the current app version, the force update is active.

### 3. App Blocking Behavior

When force update is active, the app is **completely blocked**:

**Blocks intent handling** (`nuf.java:1486`):
```java
if (mecVar.m14097a()) {
    mecVar.m14098b();  // Shows force update screen
    return;  // Blocks all intent processing
}
```

**Blocks deep links** (`nuf.java:190`):
```java
if (mecVar.m14097a()) {
    // "handleIntent: ful failed, skipiing handlng intent"
    mecVar.m14098b();
    return;
}
```

**Blocks incoming calls** (`pb8.java:509-510`):
```java
if (((mec) yzbVar12.f84942c.getValue()).m14097a()) {
    ct4.m4647W0("NotifListenerImpl", 
        "Early return in onNotifCallStart cuz of forceUpdateLogic.isNeedForceUpdate()");
    return;  // Silently drops incoming call
}
```

**Blocks push notifications** (`cei.java:211`):
```java
if (((mec) eeiVar3.f15552h.getValue()).m14097a()) {
    u92Var.m22714O(str12, 1, "SKIP_PUSH_FORCE_UPDATE", z);
    // Notification silently dropped
}
```

**Replaces entire UI** (`lec.java:45-47`):
```java
if (!(rootControllerM15283l.m9867C() instanceof ForceUpdateScreen)) {
    rootControllerM15283l.m16176k1().m2016T(
        z5l.m26534I(new ForceUpdateScreen(), d48Var, d48Var));
}
```

### 4. Update URL Handling — No Validation

**File:** `p000/C0250cu.java`

The update button click handler (`m4733a`):

```java
public final void m4733a(Activity activity) {
    if (!((xu7) this.f10642b.getValue()).m25471a()) {
        // Google Play Services NOT available — opens URL directly in browser
        z8f.m26689J(new C1387zt(this, 0), activity, 
            (String) this.f10641a.getValue());  // Opens the URL
    } else {
        // Google Play Services available — uses in-app update flow
        f3mVarM18857a.mo4009d(qgi.f56619a, c0687ok);
    }
}
```

**Default URL** (`C1350yt.java:32`):
```java
case 0:
    return "https://download.max.ru/#android?version=26.15.3";
```

**Fallback to Play Store** (`C0803pc.java:67`):
```java
// If in-app update result type == 2, uses Play Store URL
// Otherwise uses the server-provided URL from c0250cu.f10641a.getValue()
((C1313xt) obj).f80968a == 2 
    ? "https://play.google.com/store/apps/details?id=ru.oneme.app" 
    : (String) c0250cu.f10641a.getValue()
```

The URL is opened via `z8f.m26689J` which simply calls:
```java
intent = new Intent("android.intent.action.VIEW");
intent.setData(Uri.parse(str));
context.startActivity(intent);
```

**No URL validation is performed** — the URL is opened directly in the browser/system handler.

### 5. `versionForceUpdateReceived` Preference

**File:** `p000/ri9.java` (index 20 in f60396f1 array)

```java
new job(ri9.class, "versionForceUpdateReceived", 
    "getVersionForceUpdateReceived()Ljava/lang/String;")
```

This preference:
- Is set by the server during session init when `app-update-type == 1`
- Stores the version string (currently "26.15.3")
- Is checked on every app interaction via `mec.m14097a()`
- Is cleared only when the app version no longer matches (`C0136c6.java` case 22)

### 6. Targeted Attack Potential

The force update is triggered per-session during `SessionInit`. The server sends device-specific information in the session init request:

```java
// isg.java mo464l() - session init request includes:
map.put("deviceType", cgjVarM23761j.f8564a);
map.put("appVersion", cgjVarM23761j.f8565b);
map.put("buildNumber", Integer.valueOf(cgjVarM23761j.f8566c));
map.put("osVersion", cgjVarM23761j.f8567d);
map.put("locale", cgjVarM23761j.f8569f);
map.put("deviceName", cgjVarM23761j.f8571h);
// Plus deviceId, clientSessionId
```

Since the server responds per-session with `app-update-type`, it can:
- **Target specific users** by their device ID or session
- **Target specific device types** or OS versions
- **Selectively force-update** individual users to redirect them to a malicious URL

### 7. Signature Verification — ABSENT for Direct Updates

**For Google Play in-app updates** (`sxl.java`): The Play Core library verifies the Play Store (`com.android.vending`) signature before binding to the update service. This provides signature verification through Google's infrastructure.

**For non-Play-Store path** (when Google Play Services unavailable): The app simply opens `https://download.max.ru/#android?version=...` in the browser. There is:
- **No APK signature verification** by the app itself
- **No certificate pinning** on the download URL
- **No integrity check** on the downloaded file
- The URL uses HTTPS but the domain could be compromised or MITM'd

The app does NOT perform any post-download verification of the APK. Android's built-in signature verification during installation is the only protection.

## Attack Scenarios

### Scenario 1: Server-Side Targeted Lockout
A compromised or malicious server operator sends `app-update-type=1` to a specific user's session. The user is immediately locked out of all messaging, calls, and notifications with no recourse.

### Scenario 2: Malicious Update Redirect
If the server-provided update URL (stored in `f10641a`) can be modified server-side, users could be redirected to a malicious APK download. On devices without Google Play Services (common in Russia/CIS markets where Max is popular), the direct URL path is used.

### Scenario 3: Downgrade Attack
The version comparison uses simple string equality (`zm0.m27177c` which is just `obj.equals(obj2)`). The server could potentially set a lower version string to force users to "update" to an older, vulnerable version.

### Scenario 4: Silent Communication Blackout
The force update blocks incoming calls and push notifications **silently** — the sender sees no error, but the recipient never receives the communication. This could be used for targeted surveillance or censorship.

## Affected Components

| Component | File | Role |
|-----------|------|------|
| Session Init Handler | `p000/isg.java` | Receives force update command |
| Force Update Logic | `p000/mec.java` | Checks if force update is needed |
| Force Update Screen | `one/p011me/appupdate/forceupdate/ForceUpdateScreen.java` | Blocking UI |
| Update URL Handler | `p000/C0250cu.java` | Opens update URL |
| URL Provider | `p000/C1350yt.java` | Provides default update URL |
| Navigation Blocker | `p000/lec.java` | Replaces root with force update screen |
| Intent Blocker | `p000/nuf.java` | Blocks all intents during force update |
| Call Blocker | `p000/pb8.java` | Silently drops incoming calls |
| Push Blocker | `p000/cei.java` | Silently drops push notifications |
| Preference Store | `p000/ri9.java` | Stores `versionForceUpdateReceived` |
| Session State | `p000/rm9.java` | `SESSION_FORCE_UPDATE` (code 110) |

## Recommendations

1. **Client-side validation**: The app should verify that the force update version is actually newer than the current version (prevent downgrade)
2. **URL allowlisting**: Update URLs should be validated against a hardcoded allowlist of trusted domains
3. **APK signature verification**: The app should verify the APK signature before prompting installation
4. **User notification**: Silent blocking of calls/notifications during force update should at minimum log locally
5. **Rate limiting**: Force update commands should have a cooldown to prevent abuse
6. **Transparency**: Users should be able to see why they're being force-updated and verify the update source
