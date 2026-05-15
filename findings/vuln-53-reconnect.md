# VULN-53: WebSocket Reconnection & Session Resumption Analysis

## Summary

Max messenger uses a WebSocket-based signaling transport (`tch` class) with token-based reconnection and timestamp-based message recovery (`recoverTs`). The reconnection mechanism reuses the original session token appended to the URL, creating potential replay and hijack attack surfaces.

## 1. Grep Results (reconnect/session patterns)

```
ru/ok/android/externcalls/sdk/events/HangupHint.java:5:    SHOULD_RECONNECT
ru/ok/android/externcalls/sdk/wt/WTSignaling.java:163:  tch.access$resetReconnectContext(tchVar);
ru/ok/android/externcalls/sdk/wt/WTSignaling.java:164:  tch.access$resetReconnectDelay(tchVar);
ru/ok/android/externcalls/sdk/wt/WTSignaling.java:216:  mch("webtransport_restart", "webtransport_connected", "webtransport_reconnected", ...)
p000/uw7.java:30:  RECONNECTION state enum (DIAL, NOT_CONTACT_DIAL, ACTIVE, RECONNECTION)
p000/fv1.java:30:  Timeouts(timeoutIceReconnectMillis=30000, signalingMaxRetryCount=5, ...)
p000/jol.java:156: RECONNECTION_TIMED_OUT_DURING_UPDATE
p000/jol.java:158: RECONNECTION_TIMED_OUT
p000/sch.java:41:  Timeouts(connectTimeout, initialReconnectDelay, reconnectDelayScaleFactor, maxReconnectDelay)
p000/gb2.java:51:  CallsSignalingTimeouts(initialReconnectDelay=2000, reconnectDelayScaleFactor=1.0, maxReconnectDelay=2000)
```

## 2. Reconnection Auth Mechanism

**No full re-authentication on reconnect.** The system uses a token-based resume approach:

### Endpoint URL Construction (`och.m15802b` / `qyl.m19326a`):
- `EndpointParameters` (`l86` class) contains: `conversationId`, `token`, `userId`, `deviceIdx`, `peerId`, `endpointBaseUrl`, `endpointIPs`, `recoverTs`
- The WebSocket URL is built with the token as a query parameter: `appendQueryParameter(ApiProtocol.KEY_TOKEN, l86Var.f36253b)`

### On Reconnect (`tch.m21750a` - restart method):
```java
String strM15803c = och.m15803c(str2, ApiProtocol.KEY_TOKEN, str);  // reuse token
strM15803c = och.m15803c(strM15803c, "userId", String.valueOf(l.longValue()));
strM15803c2 = och.m15803c(strM15803c, "tgt", URL_TYPE_RETRY);  // mark as retry
strM15803c2 = och.m15803c(strM15803c2, "recoverTs", String.valueOf(j));  // recovery timestamp
```

### On Simple Reconnect (`tch.m21748a`):
```java
// Only appends recoverTs to existing URL, no new token generation
strM15803c = och.m15803c(strM15803c, "recoverTs", strValueOf);
```

**Finding:** The same token is reused across reconnections. No token rotation occurs. The `tgt=retry` parameter distinguishes reconnections from initial connections.

## 3. Replay Attack Vulnerability

### Risk: MEDIUM-HIGH

**Token Replay:**
- The WebSocket connection URL contains the full auth token as a query parameter
- On reconnection, the same token is reused with only `tgt=retry` and `recoverTs` added
- If an attacker captures the WebSocket URL (e.g., via logs, proxy, or memory dump), they can replay it to establish a new connection

**Timestamp-based Recovery (`recoverTs`):**
- The `stamp` field from server messages is tracked: `this.f66971u = Math.max(jOptLong, this.f66971u)`
- On reconnect, `recoverTs` is sent to recover missed messages
- An attacker replaying an old URL with `recoverTs=0` could receive all messages since the session began

**Mitigating Factors:**
- Token likely has server-side expiry (not visible in client code)
- TLS protects the URL in transit
- The `net-ssl-session-validate` PmsKey suggests SSL session validation exists

## 4. PmsKey Findings

### `disconnect-timeout` (PmsKey index 28)
- **Location:** `PmsKey.java:112` → `f88677disconnecttimeout`
- **Usage:** `vei.java:161` - Controls how long the client waits before disconnecting an idle session
- **Default:** 300 (seconds, from `HttpStatus.SC_MULTIPLE_CHOICES` = 300)
- **Logic:** If elapsed time since last activity exceeds `disconnect-timeout` and no pending tasks, the session is disconnected

### `reconnect-call-ringtone` (PmsKey index 96)
- **Location:** `PmsKey.java:314` → `f88831reconnectcallringtone`
- **Purpose:** Controls ringtone behavior during call reconnection (the `RECONNECTION` state in `uw7` enum)
- **No direct usage found** outside PmsKey definition and configuration propagation

## 5. Message Loss/Duplication During Reconnection

### Message Loss Window: EXISTS

**Recovery Mechanism:**
- Server sends `stamp` (timestamp) with each message
- Client tracks max stamp: `this.f66971u = Math.max(jOptLong, this.f66971u)`
- On reconnect, `recoverTs` is sent to request messages since that timestamp
- If `isFastRecoverEnabled` is false, `recoverTs` is NOT sent → **messages during disconnect are lost**

**Duplication Risk:**
- The `recoverTs` is the last received timestamp; server replays from that point
- If the client processed a message but crashed before updating `f66971u`, the message will be re-delivered on reconnect
- No client-side deduplication mechanism visible in the transport layer

**Reconnect Delay Window:**
- Default reconnect delay: 2000ms (`RECONNECT_DELAY_MILLIS`)
- Configurable via `sch` timeouts: `initialReconnectDelay`, `reconnectDelayScaleFactor`, `maxReconnectDelay`
- With exponential backoff + jitter, gaps can grow significantly

## 6. Server-Forced Reconnection to Different Endpoint (Redirect Attack)

### Risk: MEDIUM

**Endpoint Failover Mechanism (`och.m15801a`):**
- On `UnknownHostException` or `ConnectException`, the client cycles through `endpointIPs` list
- The authority (host:port) is replaced with the next IP in the list: `uriBuild.buildUpon().encodedAuthority((String) list.get(i)).build()`

**Server-Initiated Peer/Conversation Update:**
- When server sends `type=notification, notification=connection`, the client updates its endpoint:
```java
this.f66973w = new x0l(strOptString5, lM23273c0);  // new conversationId, peerId
l86 l86VarM13116a = l86.m13116a(this.f66959i, strOptString5, lM23273c0);
this.f66969s = och.m15802b(l86VarM13116a);  // rebuild URL with new params
```
- **This allows the server to redirect the client to a different conversation/peer without client validation**

**`redirectHost` in `fef` class:**
- Parses `redirectHost` field (format: `host:port`)
- Extracts host and port separately via `m7412d()` and `m7413e()`
- Used with TLS flag (`this.f18433d = kxaVar.m12869L0()`)
- **A compromised or MITM'd server could redirect to attacker-controlled endpoint**

**Fallback Transport:**
- `FALLBACK_TO_OTHER_TRANSPORT_TIMEOUT = 21000ms` - after 21s, client may fall back to another transport
- `MSG_REQUEST_FALLBACK` message triggers transport switch

## 7. `net-session-suppress-bad-disconnected-state` PmsKey

- **Location:** `PmsKey.java:753` → `f88776netsessionsuppressbaddisconnectedstate` (index 245)
- **Type:** Boolean
- **Usage:** `ubi.java:73,144` - Read from server configuration and propagated as a session parameter
- **Purpose:** When enabled, suppresses reporting/handling of "bad disconnected" states
- **Security Implication:** If enabled, the client may not properly detect or report abnormal disconnections (e.g., from a MITM forcing a disconnect). This could mask session hijacking attempts where an attacker forces a disconnect to steal the session token during the reconnection window.

**Related PmsKeys:**
- `net-ssl-session-validate` - SSL session validation (adjacent in code)
- `net-session-rbc-enabled` - Session RBC (reconnect-before-close?) feature flag

## Vulnerability Summary

| Issue | Severity | Description |
|-------|----------|-------------|
| Token reuse on reconnect | Medium-High | Same token used across all reconnections without rotation |
| Token in URL query params | Medium | Susceptible to logging, referrer leaks, proxy capture |
| Server-forced redirect | Medium | Server can redirect client to different endpoint/conversation |
| `recoverTs` replay | Medium | Old timestamps can be replayed to receive historical messages |
| Suppress bad disconnect | Low-Medium | Can mask hijacking-induced disconnections |
| Message loss window | Low | Gap between disconnect and reconnect loses messages if fast-recover disabled |
| Message duplication | Low | No client-side dedup on recovery replay |

## Recommendations

1. Implement token rotation on each reconnection (server issues new short-lived token on connect)
2. Move token from URL query parameter to WebSocket subprotocol or first-message auth
3. Validate `redirectHost` against a pinned allowlist of known server endpoints
4. Add client-side message deduplication using message IDs
5. Ensure `net-session-suppress-bad-disconnected-state` defaults to false in production
6. Add server-side rate limiting on `recoverTs=0` requests to prevent bulk message retrieval
