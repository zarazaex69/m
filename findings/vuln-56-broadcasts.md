# VULN-56: Broadcast Receiver Analysis — Max Messenger

## Summary

Analysis of broadcast receivers in Max (ru.oneme.app) for injection, abuse, and manipulation vectors. The app targets SDK 35 (Android 15) with minSdk 26.

## 1. Exported Broadcast Receivers (AndroidManifest.xml)

| Receiver | Exported | Permission Protected | Intent Filters |
|----------|----------|---------------------|----------------|
| `ru.ok.tamtam.android.services.BootCompletedReceiver` | **YES** | None | `BOOT_COMPLETED`, `QUICKBOOT_POWERON`, `com.htc.intent.action.QUICKBOOT_POWERON` |
| `one.me.background.wake.BackgroundWakeBootReceiver` | **YES** (disabled) | None | `MY_PACKAGE_REPLACED`, `BOOT_COMPLETED` |
| `com.google.firebase.iid.FirebaseInstanceIdReceiver` | **YES** | `com.google.android.c2dm.permission.SEND` | `com.google.android.c2dm.intent.RECEIVE` |
| `androidx.work.impl.diagnostics.DiagnosticsReceiver` | **YES** | `android.permission.DUMP` | `androidx.work.diagnostics.REQUEST_DIAGNOSTICS` |
| `androidx.profileinstaller.ProfileInstallReceiver` | **YES** | `android.permission.DUMP` | Profile installer actions |

### Non-exported receivers (not directly attackable):
- `one.me.android.calls.CallNotifierBroadcastReceiver` — **not exported**, no intent-filter
- `one.me.android.LocaleAndTimeChangeReceiver` — **not exported**, no intent-filter
- `one.me.background.wake.BackgroundCheckReceiver` — exported=false, disabled

## 2. Dynamically Registered Receivers

Key dynamically registered receivers found:
- **CallsBluetoothManager.BluetoothHeadsetBroadcastReceiver** — handles Bluetooth SCO/headset state changes
- **CallsWiredHeadsetManager.headsetReceiver** — handles `HEADSET_PLUG` and `MEDIA_BUTTON` actions
- **LocaleAndTimeChangeReceiver** — registered at runtime for locale/time changes

## 3. BootCompletedReceiver — What It Does on Boot

**File:** `ru.ok.tamtam.android.services.BootCompletedReceiver`

On `BOOT_COMPLETED`:
1. Logs "BootCompletedReceiver" to BackgroundWake tag
2. Validates the action is `android.intent.action.BOOT_COMPLETED`
3. Executes `RunnableC0793p3` (case 20) which:
   - Calls `onBootCompleted` on a `uei` instance
   - Sets a flag via `f3e.m25798A(true)` — likely marks app as "booted"
   - Calls `vei.m23603c(false)` — likely reschedules background sync
   - Calls `zgi.m26995a()` — likely reinitializes background services
   - Calls `toc.m22080d()` — additional background task scheduling

**Risk:** LOW — The receiver is exported but only responds to system `BOOT_COMPLETED` action. An attacker can send a fake `BOOT_COMPLETED` intent to trigger background service rescheduling, but this only causes the app to reinitialize its background tasks (no data leak, no sensitive action).

## 4. Attacker-Controlled Data from Intents

### CallNotifierBroadcastReceiver (NOT exported — internal only)
The `va1.m23482b()` method processes intent data:
- Reads `intent.getAction()` to determine call notification action
- Reads `intent.getBooleanExtra("incoming_param_is_video", false)` 
- Reads `intent.getExtras()` and forwards them to MainActivity

Actions handled:
- `action-open-call` — opens active call screen
- `action-open-incoming` — opens incoming call screen (passes all extras)
- `action-join-link` — joins a call via link (passes all extras)
- `action-rate-call` — opens call rating (passes all extras)
- `action-unknown-call` — handles unknown call notification
- Decline call, toggle mute, end call actions

**Risk:** LOW-MEDIUM — While the receiver processes attacker-controllable extras and forwards them to MainActivity, it is **NOT exported** in the manifest. It's only triggered via `PendingIntent.getBroadcast()` from notification actions. An attacker cannot directly invoke it.

### FirebaseInstanceIdReceiver (exported, permission-protected)
- Reads `intent.getParcelableExtra("pending_intent")` and calls `.send()` on it
- Reads `intent.getExtras()` and processes them
- Protected by `com.google.android.c2dm.permission.SEND` (signature-level GMS permission)

**Risk:** LOW — Protected by GMS signature permission; only Google Play Services can send to it.

## 5. Ordered Broadcast Vulnerabilities

### CallsWiredHeadsetManager — abortBroadcast() Usage
```java
// On MEDIA_BUTTON action with specific key codes (CALL, ENDCALL, HEADSETHOOK, PLAY, PAUSE)
broadcastReceiver.abortBroadcast();
```

The app registers a dynamic receiver for `android.intent.action.MEDIA_BUTTON` and aborts the broadcast when call-related media buttons are pressed. This is a **defensive** use — preventing other apps from intercepting call control buttons during an active call.

**Risk:** LOW — This is standard call handling behavior. No attacker can abuse this since the receiver is dynamically registered (not exported to external apps).

### FirebaseInstanceIdReceiver
Checks `isOrderedBroadcast()` but does not abort — standard FCM behavior.

**No ordered broadcast vulnerability found** where an attacker could abort broadcasts meant for the app.

## 6. Receivers Triggering Sensitive Actions

### BootCompletedReceiver
- Triggers background service rescheduling (sync, wake alarms)
- Does NOT send messages, make calls, or share data

### CallNotifierBroadcastReceiver (not exported)
- Can accept/decline calls, toggle mute, end calls
- Can open incoming call screen, join call links
- **Mitigated:** Not exported; only reachable via PendingIntent from notifications

### BackgroundWakeBootReceiver (exported but disabled)
- Reschedules background wake alarms on boot/package update
- Does NOT perform sensitive actions

## 7. CallNotifierBroadcastReceiver — Call Manipulation Analysis

**Can calls be manipulated?** 

The receiver delegates to `va1.m23482b()` which can:
- **Accept incoming calls** (action matching `wo1`)
- **Decline calls** (action matching `zo1`)
- **End active calls** (action matching `yo1`)
- **Toggle microphone mute** (action matching `xo1`)
- **Open call join links** (action matching `ap1`)

However, **exploitation is not feasible** because:
1. The receiver is **NOT exported** (`android:exported` not set, no intent-filter → defaults to not exported on targetSdk 35)
2. It is only invoked via `PendingIntent.getBroadcast()` created by `CallServiceImpl` for notification actions
3. The PendingIntent is created with flag `201326592` (FLAG_IMMUTABLE | FLAG_UPDATE_CURRENT), preventing intent modification

## Risk Assessment

| Finding | Severity | Exploitable |
|---------|----------|-------------|
| BootCompletedReceiver exported without permission | Low | Partially — can trigger background reschedule |
| BackgroundWakeBootReceiver exported | Info | No — disabled by default |
| CallNotifierBroadcastReceiver processes intent extras | Medium (design) | No — not exported |
| abortBroadcast on MEDIA_BUTTON | Info | No — defensive use |
| FirebaseInstanceIdReceiver processes PendingIntent extra | Low | No — GMS permission protected |

## Recommendations

1. **BootCompletedReceiver** should add a permission requirement or validate the caller, even though the impact is low. Any app can send `BOOT_COMPLETED` to trigger background rescheduling.
2. **CallNotifierBroadcastReceiver** design is secure — not exported, uses FLAG_IMMUTABLE PendingIntents. No changes needed.
3. No high-severity broadcast injection vulnerabilities found. The app properly restricts sensitive receivers from external access.

## Proof of Concept — BootCompletedReceiver Trigger

```bash
# Any app (or adb) can trigger the exported BootCompletedReceiver
adb shell am broadcast -a android.intent.action.BOOT_COMPLETED -n ru.oneme.app/ru.ok.tamtam.android.services.BootCompletedReceiver
```

Impact: Forces background service rescheduling. No data exfiltration or sensitive action triggered.
