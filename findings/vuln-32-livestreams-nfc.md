# VULN-32: Live Streams Infrastructure & NFC Service Analysis

## Date: 2026-05-15
## Component: Live Streams (RTMP), WebAppNfcService (HCE)
## Package: ru.oneme.app (Max Messenger)

---

## 1. Live Streams Infrastructure

### 1.1 RTMP Client

**Library:** `io.antmedia.rtmp_client.RtmpClient` — native JNI library (`librtmp-jni.so`)

**Key class:** `p000.zyf` (extends Media3 DataSource `or0`)
- Opens RTMP connection via `nativeOpen(String url, false, handle, 10000, 10000)`
- URL passed directly from `y55Var.f82202a.toString()` (a `Uri` object)
- No stream key sanitization or redaction observed in the data source layer
- No explicit logging of the RTMP URL in `zyf.java` itself

### 1.2 Live Stream URL Construction

**PmsKey:** `live-streams-url-prefix` (enum ordinal 315, field `f88730livestreamsurlprefix`)

**Usage in `qp6.java`:**
```java
this.f57404L1 = new po6(this, PmsKey.f88730livestreamsurlprefix, i11);
```
Exposed as property: `getLiveStreamsUrlPrefix()` returning `String`.

**Usage in `c60.java` (line 559):**
```java
if (w0i.m24048k0(strM362b, (String) qp6Var.f57404L1.mo677x(...), false)) {
    z = true; // marks stream as "live" if URL starts with the configured prefix
}
```

The `live-streams-url-prefix` is a **server-configured string** fetched via PMS (Platform Management Service). The actual RTMP server URL is not hardcoded — it's delivered dynamically from the backend. The URL prefix is used to identify whether a media attachment URL belongs to a live stream.

### 1.3 RTMP Key Exposure Assessment

| Vector | Status |
|--------|--------|
| Logged in logcat | **Not directly** — `zyf.java` has no logging calls. However, the URL is passed as a plain `Uri` object. |
| Exposed in intents | **Not observed** — RTMP connection is internal to the media player pipeline. |
| Debug mode risk | The PmsKey `log-sensitive` exists — if enabled, sensitive data including stream URLs could be logged. |
| Stream key in proto | The `LiveStream` proto only contains `updateTime` and `media` (Attach) — no explicit `streamKey` field. The key is likely embedded in the RTMP URL path. |

**Risk:** MEDIUM — The RTMP stream key is embedded in the URL and passed through the media pipeline without explicit redaction. If `log-sensitive` or `log-full` PMS flags are enabled, the full RTMP URL (including stream key) could appear in logs.

### 1.4 LiveStream Proto Structure

```protobuf
message LiveStream {
    int64 updateTime = 1;
    Attaches.Attach media = 2;  // Contains the stream URL
}
```

---

## 2. WebAppNfcService (NFC HCE)

### 2.1 Service Architecture

**Class:** `one.me.webapp.util.WebAppNfcService extends HostApduService`
**Component name:** `ComponentName("ru.oneme.app", WebAppNfcService.class.getName())`
**Module:** `web-app_release`

This is a **Host Card Emulation (HCE)** service that allows the device to emulate an NFC card. It is controlled by WebApp (mini-app/bot) bridge.

### 2.2 NFC Operations Available to Bots

Three operations identified via command classes:

| Class | Operation | Description |
|-------|-----------|-------------|
| `dxb` | **Start NFC / Set Data** | Fields: `queryId` (String), `data` (String) — sets APDU response data |
| `exb` | **Stop NFC** | Field: `queryId` (String) — disables HCE and clears data |
| `fxb` | **Check NFC Status** | Field: `queryId` (String) — checks if NFC adapter is available/enabled |

### 2.3 Data Flow (Bot → NFC Card Emulation)

```
Bot (WebApp JS) → Bridge → oek controller → jxb.f32337d.set(data.getBytes())
                                           → jxb.f32335b = TRUE (enable)
                                           → WebAppNfcService.processCommandApdu() returns stored bytes
```

1. Bot sends `dxb(queryId, data)` via JS bridge
2. Controller `oek` validates queryId matches current webapp session
3. Data is stored in `AtomicReference<byte[]>` (`jxb.f32337d`)
4. NFC service enabled flag set to `TRUE` (`jxb.f32335b`)
5. When external NFC reader sends SELECT APDU (INS=0xA4), service returns the stored bytes
6. Service emits `lxb.SUCCESS` event back to bot

### 2.4 APDU Logging (Information Disclosure)

**CRITICAL:** In debug mode (`ct4.m4657b()`), the service logs raw APDU commands:
```java
if (ct4.m4657b()) {
    ymcVar.m26051c(vk9Var2, name, "APDU received: ".concat(rz7.m20769h(bArr)), null);
}
```
This logs the full hex-encoded APDU data received from external NFC readers.

### 2.5 Relay Attack Assessment

**Can a bot relay NFC commands to a remote server?**

**YES — Architecturally possible.** The flow is:

1. Bot registers HCE service with arbitrary response data
2. External NFC reader taps phone → APDU arrives at `processCommandApdu()`
3. Service returns pre-set data from `AtomicReference`
4. Bot receives SUCCESS/FAILED event via `k6h` state flow (`f32338e`)

**However, the current implementation has limitations:**
- The response data is **pre-set** (not dynamically computed per APDU)
- Only SELECT (INS=0xA4) commands return the bot's data; other INS codes return error `{0x6F, 0x00}`
- There is **no callback to the bot per APDU** — the bot cannot see incoming APDUs in real-time and respond dynamically

**Partial relay risk:** A bot could pre-load a static NDEF or AID-select response, effectively cloning a static NFC card. Full real-time relay (where each APDU is forwarded to a remote server and response relayed back) is **not possible** with the current architecture because there's no per-APDU callback to the WebApp.

### 2.6 User Consent

**NO per-operation consent dialog found.**

Analysis:
- No permission dialog, confirmation sheet, or user prompt before NFC HCE activation
- The only validation is `oek.m15849b()` which checks that the `queryId` matches the current webapp session ID
- The NFC adapter availability is checked (`nfcAdapter.isEnabled()`)
- The component is enabled/disabled via `PackageManager.setComponentEnabledSetting()` — but this is programmatic, not user-facing
- The service is registered in the manifest with `apduservice` XML resource

**Risk:** A malicious mini-app (bot) can silently activate NFC card emulation with arbitrary data without any user confirmation beyond having the mini-app open.

---

## 3. Security Findings Summary

| # | Finding | Severity | Component |
|---|---------|----------|-----------|
| 1 | RTMP stream key embedded in URL without redaction | MEDIUM | Live Streams |
| 2 | `live-streams-url-prefix` server-configurable — no client-side validation of RTMP target | LOW | Live Streams |
| 3 | NFC HCE activated by bots without user consent | HIGH | WebAppNfcService |
| 4 | Raw APDU data logged in debug mode | MEDIUM | WebAppNfcService |
| 5 | Static NFC card cloning possible via bot-supplied data | HIGH | WebAppNfcService |
| 6 | No rate limiting on NFC data updates | LOW | WebAppNfcService |
| 7 | Only queryId session validation — no origin/permission check | MEDIUM | WebAppNfcService |

---

## 4. Recommendations

1. **NFC consent:** Require explicit user confirmation (system dialog) before activating HCE from a mini-app
2. **NFC data validation:** Restrict what data bots can set as APDU responses (e.g., block payment AIDs)
3. **RTMP URL redaction:** Strip stream keys from any logged URLs; use token-based auth with short TTL
4. **Debug logging:** Ensure APDU logging is completely disabled in release builds (not just gated by debug flag)
5. **NFC session binding:** Tie NFC activation to foreground activity state; auto-disable when mini-app loses focus
