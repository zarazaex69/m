# WebSocket Binary Protocol — Max Messenger v26.15.3

## Packet Structure (x5d.java)

```
┌─────────┬─────────┬──────────┬──────────┬────────────────────────┬─────────────┐
│ ver (1B)│ cmd (1B)│ seq (2B) │opcode(2B)│ size_and_cof (4B)      │ payload     │
│ 0x05-0A │ 0-2     │ uint16   │ int16    │ [cof:8][length:24]     │ variable    │
└─────────┴─────────┴──────────┴──────────┴────────────────────────┴─────────────┘
```

### Header Fields (10 bytes total)

| Offset | Size | Field | Description |
|--------|------|-------|-------------|
| 0 | 1 | `ver` | Protocol version (5-10, current=10) |
| 1 | 1 | `cmd` | Command type (0=server push, 1=request, 2=response) |
| 2 | 2 | `seq` | Sequence number (uint16, wraps at 65535) |
| 4 | 2 | `opcode` | Operation code (signed int16) |
| 6 | 4 | `size_cof` | Upper 8 bits = compression factor, lower 24 bits = payload length |

### Compression (LZ4)

From `x5d.m24955c()`:
```java
// Compression applied when payload >= 32 bytes
if (i < 32) return m24954b(s);  // no compression

// Compression factor = (original_size / compressed_size) + 1
// Stored in upper 8 bits of size field
byteBufferAllocate.putInt((((i / iCompress) + 1) << 24) | iCompress);
```

**Decompression (VULNERABILITY)**:
```
decompressed_size = payload_length * compression_factor
max = 16,777,215 * 127 = 2,130,706,305 bytes (2.1 GB)
```

### Command Types (cmd field)

| Value | Name | Direction | Description |
|-------|------|-----------|-------------|
| 0 | PUSH | Server→Client | Server-initiated notification/command |
| 1 | REQUEST | Client→Server | Client request (expects response) |
| 2 | RESPONSE | Server→Client | Response to client request |

## Opcode Categories

### Session Management
- Login/auth handshake
- Session resume
- Keepalive/ping

### Message Operations  
- Send message
- Edit message
- Delete message
- Read receipt
- Typing indicator

### Chat Operations
- Create chat
- Join/leave chat
- Chat settings update
- Participant management

### Server Push Commands (cmd=0)
- LOGOUT — force session termination
- NOTIF_CONFIG — push arbitrary configuration
- Force update trigger
- Message delivery notifications
- Presence updates
- Typing indicators

## Protocol State Machine

```
┌──────────────┐
│ DISCONNECTED │
└──────┬───────┘
       │ WebSocket connect
       ▼
┌──────────────┐
│  CONNECTED   │ ← Server can push cmd=0 HERE (pre-auth!)
└──────┬───────┘
       │ Login request (cmd=1)
       ▼
┌──────────────┐
│  LOGGED_IN   │ ← Normal operation
└──────────────┘
```

### Pre-Auth Processing Vulnerability

Between CONNECTED and LOGGED_IN states, server push messages (cmd=0) are 
processed without authentication verification. A MITM attacker (trivial due 
to no cert pinning) can inject commands before the client authenticates.

## Serialization

Payload uses Protobuf Nano (lightweight protobuf):
- `AbstractC0830q2` — base message class
- `C0437hw` — serialization buffer
- `j8h.m11187W()` — writes protobuf to output stream

## Security Issues

1. **No per-message authentication** — After session establishment, individual 
   packets have no HMAC or signature. MITM can inject/modify packets.

2. **Nonce not enforced** — NonceException on mismatch is only logged, 
   execution continues.

3. **Sequence counter overflow** — 16-bit counter wraps, potential for 
   response confusion.

4. **LZ4 decompression bomb** — Single packet can force 2.1GB allocation.

5. **Pre-auth command processing** — Server pushes processed before login.

6. **No replay protection** — No timestamps or monotonic counters prevent 
   packet replay.

## Key Classes

| Class | Role |
|-------|------|
| `x5d` | Packet header (de)serialization |
| `owb` | WebSocket message handler/dispatcher |
| `pwb` | Protocol state machine |
| `ksg` | Session state tracking |
| `ul9` | Login response handler |
| `kl9` | Post-login initialization |
| `d49` | LZ4 compressor factory |
| `AbstractC0830q2` | Base protobuf message |
