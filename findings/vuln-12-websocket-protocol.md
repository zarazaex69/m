# VULN-12: WebSocket Protocol State Machine Analysis

## Overview

Max messenger uses a dual-layer WebSocket architecture:
1. **Signaling layer** (tch.java/tcf.java) - OkHttp WebSocket for VoIP/call signaling using JSON messages
2. **Binary protocol layer** (pwb.java/owb.java/x5d.java) - Custom binary protocol over raw TCP for the main messaging session

## 1. Main WebSocket Connection Classes

- **tch.java** - Abstract signaling transport (WebSocket connection manager with reconnect logic)
- **tcf.java** - OkHttp WebSocket implementation (RFC 6455 framing)
- **epk.java** - WebSocket frame reader (opcodes 1=text, 2=binary, 8=close, 9=ping, 10=pong)
- **k4k.java** - WebSocket handshake interceptor (Sec-WebSocket-Key, Version 13, permessage-deflate)
- **pwb.java** - Binary session protocol manager (packet sender/reader threads)

## 2. Binary Protocol Handler

**x5d.java** - Packet structure (10-byte header):
```
Byte 0: version (5-10, currently 10)
Byte 1: cmd type (0=server push, 1=response, 2=retry, 3=error)
Bytes 2-3: sequence number (short)
Bytes 4-5: opcode (short)
Bytes 6-9: payload length (lower 24 bits) + compression factor (upper 8 bits)
```

**owb.java** - Packet reader/sender runnable that dispatches based on opcode via massive switch statement in `m17502c()`.

## 3. Message Opcodes (Partial Map from u0d.java)

| Opcode | Name | Description |
|--------|------|-------------|
| 1 | PING | Keep-alive |
| 3 | RECONNECT | Reconnection |
| 6 | SESSION_INIT | Session initialization |
| 16 | PROFILE | Profile data |
| 17 | AUTH_REQUEST | Auth challenge |
| 18 | AUTH | Auth response |
| 19 | LOGIN | Login command |
| 20 | LOGOUT | Logout/session kill |
| 21 | SYNC | Data sync |
| 22 | CONFIG | Configuration push |
| 128 | NOTIF_MESSAGE | Incoming message notification |
| 130 | NOTIF_MARK | Read marker notification |
| 131 | NOTIF_CONTACT | Contact update |
| 134 | NOTIF_CONFIG | **Server config push** |
| 135 | NOTIF_CHAT | Chat update |
| 137 | NOTIF_CALL_START | Incoming call |

## 4. State Transitions - Pre-Auth Message Bug

**States** (pwb.java `f54720c`): 0=DISCONNECTED, 1=CONNECTED, 2=LOGGED_IN

**Critical Finding:** The `mo5169v()` method in AbstractC0830q2 returns `true` (login required) for all commands EXCEPT those implementing `gf0`. The check in owb.java `m17501b()`:

```java
if (c6dVar.f7631b.f4709a.mo5169v() && this.f51589b.f54720c.get() != 2) {
    // Skip - need login
}
```

**BUG:** The `f54728k` (session initialized) flag is checked separately. If `f54728k` is true but state != LOGGED_IN, commands that don't require login (`gf0` subclasses) can be sent. The SESSION_INIT (jsg) has a double-init detection but it only logs and skips - no hard enforcement.

**Pre-auth window:** Between CONNECTED and LOGGED_IN states, the binary socket is open and accepting frames. Server-push messages (cmd=0) are processed regardless of auth state in `m17503d()`.

## 5. Replay Attack Vectors

**Sequence numbers:** Simple incrementing `AtomicInteger` (`f54719b`) cast to `short`. Wraps at 65535.

**Nonce mechanism exists but is weak:**
- Server sends nonce in SESSION_INIT response (ksg.java `f34984z0`, `f34975A0`)
- Client stores expected nonce and validates on LOGIN (tl9.java `f67623e`, `f67624f`)
- NonceException is thrown on mismatch but **only logged, not fatal**: `ct4.m4626M(... "Nonce error" ...)` - execution continues

**Vulnerability:** The nonce check logs an error but does NOT prevent the login packet from being sent. A replayed session could succeed if the server doesn't enforce nonces strictly.

**No per-message HMAC or signature** - packets are not individually authenticated after session establishment.

## 6. Keep-Alive/Ping Mechanism

**Signaling layer (tch.java):**
- Server sends "ping" text, client responds "pong"
- `SERVER_PING_TIMEOUT_MAX = 61000ms`, `SERVER_PING_TIMEOUT_MIN = 11000ms`
- Timeout triggers `CLOSE_SOCKET_CODE_TIMEOUT = 4000` and reconnect

**Binary layer:** Opcode 1 (PING) exists but handling not visible in dispatch.

**Abuse vector:** The `f66952b` activity timeout is calculated as `max(max(j/2, j-60000), 30000)`. An attacker controlling the server response could set `activityTimeout` in the `conversationParams` to an extremely large value, preventing timeout detection and keeping stale connections alive indefinitely.

## 7. Message Ordering Bugs

**Critical Finding:** In `m17503d()`, received packets are dispatched immediately based on sequence number lookup:
```java
a6d a6dVar2 = this.f51589b.f54741x.get(Short.valueOf(x5dVar.f79132c));
```

If `a6dVar2` is null (response for unknown sequence), it's simply logged and dropped. **No reordering buffer exists.**

**Server pushes (cmd=0)** are processed in arrival order with no sequence validation. Out-of-order NOTIF_MESSAGE (128) could cause message display inconsistencies.

**Retry mechanism:** On "service-unavailable" + "recoverable" errors, commands are retried with exponential backoff (`f20035c * 2`, max 30000ms). The retry counter (`f20036d`) maxes at 5 attempts. **Race condition:** If a retry succeeds but the original also arrives late, duplicate processing occurs since the entry is only removed from `f54741x` on first response.

## 8. Server Push Arbitrary Commands

**YES - Server can push arbitrary commands.** When `x5dVar.f79131b == 0` (cmd type = server push):
```java
if (x5dVar.f79131b == 0) {
    m17502c(bArrM17799a, x5dVar, new pb8(...));
}
```

This processes ANY opcode without client request. Notable server-pushable opcodes:
- **NOTIF_CONFIG (134)** - Pushes arbitrary config object (`cb4`) to client
- **LOGOUT (20)** - Forces session termination: `this.f51589b.m18343h(false, false, ...)`
- **CONFIG (22)** - Full configuration replacement
- **NOTIF_CHAT (135)** - Can modify chat state
- **Force update** - `version.force.update.received` preference triggers ForceUpdateScreen

**A compromised or MITM'd server can force logout, push malicious config, or trigger force-update UI.**

## 9. LZ4 Compression - Decompression Bomb

**Compression in x5d.java `m24955c()`:**
```java
this.f79134e = (byte) (i >> 24);  // compression factor stored in upper byte
int i2 = i & 16777215;            // payload length in lower 24 bits
```

**Decompression in owb.java `m17503d()`:**
```java
byte b3 = x5dVar.f79134e;
if (b3 > 0) {
    int i5 = x5dVar.f79136g * b3;  // OUTPUT SIZE = payload_length * compression_factor
    byte[] bArr2 = new byte[i5];
    d49.m4981F().safeDecompressor().decompress(bArrM17799a, 0, i, bArr2, 0, i5);
}
```

**CRITICAL VULNERABILITY:** The decompressed size is `payload_length * compression_factor` where:
- `payload_length` = up to 16,777,215 bytes (24 bits)
- `compression_factor` = up to 127 (signed byte, but checked `> 0`)

**Maximum allocation: 16,777,215 * 127 = ~2.1 GB** from a single packet.

No validation that the decompressed size is reasonable. A malicious server (or MITM) can send a small compressed payload with `compression_factor = 127` and `payload_length` set high, causing OOM crash.

Additionally, `LZ4BlockInputStream` has `MAX_BLOCK_SIZE = 33554432` (32MB) but this is only for the streaming API, not the direct `safeDecompressor().decompress()` call used here.

**Zstd path** (`f79134e == -1`) calls `pag.m17799a()` - separate decompression with unknown bounds.

## Summary of Vulnerabilities

| # | Issue | Severity | Impact |
|---|-------|----------|--------|
| 1 | LZ4 decompression bomb (2.1GB allocation) | HIGH | DoS/OOM crash |
| 2 | Server can push LOGOUT/CONFIG without client consent | HIGH | Remote session kill, config manipulation |
| 3 | Nonce validation is log-only, not enforced | MEDIUM | Session replay possible |
| 4 | No per-message authentication after session setup | MEDIUM | Packet injection by MITM |
| 5 | Pre-auth window accepts server pushes | MEDIUM | Attack surface before authentication |
| 6 | Sequence number is 16-bit wrapping counter | LOW | Collision after 65535 messages |
| 7 | No message reordering buffer for notifications | LOW | UI inconsistency |
| 8 | Activity timeout controllable by server | LOW | Stale connection abuse |
