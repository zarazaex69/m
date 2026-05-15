# VULN-26: BackgroundListenService Analysis

## Summary

`BackgroundListenService` is a foreground service that keeps Max messenger's network connection alive when the app is "closed" (backgrounded). It maintains a persistent WebSocket connection to the server with periodic pings, controlled entirely by server-side PMS (remote config) keys.

## Service Location

- `one.p011me.background.wake.BackgroundListenService` (classes2.dex)
- Controller: `p000.pn0` (implements app lifecycle callbacks)
- Helper: `p000.jil` (start/stop static methods)
- Module: `background-wake_release`

## What Triggers It

### 1. App Goes Background (Primary Trigger)
- `pn0.mo5875f()` — lifecycle callback `onAppGoesBackground`
- Checks `shouldRunInBackground` flag (set by reachability check)
- If true: calls `jil.m11441b()` → `startForegroundService(BackgroundListenService)`

### 2. Boot / App Update Receiver
- `BackgroundWakeBootReceiver` — receives `BOOT_COMPLETED` and `MY_PACKAGE_REPLACED`
- Reschedules the exact alarm if feature is enabled

### 3. Exact Alarm (Periodic Wake)
- `pn0.m18065e()` — schedules `setExactAndAllowWhileIdle` alarm
- Interval: `gn0.f21894b * 60 * 1000` ms (configurable minutes from server)
- Fires `BackgroundCheckReceiver` which re-evaluates whether to start the service

### 4. Reachability Check (on0 coroutine)
- `t48` result: `shouldRun = !pushReachable && oneMeReachable`
- If push delivery is unreliable but OneMe server is reachable → starts foreground service
- This means: **if Google FCM push is blocked (common in Russia/China), the service runs permanently**

## What It Does in Background

### Foreground Service (notification ID 9001)
- Shows persistent notification ("background wake" channel)
- Registers process flag `64L` (bit 6) in the process tracker (`u6e`)

### Maintains WebSocket Connection
- The `hod` coroutine (ping scheduler) runs in a loop:
  - If app is **interactive**: uses normal `f28253c` ping interval
  - If app is **NOT interactive** but `pingBackgroundInterval > 0`: uses background ping interval
  - Sends keepalive pings to maintain the WebSocket connection to server

### Schedules Exact Alarms
- Uses `AlarmManager.setExactAndAllowWhileIdle()` (ELAPSED_REALTIME_WAKEUP = type 2)
- Wakes device from Doze mode periodically

## PmsKey: `keep-background-socket`

- PmsKey ordinal: 302 (HTTP 302)
- Parsed into: `gn0` (Enabled) or `fn0` (Disabled)
- `gn0` fields:
  - `f21894b` = `checkBackgroundIntervalMinutes` — alarm repeat interval
  - `f21895c` = `suggestionIntervalMinutes` — how often to suggest enabling
  - `f21896d` = `checkForegroundIntervalSec` — foreground check interval
- **Server can enable/disable this feature remotely** via PMS config push
- `jn0` observer: if PMS changes to disabled (`fn0`), force-disables the feature

## PmsKey: `ping-background-interval`

- PmsKey ordinal: 303 (HTTP 303)
- Exposed as `qp6.getPingBackgroundInterval()` → returns `long` (milliseconds)
- Used in `hod` ping scheduler: when app is not interactive and interval > 0, pings at this rate
- **Server controls how frequently the background connection pings** — can be set very aggressively

## PmsKey: `spin-lock-enabled`

- PmsKey ordinal: 304 (HTTP 304)
- Exposed as `qp6.isSpinLockEnabled()` → boolean
- Description: "Enable SpinLock in concurrency" (default: FALSE)
- Implementation: `sjf` class — a `ReentrantSpinLock` using `AtomicBoolean` + `Thread.yield()` busy-wait loop
- Used in: `itg`, `uq5`, `je4`, `pwb` — database/concurrency primitives
- **Does NOT directly prevent CPU sleep** — it's a userspace spin lock for thread synchronization
- However: if enabled during background operation, `Thread.yield()` loops consume CPU cycles instead of blocking

## Surveillance Assessment

### No Direct Surveillance Capabilities Found
- **No AudioRecord/MediaRecorder** usage in background-wake package
- **No LocationManager** or location tracking
- **No Camera** access
- The service's sole purpose is maintaining network connectivity

### Indirect Surveillance Concern
- The persistent connection means the **server can push commands at any time**
- Combined with other app capabilities, this provides a reliable command channel
- The server controls whether this runs and at what frequency — user has limited control
- Analytics events tracked: `carpet_mode_on`, `system_curtain_shown/hidden`, `work_in_background_permission`

## Battery & Wakelock Impact

### No PowerManager.WakeLock in BackgroundListenService
- The service does NOT acquire a traditional `PowerManager.WakeLock`
- It relies on Android's foreground service guarantee to stay alive
- Process tracking via `u6e` (AtomicIntegerArray bit flags) — logical tracking only, not a wakelock

### Battery Impact Vectors
1. **Exact alarms with `setExactAndAllowWhileIdle`** — bypasses Doze mode, wakes CPU
2. **Foreground service** — prevents process death, keeps CPU scheduling active
3. **WebSocket keepalive pings** — prevents radio from entering idle state
4. **Spin-lock (if enabled)** — busy-wait loops waste CPU cycles vs. blocking locks
5. **`wakelock-on-push`** PmsKey exists separately (ordinal 32) — used for push processing

### Wakelock Elsewhere
- `CallServiceImpl`: acquires `PARTIAL_WAKE_LOCK` tag `"max:calls_prx"` (calls only)
- `koe`: acquires proximity wake lock tag `"max:proximity_helper"` (calls only)
- `p3l`: force-releases wakelocks on timeout with log "IS FORCE-RELEASED ON TIMEOUT"

## Risk Assessment

| Factor | Rating | Notes |
|--------|--------|-------|
| Battery drain | **HIGH** | Exact alarms + foreground service + WebSocket pings bypass all OS power saving |
| Server control | **HIGH** | All intervals and enable/disable controlled remotely via PMS |
| User transparency | **MEDIUM** | Shows notification, but user doesn't understand implications |
| Surveillance enabler | **MEDIUM** | No direct spying, but provides always-on command channel |
| Spin-lock CPU waste | **LOW** | Only affects concurrency primitives, not directly tied to background wake |

## Key Concern

The entire background-wake system is **server-controlled**. The server decides:
1. Whether it's enabled (`keep-background-socket` → gn0 vs fn0)
2. How often to wake the device (alarm interval in minutes)
3. How often to ping in background (`ping-background-interval`)
4. Whether spin-locks burn CPU (`spin-lock-enabled`)

The user can toggle "work in background" permission, but the app can re-prompt via `suggestionIntervalMinutes`. The decision logic (`!pushReachable && oneMeReachable`) means in environments where FCM is unreliable, this runs **permanently** — which is the default state for many Russian users.
