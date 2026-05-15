# VULN-110: Max Messenger Custom Binary Protocol Deep Analysis

**Date:** 2026-05-15
**Source:** ~/max/decompiled/jadx/base/sources/p000/

## 1. Protocol Architecture

### Core Classes
- **`pwb.java`** — Session manager. Handles connection state, packet queue, nonce validation, sequence numbers.
- **`owb.java`** — Packet sender (Runnable) and response handler. Entry points: `m17501b()` (send), `m17503d()` (receive), `m17502c()` (dispatch by opcode).
- **`x5d.java`** — Packet structure. Serialization/deserialization of the binary wire format.
- **`u0d.java`** — Opcode enum registry (161 opcodes mapped by name and short value).
- **`tch.java`** — WebSocket signaling transport with ping/pong keepalive (separate from binary protocol).

### Data Flow
```
WebSocket binary frame
  → pwb.f54715L.mo8858j() reads 10-byte header
  → new x5d(bArr) parses header
  → payload read in 256-byte chunks
  → decompression (LZ4 or Zstd)
  → owb.m17502c() dispatches by opcode (s2 = x5dVar.f79133d)
```

## 2. Packet Format (10-byte header + payload)

```
Offset  Size   Field           Notes
0       1      version         Valid range: 5–10 (current: 0x0A = 10)
1       1      cmd             0=push/notification, 1=response, 2=retry, 3=error
2       2      seq             Sequence number (short), monotonically incrementing
4       2      opcode          Command identifier (short)
6       4      size_and_flags  Upper byte = compression flag, lower 3 bytes = payload length
10+     N      payload         Protobuf-serialized data
```

### Compression Flags (byte at offset 6, upper bits of the 4-byte int)
- `0x00` — No compression (payload < 32 bytes)
- `> 0` — LZ4 compressed. Value = `(originalSize / compressedSize) + 1` (ratio hint for decompression buffer allocation: `payloadLength * compressionFlag`)
- `0xFF (-1)` — Zstd compressed (decompressed via `pag.m17799a()`)

### Encryption
**None at the application protocol layer.** The binary protocol relies entirely on TLS (WSS) for transport encryption. No per-packet encryption, MAC, or signing is present. Payload is raw protobuf after decompression.

## 3. Complete Opcode Table (Selected Key Opcodes)

| Opcode | Name | Category |
|--------|------|----------|
| 1 | PING | Keepalive |
| 3 | RECONNECT | Session |
| 6 | SESSION_INIT | Session |
| 16 | PROFILE | User |
| 17 | AUTH_REQUEST | Auth |
| 18 | AUTH | Auth |
| 19 | LOGIN | Auth |
| 20 | LOGOUT | Auth |
| 21 | SYNC | Data |
| 23 | AUTH_CONFIRM | Auth |
| 32 | CONTACT_INFO | Contacts |
| 43 | REMOVE_CONTACT_PHOTO | Contacts |
| 48–63 | CHAT_* | Chat ops |
| 64 | MSG_SEND | Messaging |
| 65 | MSG_TYPING | Messaging |
| 66 | MSG_DELETE | Messaging |
| 67 | MSG_EDIT | Messaging |
| 70 | MSG_SHARE_PREVIEW | Messaging |
| 72–74 | MSG_SEARCH* | Search |
| 80 | PHOTO_UPLOAD | Media |
| 96 | SESSIONS_INFO | Session mgmt |
| 97 | SESSIONS_CLOSE | Session mgmt |
| 118 | MSG_SEND_CALLBACK | Bot |
| 128 | NOTIF_MESSAGE | Push notification |
| 129 | NOTIF_TYPING | Push notification |
| 130 | NOTIF_MARK | Push notification |
| 131 | NOTIF_CONTACT | Push notification |
| 132 | NOTIF_PRESENCE | Push notification |
| 178 | MSG_REACTION | Reactions |
| 179 | MSG_CANCEL_REACTION | Reactions |
| 193 | STICKER_CREATE | Stickers |

Total: **161 registered opcodes** in `u0d.f69055B3` array.

## 4. Sequence Number & Replay Analysis

### Sequence Number Mechanism
- `pwb.f54719b` — `AtomicInteger`, incremented per outgoing packet (`atomicInteger2.incrementAndGet()`)
- Truncated to `short` for wire format (wraps at 65535)
- Response matching: `pwb.f54741x` (ConcurrentHashMap<Short, a6d>) maps seq→pending request

### Anti-Replay: **WEAK/ABSENT**
- **No server-side sequence validation visible in client code.** The client only uses seq to match responses to requests.
- **Connection number check exists** (`f54732o` — connection generation counter). Packets with wrong connection number are rejected with "wrong connection number" error. This prevents cross-connection replay but NOT within the same connection.
- **No timestamp validation** on incoming packets.
- **No HMAC/signature** on packets — an attacker on the wire (if TLS is broken/stripped) can inject arbitrary packets.

**Vulnerability:** Within a single WebSocket connection, there is no mechanism preventing replay of server→client push notifications (cmd=0). An attacker who can MITM the TLS connection can replay old NOTIF_MESSAGE packets.

## 5. Keepalive / Ping Mechanism

### Binary Protocol Level (opcode 1 = PING)
- Opcode 1 is registered as "PING" in `u0d.f69120X`
- Server sends push packets with opcode 1; client processes them

### WebSocket Signaling Level (`tch.java`)
- Text-based ping/pong: server sends `"ping"` string, client responds `"pong"`
- **Timing constants exposed:**
  - `SERVER_PING_TIMEOUT_MIN = 11000ms`
  - `SERVER_PING_TIMEOUT_MAX = 61000ms`
  - `RECONNECT_DELAY_MILLIS = 2000ms`
  - `FALLBACK_TO_OTHER_TRANSPORT_TIMEOUT = 21000ms`
- `f66970t` tracks last pong time via `SystemClock.elapsedRealtime()`
- If `elapsedRealtime - lastPongTime > timeout` → connection considered dead

### Timing Attack Surface
- Ping/pong is plaintext `"ping"`/`"pong"` over WebSocket text frames
- Fixed timeout windows (11s–61s) allow an attacker to:
  1. Measure connection liveness by observing ping intervals
  2. Force reconnection by blocking pong responses for >61s
  3. Trigger fallback transport after 21s of failure

## 6. Post-Handshake Authentication

### Session State Machine
```
DISCONNECTED(0) → CONNECTED → SESSION_INIT(opcode 6) → LOGIN(opcode 19) → LOGGED_IN(state=2)
```

### Nonce Validation
- `pwb.f54721d` through `f54724g` — volatile Long fields storing nonce values
- `NonceException` thrown if login nonce doesn't match expected values
- Nonce checked at send time (`owb.java:127`): compares `tl9Var.f67623e` with `pwb.f54721d`

### Mid-Session Authentication: **NONE**
- After LOGIN succeeds (`f54720c.get() == 2`), no further authentication occurs on individual packets
- `mo5169v()` check: some commands require logged-in state, but this is a local client-side check only
- **No per-packet auth token or session binding** in the binary wire format

**Vulnerability:** If TLS is compromised, an attacker can inject arbitrary packets mid-session. The protocol has no mechanism to authenticate individual packets after the initial login handshake. The `cmd` byte and `seq` number provide no cryptographic binding.

## 7. Summary of Vulnerabilities

| # | Finding | Severity | Impact |
|---|---------|----------|--------|
| 1 | No per-packet encryption/MAC | High | Full packet injection if TLS compromised |
| 2 | No anti-replay within connection | Medium | Replay of push notifications possible |
| 3 | Sequence number is 16-bit, wrapping | Low | Seq collision after 65535 packets |
| 4 | No post-handshake packet authentication | High | MITM can inject commands mid-session |
| 5 | Predictable ping timing (11–61s windows) | Low | Connection state fingerprinting |
| 6 | Compression oracle potential (LZ4/Zstd) | Medium | CRIME-style attacks if attacker controls partial plaintext |
| 7 | Unknown opcode causes session error propagation | Low | DoS via malformed opcode injection |

## 8. Key Code References

- Packet construction: `x5d.m24954b()` (uncompressed), `x5d.m24955c()` (LZ4 compressed)
- Packet parsing: `x5d(byte[])` constructor, `owb.m17503d()`
- Opcode dispatch: `owb.m17502c()` — giant if/else chain on `s2` (opcode short)
- Session init: `jsg.class` (SESSION_INIT command)
- Login: `tl9.class` (LOGIN command with nonce)
- Auth token storage: `adc.java` → `auth.token` in SharedPreferences
- Session data: `lg0.java` → JSON with `auth_token`, `device_id`, `client_version`
