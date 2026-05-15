# VULN-38: WebTransport Implementation Vulnerabilities

## Overview

Max messenger implements WebTransport (WT) as an alternative signaling transport for calls, built on the Kwik/Flupke QUIC stack. Controlled by PmsKey `calls-sdk-wt-enabled` and `calls-android-wtp`.

## Key Components

- **WTSignaling** (`ru.ok.android.externcalls.sdk.p033wt.WTSignaling`) — main WT signaling class, extends `tch` (base signaling transport)
- **lpk** — WebTransport socket implementation (QUIC-based bidirectional stream)
- **SignalingTransportBuilder** — decides between WT and WS transport
- **pch (FallbackParams)** — controls WT→WS fallback behavior
- **bch** — custom hostname verifier for WT connections
- **tech.kwik.flupke** — underlying QUIC/HTTP3 library

## Findings

### 1. Custom Hostname Verifier with Bypass Logic (Medium-High)

**Location:** `WTSignaling$nal$1.verify()` and `bch` class

The WT hostname verifier (`WTSignaling$nal$1`) implements custom certificate verification:

```java
public boolean verify(String hostname, X509Certificate certificate) {
    bch bchVar = (bch) this.this$0.getHostnameVerifier();
    String str = (String) bchVar.f5187a.invoke(); // dynamic hostname from callback
    List list = (List) bchVar.f5188b.invoke();    // dynamic IP list from callback
    // If hostname matches dynamic value OR is in IP list, verify against dynamic hostname
    // Otherwise fall through to SAN/CN check
}
```

**Issue:** The hostname verifier accepts connections where the presented hostname matches any entry in a dynamically-provided IP list (`bchVar.f5188b`). If an attacker can influence the IP list (e.g., via server response manipulation), they could redirect WT connections to attacker-controlled servers that present valid certificates for a different domain.

### 2. Downgrade Attack: WT → WS Fallback (Medium)

**Location:** `tch` base class, `pch` FallbackParams

The fallback mechanism is aggressive:
- `FALLBACK_TO_OTHER_TRANSPORT_TIMEOUT = 21000` (21 seconds)
- `FallbackParams.fallbackOnAnyReconnectCase = true` (default)
- `FallbackParams.connectTimeout = 5000` (5 seconds)
- `isFallbackSupported()` returns `true` for WTSignaling

**Attack vector:** An attacker performing a network-level DoS on UDP/QUIC traffic (port blocking) can force the client to fall back from WT to WebSocket (WS). The `prefer_ws` flag in `vch.f73519a` controls this at the `SignalingTransportBuilder.build()` level:

```java
boolean z2 = (z || (str = this.wtEndpointBaseUrl) == null || str.length() == 0) ? false : true;
// If prefer_ws=true OR no WT endpoint, falls back to WS
```

No integrity protection exists to prevent forced downgrade. The fallback is silent with no user notification.

### 3. Separate Endpoint Enables Network Restriction Bypass (Medium)

**Location:** `ConversationParams`, `ApiProtocol.KEY_WT_ENDPOINT`

The server provides separate endpoints:
- `endpoint` / `wse` — WebSocket endpoint (typically WSS on port 443)
- `wt_endpoint` / `wte` — WebTransport endpoint (QUIC/UDP, likely different port)
- `wsip` / `wtip` — separate IP lists for each

**Issue:** WT uses UDP/QUIC on potentially different ports than WS (TCP/443). This means:
- WT traffic may bypass TCP-based firewalls/DPI that only inspect port 443
- The separate `wtip` list allows routing WT through different infrastructure
- Network monitoring tools expecting TCP signaling would miss WT-based call setup

### 4. Token Passed in URL Query Parameters (Low-Medium)

**Location:** `qyl.m19326a()` — endpoint URL builder

Authentication tokens are passed as URL query parameters:
```java
tse.m22296j(ApiProtocol.KEY_TOKEN, str2, arrayList); // token in URL
tse.m22296j("userId", str, arrayList);
tse.m22296j(ApiProtocol.PARAM_CONVERSATION_ID, l86Var.f36252a, arrayList);
tse.m22296j(ApiProtocol.PARAM_PEER_ID, ...);
```

The same token/userId/conversationId are used for both WT and WS endpoints. Token leakage via URL logging, referrer headers, or QUIC connection migration could expose session credentials.

### 5. No Separate TrustManager — Uses Custom Verifier Instead (Medium)

**Location:** `WTSignaling`, `vqb`, `lpk`

Unlike typical TLS implementations, WT does NOT use Android's standard `TrustManager`/`SSLContext`. Instead:
- The `w68` class configures the QUIC connection parameters
- Certificate verification is delegated to the custom `WTSignaling$nal$1.verify()` callback
- The Kwik QUIC library handles TLS 1.3 internally

This means standard Android certificate pinning (NetworkSecurityConfig) is **bypassed** for WT connections. The custom verifier in `bch` does check SAN/CN but relies on dynamic hostname resolution from callbacks rather than pinned certificates.

### 6. `isAvailable()` Always Returns True (Low)

**Location:** `WTSignaling.Companion.isAvailable()`

```java
public final boolean isAvailable() {
    return true; // No runtime capability check
}
```

No actual check for QUIC/UDP availability. Combined with `isWebTransportEnabled()`:
```java
private boolean isWebTransportEnabled() {
    return this.callParams.f22700u.f16980s && WTSignaling.isAvailable();
}
```

The only gate is the server-side `calls-sdk-wt-enabled` PmsKey flag.

### 7. `calls-android-wtp` Parameter Controls Fallback Behavior

**Location:** `PmsKey.f88624callsandroidwtp` (ordinal 126), parsed via `vg6.m23634F(pch)`

This PmsKey configures `FallbackParams`:
- `enableTimeoutBasedFallback` (boolean)
- `timeoutMS` (Long, nullable)
- `fallbackOnAnyReconnectCase` (boolean)
- `connectTimeout` (long, default 5000ms)

Server-controlled fallback parameters mean the server can force clients to always use WS (by setting very short timeouts) or never fall back (by disabling timeout-based fallback), potentially trapping clients on a non-functional WT connection.

## Risk Summary

| # | Issue | Severity | Impact |
|---|-------|----------|--------|
| 1 | Custom hostname verifier with dynamic bypass | Medium-High | MitM on WT connections |
| 2 | Forced WT→WS downgrade via UDP blocking | Medium | Silent protocol downgrade |
| 3 | WT on different port/protocol bypasses network controls | Medium | Firewall/DPI evasion |
| 4 | Auth tokens in URL query parameters | Low-Medium | Token leakage |
| 5 | Bypasses Android NetworkSecurityConfig | Medium | No certificate pinning |
| 6 | No runtime QUIC availability check | Low | Potential connection failures |
| 7 | Server-controlled fallback params | Low | DoS or forced downgrade |

## Recommendations

1. Implement certificate pinning for WT connections independent of Android NSC
2. Add integrity protection to prevent silent WT→WS downgrade (e.g., notify user)
3. Move auth tokens from URL to QUIC stream headers
4. Add mutual authentication or channel binding between WT and WS transports
5. Implement QUIC availability probing before attempting WT connections
