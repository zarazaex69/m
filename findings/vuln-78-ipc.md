# VULN-78: Inter-Process Communication (IPC) Analysis — Max Messenger

## Summary

Analysis of IPC mechanisms in Max messenger (package: `ru.oneme.app`) for privilege escalation and intent-based attack vectors.

---

## 1. Services Declared in Manifest

| # | Service | Exported | Permission | ForegroundServiceType |
|---|---------|----------|------------|----------------------|
| 1 | `one.me.android.concurrent.SingleCoreFeature$ToggleService` | No (disabled) | — | — |
| 2 | `one.me.android.concurrent.WatchdogFeature$ToggleService` | No (disabled) | — | — |
| 3 | `one.me.android.concurrent.UseSystemThreadPoolQueueFeature$ToggleService` | No (disabled) | — | — |
| 4 | `one.me.android.fresco.FrescoExecutorFeature$ToggleService` | No (disabled) | — | — |
| 5 | `androidx.appcompat.app.AppLocalesMetadataHolderService` | No (disabled) | — | — |
| 6 | `ru.ok.tamtam.android.services.NotificationTamService` | **No explicit export attr** | — | — |
| 7 | **`one.me.webapp.util.WebAppNfcService`** | **Yes** | `android.permission.BIND_NFC_SERVICE` | — |
| 8 | `androidx.work.impl.foreground.SystemForegroundService` | No | — | camera\|dataSync\|location\|mediaPlayback\|microphone |
| 9 | `one.me.calls.impl.service.CallServiceImpl` | No | `BIND_TELECOM_CONNECTION_SERVICE` | camera\|mediaPlayback\|mediaProjection\|microphone\|phoneCall |
| 10 | `one.me.background.wake.BackgroundListenService` | No | — | dataSync |
| 11 | `one.me.sdk.vendor.push.FcmMessagingService` | No | — | — |
| 12 | `one.me.android.media.service.OneMeMediaSessionService` | No | — | mediaPlayback |
| 13 | `ru.ok.tamtam.android.calls.MediaProjectionService` | No | — | mediaProjection |
| 14 | `com.google.android.gms.auth.api.signin.RevocationBoundService` | Yes | `com.google.android.gms.auth.api.signin.permission.REVOCATION_NOTIFICATION` | — |
| 15 | `androidx.work.impl.background.systemjob.SystemJobService` | Yes | `android.permission.BIND_JOB_SERVICE` | — |
| 16 | `one.video.calls.sdk.upload.FileUploadService` | No | `BIND_JOB_SERVICE` | — |
| 17 | `ru.ok.android.externcalls.analytics.internal.upload.UploadService` | No | `BIND_JOB_SERVICE` | — |

---

## 2. Bound Services (onBind implementations)

| Service | Returns IBinder | Notes |
|---------|----------------|-------|
| `BackgroundListenService` | Yes (line 58) | Returns null binder, not exploitable |
| `OneMeMediaSessionService` | Yes (line 653) | MediaBrowserService binding |
| `FrescoExecutorFeature$ToggleService` | Yes (line 12) | Disabled, returns null |
| `WatchdogFeature$ToggleService` | Yes (line 12) | Disabled, returns null |
| `UseSystemThreadPoolQueueFeature$ToggleService` | Yes (line 12) | Disabled, returns null |
| `SingleCoreFeature$ToggleService` | Yes (line 12) | Disabled, returns null |
| `MediaProjectionService` | Yes (line 90) | Not exported |
| `NotificationTamService` | Yes (line 1048) | Not exported |

**Finding:** No bound services expose a usable Binder/Messenger interface to external apps. All services with meaningful `onBind()` implementations are either not exported or protected by system permissions.

---

## 3. Exported Services Accepting External Commands

### 3.1 WebAppNfcService (exported=true)

- **Class:** `one.me.webapp.util.WebAppNfcService`
- **Extends:** `android.nfc.cardemulation.HostApduService`
- **Permission:** `android.permission.BIND_NFC_SERVICE` (system-level)
- **Intent-filter:** `android.nfc.cardemulation.action.HOST_APDU_SERVICE`
- **Risk:** LOW — Protected by system permission. Only the NFC subsystem can bind to it. The `processCommandApdu()` method processes APDU commands from NFC readers but is sandboxed by the Android NFC framework.

### 3.2 NotificationTamService (no explicit exported attribute)

- **Manifest:** `<service android:name="ru.ok.tamtam.android.services.NotificationTamService"/>`
- **Target SDK ≥ 31:** Without `android:exported`, defaults to `false` since no intent-filter is declared.
- **onStartCommand actions:** Handles `ru.ok.tamtam.action.DIRECT_REPLY`, `ru.ok.tamtam.action.NOTIF_CANCEL_BUNDLED`, and others.
- **Risk:** LOW — Not accessible from external apps on modern Android.

---

## 4. PendingIntent Analysis

### 4.1 Flag Usage Summary

| Value | Decoded Flags | Usage Location |
|-------|---------------|----------------|
| 201326592 | FLAG_IMMUTABLE \| FLAG_UPDATE_CURRENT | CallServiceImpl, BackgroundCheckReceiver alarms |
| 67108864 | FLAG_IMMUTABLE | OneMeMediaSessionService notification actions |
| 134217728 | FLAG_UPDATE_CURRENT | PipWorker notification actions |

### 4.2 FLAG_MUTABLE Usage (Potential Vulnerability)

**File:** `p000/xfa.java` (MediaSessionCompat)
```java
PendingIntent.getBroadcast(oneMeMediaSessionService, 0, intent2, 
    Build.VERSION.SDK_INT >= 31 ? 33554432 : 0);
// 33554432 = FLAG_MUTABLE
```

**Risk:** MEDIUM — On Android 12+ (SDK 31), the media button PendingIntent is created with `FLAG_MUTABLE`. This is required by the MediaSession API for media button dispatch but could theoretically allow a malicious app to fill in the intent extras if it can intercept the PendingIntent.

### 4.3 Helper Function `ij9.m10405p()`

```java
public static final int m10405p(int i) {
    return (Build.VERSION.SDK_INT < 31 || (67108864 & i) != 0) ? i : i | 33554432;
}
```

This adds `FLAG_MUTABLE` when `FLAG_IMMUTABLE` is not already set and SDK ≥ 31. Used in PipWorker where `FLAG_UPDATE_CURRENT` (134217728) is passed — resulting in `FLAG_UPDATE_CURRENT | FLAG_MUTABLE`.

### 4.4 PipWorker Implicit Intent Issue

**File:** `ru/p027ok/messages/services/PipWorker.java`
```java
Intent intent = new Intent("ru.ok.video.ACTION_VIDEO_STOP");
// No explicit component or package set!
PendingIntent.getBroadcast(context, 5, intent, FLAG_UPDATE_CURRENT | FLAG_MUTABLE);
```

**Risk:** MEDIUM — The first two `ACTION_VIDEO_STOP` intents lack an explicit package. Combined with `FLAG_MUTABLE`, a malicious app could register a receiver for this action and potentially intercept or modify the broadcast. However, the third intent (`ACTION_VIDEO_PLAY`) correctly sets `.setPackage("ru.oneme.app")`.

### 4.5 ij9.m10394U() — Android 14 Safety Check

```java
public static int m10394U(Intent intent, int i) {
    return (Build.VERSION.SDK_INT < 34 || 
        !(((str = intent.getPackage()) == null || str.length() == 0 || 
        intent.getComponent() == null) && ((33554432 & i) != 0))) ? i : 16777216 | i;
}
```

On Android 14+, if a mutable PendingIntent has no package/component set, it adds `FLAG_ONE_SHOT` (16777216) as a safety measure. This mitigates the PipWorker issue on Android 14+.

---

## 5. Intent Redirection Vulnerabilities

### 5.1 FirebaseInstanceIdReceiver — wrapped_intent Pattern

**File:** `p000/v8j.java`
```java
Parcelable parcelableExtra = intent.getParcelableExtra("wrapped_intent");
Intent intent2 = parcelableExtra instanceof Intent ? (Intent) parcelableExtra : null;
```

**Context:** This is the standard Firebase Cloud Messaging `wrapped_intent` pattern in `FirebaseInstanceIdReceiver` (exported=true, permission=`com.google.android.c2dm.permission.SEND`).

**Risk:** LOW — Protected by the `com.google.android.c2dm.permission.SEND` permission (signature-level, held by GMS). The wrapped intent is only used to dispatch to the FCM service internally.

### 5.2 LinkInterceptorActivity — Deeplink Forwarding

**File:** `one/p011me/android/deeplink/LinkInterceptorActivity.java`
- **Exported:** Yes
- **Intent-filters:** Handles `VIEW` intents for `https://max.ru/*` and `max://max.ru/*`
- **Behavior:** Forwards action and data to `MainActivity`:
```java
Intent intent2 = new Intent(this, MainActivity.class);
intent2.setAction(intent.getAction());
intent2.setData(intent.getData());
startActivity(intent2);
```

**Risk:** LOW-MEDIUM — The activity forwards the URI data from external intents to MainActivity. While the target is explicit (MainActivity.class), a crafted deeplink URI could potentially trigger unintended navigation within the app. The `MyTracker.handleDeeplink()` call processes the URI before internal routing, which could be an additional attack surface.

---

## 6. Services Running in Separate Processes

**Finding:** No services declare `android:process` attribute. All services run in the main application process.

This means:
- No cross-process IPC within the app itself
- No separate process isolation for sensitive operations (calls, media)
- Reduced attack surface for inter-process privilege escalation within the app

---

## 7. Risk Assessment

| Finding | Severity | Exploitability |
|---------|----------|----------------|
| PipWorker implicit intent + FLAG_MUTABLE | Medium | Requires malicious app with matching receiver; mitigated on Android 14+ |
| MediaSession FLAG_MUTABLE PendingIntent | Low-Medium | Standard API requirement; limited exploitation path |
| LinkInterceptorActivity deeplink forwarding | Low-Medium | Requires crafted URI; limited to in-app navigation |
| WebAppNfcService exported | Low | System permission protected |
| NotificationTamService command handling | Low | Not exported on target SDK ≥ 31 |

---

## 8. Recommendations for Further Investigation

1. **PipWorker implicit broadcast:** Verify if `ru.ok.video.ACTION_VIDEO_STOP` can be intercepted by a malicious app on Android < 14 to hijack video playback state.
2. **Deeplink routing:** Map all internal deeplink routes handled by `LinkInterceptorWidget` to identify if any route can trigger privileged actions (e.g., account linking, payment flows).
3. **MyTracker deeplink handling:** Investigate if `MyTracker.handleDeeplink()` can be abused to redirect to attacker-controlled URLs.
4. **NotificationTamService on older SDKs:** On devices with targetSdk < 31, the service may be implicitly exported, allowing external apps to trigger `DIRECT_REPLY` actions.
