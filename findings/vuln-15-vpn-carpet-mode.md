# VPN Detection, Carpet Mode & GOST — Max Messenger v26.15.3

## VPN Detection Mechanism

### Detection (jf4.java — ConnectionInfoNougatImpl)

```java
// p000/jf4.java line 219
public final boolean mo2504e() {  // isVpnConnected()
    gf4 gf4Var = (gf4) this.f30662o.get();
    if (gf4Var != null && this.f30653f.get()) {
        return gf4Var.f21267e;  // cached VPN state
    }
    NetworkCapabilities caps = m11380j(this);
    if (caps != null) {
        return caps.hasTransport(4);  // TRANSPORT_VPN = 4
    }
    return false;
}
```

Detection uses Android's `NetworkCapabilities.hasTransport(TRANSPORT_VPN)` — standard API, no root needed.

### State Reporting (jf4.java:424-430)

```java
gf4 gf4Var = new gf4(
    z2,           // hasInternet
    ...,          // isMetered
    ...,          // isRoaming
    z,            // isConnected
    networkCapabilitiesM11380j.hasTransport(4)  // isVpn ← field f21267e
);
String str3 = mo2504e() ? "(VPN detected)" : "";
```

The VPN state is stored in `gf4.f21267e` and propagated to all network state observers.

### VPN Warning Triggers

Three server-controlled PmsKey flags control VPN warnings:
- `show-vpn-chat-bottomsheet` → VpnConnectedWarningBottomSheet in chat
- `show-vpn-call-bottomsheet` → VpnConnectedWarningBottomSheet before calls
- `show-vpn-snackbar` → Snackbar notification

### Warning UI (VpnConnectedWarningBottomSheet.java)

Shown when:
1. User tries to make a call AND VPN is detected
2. Server flag `show-vpn-call-bottomsheet` is enabled

Triggered from:
- `ContactListWidget.java:451` — contact list call button
- `CallHistoryScreen.java:400,469` — call history redial
- `vwf.java:646` — call initiation flow

### BAD_CONNECTION_ALERT (b92.java:49)

```java
ob2.m15756l(ob2Var, "BAD_CONNECTION_ALERT", str, "VPN", null, null, null, z, null, 376);
```

When VPN state changes, a `BAD_CONNECTION_ALERT` with reason `"VPN"` is sent to analytics. This is a telemetry event — the server knows when you're on VPN.

---

## Carpet Mode (Background Keep-Alive)

### Purpose
"Carpet mode" is a background service that keeps the app alive for host reachability checks.

### Implementation (on0.java:77)

```java
if (z3) {  // shouldRun = push enabled AND app not in foreground
    ct4.m4610E("KeepBackground", "reachabilityCheck: ENTERING foreground");
    ok9.m16044h(pn0Var.f53859Y.m21369a(), "BACKGROUND_MODE", "carpet_mode_on", null, 12);
    jil.m11441b(pn0Var.f53861a);  // start BackgroundListenService
} else {
    ct4.m4610E("KeepBackground", "reachabilityCheck: EXITING foreground");
    jil.m11442c(pn0Var.f53861a);  // stop service
}
```

### Trigger Conditions (t48)
```java
// t48.m21647c() = shouldRun
boolean push = t48Var.f66293a;   // push notifications enabled
boolean oneMe = t48Var.f66294b;  // oneMe reachability flag
boolean shouldRun = t48Var.m21647c();
```

### Host Reachability
- PmsKey: `host-reachability` — server-controlled config
- PmsKey: `isHostReachabilitySendEnabled` — enables sending results to server
- The app pings specific hosts to check if they're reachable
- Results are sent back to VK servers
- **This is censorship infrastructure** — checking which sites are blocked from user's network

---

## GOST (Russian State Crypto Standard)

### Server Config Flags
- `gost-check-env` (PmsKey, integer flags) → `gostEnvironmentCheckFlags` in qp6.java
- `gostLicenseCheckEnabled` (local pref in ri9.java) → boolean

### Implementation Status

```java
// p000/pr4.java:154
sbM25894y.append("\n    UseNarnia: false\n    Gost: false\n    UsePersonalCloud: false\n    BuildTime: ");
```

**GOST is currently DISABLED** (`Gost: false` in build config). However:
1. The `gost-check-env` flag exists as a server-controlled integer (bitmask)
2. `gostLicenseCheckEnabled` preference exists in local storage
3. The infrastructure is ready to be enabled remotely

### C0528ke.java:1466 — GOST String Reference
```java
str2 = "GOST";
```
Used in connection type reporting — when GOST mode is active, connections are labeled.

### Implications
- GOST crypto can be enabled remotely via server config
- When enabled, likely forces Russian state-approved crypto algorithms
- May enable government key escrow or lawful interception
- The "license check" suggests it requires a GOST crypto license/certificate

---

## VPN Detection Bypass

### Method 1: Disable Server Flags
The VPN warning is entirely server-controlled. If you can modify PmsKey values:
- Set `show-vpn-chat-bottomsheet` = false
- Set `show-vpn-call-bottomsheet` = false  
- Set `show-vpn-snackbar` = false

### Method 2: Patch jf4.java
Replace `hasTransport(4)` check to always return false:
```smali
# In jf4.smali, find invoke-virtual hasTransport(4)
# Replace with const/4 v0, 0x0 (always false)
```

### Method 3: Network Stack
Use a VPN that doesn't register as TRANSPORT_VPN:
- Proxy-based solutions (HTTP/SOCKS proxy)
- DNS-over-HTTPS without VPN transport
- Tor with Orbot in proxy mode (not VPN mode)

### Method 4: Xposed/Frida Hook
```javascript
// Frida hook to bypass VPN detection
Java.perform(function() {
    var NetworkCapabilities = Java.use("android.net.NetworkCapabilities");
    NetworkCapabilities.hasTransport.implementation = function(transport) {
        if (transport === 4) return false;  // TRANSPORT_VPN
        return this.hasTransport(transport);
    };
});
```

---

## Summary

| Feature | Status | Control |
|---------|--------|---------|
| VPN Detection | ACTIVE | `hasTransport(4)` API |
| VPN Warning UI | Server-controlled | 3 PmsKey flags |
| VPN Telemetry | ACTIVE | BAD_CONNECTION_ALERT sent to server |
| Carpet Mode | ACTIVE | Background service for reachability |
| Host Reachability | Server-controlled | Pings hosts, reports to VK |
| GOST Crypto | DISABLED (ready) | `gost-check-env` bitmask |
| GOST License | DISABLED (ready) | `gostLicenseCheckEnabled` pref |

**Privacy Impact**: VK knows when you use a VPN, which hosts are reachable from your network, and can remotely enable Russian state crypto. The app is designed as a surveillance-ready platform with kill switches for censorship compliance.
