# VULN-100: Final Vulnerability Hunt — Max Messenger

**Date:** 2025-05-15
**Target:** Max Messenger (Android) — decompiled source
**Source:** ~/max/decompiled/jadx/base/sources

---

## 1. Notification Content on Lock Screen

**Severity:** Medium
**Status:** CONFIRMED — Message content visible on lock screen by default

### Evidence

In `p000/w1c.java` (notification builder wrapper):
```java
// Line 108: visibility field defaults to 0
public int f75614y = 0;
```

In `p000/wui.java` (actual Notification.Builder construction):
```java
// Line 84: visibility is set from the field (default 0 = VISIBILITY_PUBLIC)
((Notification.Builder) this.f78354d).setVisibility(w1cVar.f75614y);
// Line 85: no public version set — full content exposed
((Notification.Builder) this.f78354d).setPublicVersion(null);
```

In `p000/soc.java` (notification helper), the `m21389j()` method builds chat notifications but **never sets `f75614y`** to `VISIBILITY_PRIVATE` (-1) or `VISIBILITY_SECRET` (-2). The field retains its default value of `0` (`VISIBILITY_PUBLIC`).

### Impact

All message notification content (sender name, message text) is displayed on the lock screen by default. Any person with physical access to the device can read incoming messages without unlocking. The `setPublicVersion(null)` call confirms no redacted alternative is provided.

### Recommendation

Set `f75614y = -1` (VISIBILITY_PRIVATE) for message notifications and provide a `setPublicVersion()` with redacted content (e.g., "New message").

---

## 2. Biometric Authentication Bypass

**Severity:** Low-Medium
**Status:** POTENTIAL — Biometric cancel triggers `onBiometryFail` callback with no lockout

### Evidence

In `p000/p9k.java` (BiometricPrompt callback wrapper):
```java
// mo14165c() is called on authentication error (including user cancel)
public final void mo14165c() {
    ((el7) this.f52726a.f55982c).invoke();  // calls onBiometryFail
}

// mo14166d() is called on authentication failed (wrong fingerprint)
public final void mo14166d() {
    ct4.m4610E((String) this.f52726a.f55983d, "onAuthenticationFailed");  // just logs
}
```

In `WebAppRootScreen.java:968`:
```java
this.f50780J0 = new q9k(requireActivity(),
    new d6b(1, m17030s1(), ijk.class, "onBiometrySuccess", ...),
    new myc(0, m17030s1(), ijk.class, "onBiometryFail", ...));
```

The `onBiometryFail` callback is invoked on **any** authentication error, including user cancellation (error code 13). The callback is a simple method reference (`myc` with priority 0) that does not distinguish between cancel and actual failure.

### Impact

The biometric prompt is used for WebApp access control. When the user cancels the prompt, `onBiometryFail` is called — but without examining the error code, the app may simply dismiss the prompt without enforcing a lockout or preventing re-access. The actual bypass depends on whether the calling screen blocks navigation on failure. The use of `CryptoObject` in the success path (`onBiometrySuccess(CryptoObject)`) suggests crypto-backed auth, which is positive — but the cancel path does not invalidate any session state.

### Recommendation

Distinguish between user cancellation (error 13) and actual failures. On cancel, maintain the locked state. Implement attempt counting and lockout.

---

## 3. Intent Redirection via startActivityForResult

**Severity:** Low
**Status:** NOT EXPLOITABLE — No sensitive data returned via setResult

### Evidence

Activities using `startActivityForResult`:
- `CallScreen.java:1266` — screen capture intent (system MediaProjection)
- `MediaBarWidget.java:870` — media picker (returns file URIs)
- `MediaTypePickerWidget.java:128` — media type selection
- `QrScannerWidget.java:217` — QR scanner result
- `SettingRingtoneScreen.java` — ringtone picker

All `onActivityResult` handlers process media URIs, ringtone selections, or system-provided results. No handler returns authentication tokens, session data, or user credentials via `setResult()`.

### Impact

Minimal. The data returned through activity results is limited to media file URIs and UI selections. These are not sensitive in the context of a messaging app's security model. An intercepting app would only obtain file picker selections.

---

## 4. PendingIntent Hijacking on Android <12

**Severity:** Medium-High
**Status:** CONFIRMED — 8 mutable PendingIntents on pre-Android 12

### Evidence

The helper method `ij9.m10405p()` in `p000/ij9.java:643`:
```java
public static final int m10405p(int i) {
    // On SDK < 31: returns flag as-is (no IMMUTABLE added)
    // On SDK >= 31: adds FLAG_MUTABLE if IMMUTABLE not already set
    return (Build.VERSION.SDK_INT < 31 || (67108864 & i) != 0) ? i : i | 33554432;
}
```

When called with `134217728` (FLAG_UPDATE_CURRENT only), on Android < 12 (SDK < 31), the PendingIntent is created **without FLAG_IMMUTABLE**, making it implicitly mutable.

**8 instances** found with only `FLAG_UPDATE_CURRENT` (134217728):
- `soc.java:437` — notification reply action (chat server ID, push ID, message ID)
- `soc.java:477` — mark-as-read action (chat server ID, message mark)
- `soc.java:560` — notification delete intent
- `ij9.java:851` — notification content intent (opens chat)
- `pn0.java:129` — alarm manager background check
- `pn0.java:203` — alarm cancel
- `au7.java:170,174` — Google API activity intents

Additionally, `xfa.java:460`:
```java
pendingIntent = PendingIntent.getBroadcast(..., Build.VERSION.SDK_INT >= 31 ? 33554432 : 0);
// On pre-31: flag is 0 — completely unprotected, mutable PendingIntent
```

### Impact

On Android 11 and below, a malicious app can intercept and modify these PendingIntents. The notification-related ones are particularly dangerous:
- **Reply action**: attacker can modify `CHAT_SERVER_ID` and `MESSAGE_SERVER_ID` extras to redirect replies
- **Mark-as-read**: attacker can mark arbitrary messages as read
- **Content intent**: attacker can redirect notification taps

### Recommendation

Always use `FLAG_IMMUTABLE` (67108864) for PendingIntents that don't need to be modified. For the reply action (which uses RemoteInput), use `FLAG_MUTABLE | FLAG_UPDATE_CURRENT` explicitly with proper intent targeting.

---

## 5. Task Hijacking (StrandHogg)

**Severity:** Medium
**Status:** CONFIRMED — MainActivity uses singleTask with custom taskAffinity

### Evidence

From `AndroidManifest.xml`:
```xml
<activity
    android:name="one.p011me.android.MainActivity"
    android:exported="true"
    android:taskAffinity="one.me.application"
    android:launchMode="singleTask"
    ...>
    <intent-filter>
        <action android:name="android.intent.action.MAIN"/>
        <category android:name="android.intent.category.LAUNCHER"/>
    </intent-filter>
    <intent-filter>
        <action android:name="android.intent.action.SEND"/>
        <category android:name="android.intent.category.DEFAULT"/>
        <data android:mimeType="*/*"/>
    </intent-filter>
    <intent-filter>
        <action android:name="android.intent.action.SEND_MULTIPLE"/>
        <category android:name="android.intent.category.DEFAULT"/>
        <data android:mimeType="*/*"/>
    </intent-filter>
</activity>
```

The combination of:
1. `launchMode="singleTask"` — allows task reparenting
2. `taskAffinity="one.me.application"` — custom affinity (not default package name)
3. `exported="true"` with SEND intent filters — externally accessible

### Impact

**StrandHogg 1.0**: A malicious app can declare the same `taskAffinity` ("one.me.application") and place itself in the same task stack. When the user launches Max, the malicious activity appears on top, enabling phishing (fake login screen) or data theft.

**StrandHogg 2.0** (CVE-2020-0096, Android < 10): The `singleTask` launch mode allows a malicious app to hijack the task entirely, replacing the legitimate activity.

The SEND/SEND_MULTIPLE intent filters with `mimeType="*/*"` increase the attack surface — any app sharing content can trigger task interaction.

### Recommendation

- Set `taskAffinity=""` (empty string) to use the default package-based affinity
- Consider `launchMode="singleInstance"` if task isolation is needed
- Add `android:allowTaskReparenting="false"` explicitly

---

## Summary

| # | Vulnerability | Severity | Status |
|---|---|---|---|
| 1 | Notification content on lock screen | Medium | Confirmed |
| 2 | Biometric authentication bypass | Low-Medium | Potential |
| 3 | Intent redirection via startActivityForResult | Low | Not exploitable |
| 4 | PendingIntent hijacking (Android <12) | Medium-High | Confirmed (8 instances) |
| 5 | Task hijacking (StrandHogg) | Medium | Confirmed |

**Critical findings**: #4 (PendingIntent hijacking) is the most impactful — it allows modification of notification actions on older Android versions, potentially redirecting message replies. #5 (StrandHogg) enables phishing attacks against Max users. #1 exposes message content to shoulder-surfing on locked devices.
