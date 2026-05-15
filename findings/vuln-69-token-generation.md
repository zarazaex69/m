# Token/ID Generation Predictability Analysis - Max Messenger

## 1. Grep Results Summary

Key patterns found in decompiled sources:
- `UUID.randomUUID()` - used for device IDs, file names, attachment local IDs, conversation IDs, session IDs
- `SecureRandom` - used in TLS/crypto (ze4, ye4, nw3, sp5), cache UID generation (gdh, fdh)
- `new Random()` - used unseeded in WebSocket key gen (tcf), media cache (gdh)
- `new Random(System.currentTimeMillis())` - **WEAK** seed in xrf.java
- `NonceException` - sequence-based nonce validation in protocol (pwb.java)

## 2. Device ID Generation (vm5.java)

**Location:** `p000/vm5.java` method `m23753a()`

**Algorithm:**
1. Check SharedPreferences for existing device ID
2. If empty, try `Settings.Secure.getString("android_id")` (hardware ID)
3. If android_id unavailable, try instance ID from `ybi` provider
4. If all fail, fallback to `UUID.randomUUID().toString()`
5. Persisted to SharedPreferences at key index 45

**Assessment:** MIXED - Uses android_id which is device-unique but not cryptographically random. It's stable per device/user combo (reset on factory reset). Not predictable by external attackers but potentially linkable across apps on same device (pre-Android 10). The UUID fallback is cryptographically strong.

## 3. Message IDs - Sequential?

**Location:** `ru/ok/tamtam/nano/Protos.java` - `localId` field on Attach objects

**Mechanism:** Local message/attachment IDs are generated via `n7f.m14757a()` (in `rw6.java`):
- Takes `UUID.randomUUID()`, extracts 16 bytes
- Encodes as Base64 with bit manipulation (version prefix byte)
- Produces 22-character URL-safe string

**Assessment:** NOT sequential. Based on UUID v4 (128-bit random). Cannot enumerate other users' messages by incrementing IDs. Server-side message IDs (mid) are assigned server-side and not visible in client generation code.

## 4. Client-Conv-ID (PmsKey)

**Location:** `PmsKey.f88660clientconvid` = `"client-conv-id"` (enum ordinal 292)

**Usage:** Feature flag `enableP2PClientConversationIds` (boolean, in qp6.java) controls whether client-generated conversation IDs are used for P2P chats. The actual ID generation uses `UUID.randomUUID()` (seen in `ConversationFactory.java` line 149).

**Assessment:** SECURE - UUIDv4 based, 122 bits of entropy. Not predictable.

## 5. Fixed Seed Random Number Generators

**FINDING - WEAK RNG:**

| Location | Issue | Severity |
|----------|-------|----------|
| `p000/xrf.java` | `new Random(System.currentTimeMillis())` | LOW - time-seeded, predictable if timestamp known |
| `p000/g6l.java` | `public static final Random f20556a = new Random()` | LOW - default seed (nanoTime), shared static instance |
| `p000/tcf.java` | WebSocket uses `new Random()` passed from caller | MEDIUM - see #6 |
| `p000/dbh.java` / `p000/ebh.java` | `new Random(random.nextLong())` - child RNG from parent | LOW - internal use |

**No hardcoded numeric seeds found** (e.g., no `new Random(42)`).

## 6. WebSocket Nonce Generation (tcf.java)

**Location:** `p000/tcf.java` constructor, `p000/k4k.java` line 43

**Mechanism:**
```java
// k4k.java creates WebSocket with java.util.Random (NOT SecureRandom):
tcf tcfVar = new tcf(fhi.f18704h, sa0VarM3730a, wpdVar, new Random(), 0, ...);

// tcf.java constructor generates Sec-WebSocket-Key:
byte[] bArr = new byte[16];
random.nextBytes(bArr);  // java.util.Random, not SecureRandom!
this.f66919g = Base64.encode(bArr);
```

**Assessment:** MEDIUM RISK - The WebSocket `Sec-WebSocket-Key` is generated using `java.util.Random()` (default seed = System.nanoTime()). Per RFC 6455, this key should be randomly selected from a uniform distribution. Using non-cryptographic PRNG means the key is theoretically predictable if nanoTime is known. However, the key is primarily for proxy cache-busting, not security — TLS provides the actual transport security.

## 7. File Upload IDs

**Location:** `p000/lv6.java` line 149, `ru/ok/tamtam/nano/AbstractC1090a.java` line 729

**Mechanism:**
- Local file paths use `UUID.randomUUID().toString()` + extension
- Attachment `localId` uses `UUID.randomUUID().toString()` as fallback
- Upload tokens are server-generated (seen in `SampleUploadWorker.java` - fetched from server API)

**Assessment:** SECURE - File upload IDs are server-assigned tokens. Local file references use UUIDv4. Cannot enumerate other users' uploaded files from client-side ID generation alone. Server-side access control would need separate testing.

## 8. Tenor Anon ID Generation

**Location:** `p000/ri9.java` line 164

**Mechanism:**
```java
this.f60437w0 = new bbi(i3, "app.tenor.anon.id", "", this.f17707e, zjf.m27080a(String.class));
```

The Tenor anon ID is stored in SharedPreferences under key `"app.tenor.anon.id"` with default value `""`. No client-side generation logic was found in the decompiled code — it appears to be either:
- Set by the Tenor SDK internally (not in this decompilation scope)
- Assigned by Tenor's API on first request

**Assessment:** LOW RISK - The anon_id is a tracking identifier for GIF search personalization. Even if predictable, it only allows correlating GIF search history, not accessing user messages or data.

## Summary of Findings

| Component | Method | Entropy | Risk |
|-----------|--------|---------|------|
| Device ID | android_id / UUID v4 | High | Low |
| Message local IDs | UUID v4 → Base64 (22 chars) | 122 bits | None |
| Client-conv-id | UUID v4 | 122 bits | None |
| WebSocket Key | java.util.Random (16 bytes) | ~48 bits effective | Medium |
| File upload IDs | Server-assigned tokens | N/A | Low |
| Tenor anon ID | Externally managed | Unknown | Low |
| Protocol nonce | Sequence-based (Long counters) | Sequential | Low* |
| xrf random | System.currentTimeMillis() seed | ~30 bits | Low |

*Protocol nonces (pwb.java) use sequential Long values validated server-side via `NonceException` — this is replay protection, not randomness.

## Recommendations

1. **WebSocket Key** (tcf.java): Replace `new Random()` with `new SecureRandom()` for RFC 6455 compliance
2. **xrf.java**: Replace `System.currentTimeMillis()` seed with default constructor or SecureRandom
3. **g6l.java**: Static shared `Random` instance could have thread-safety issues; consider ThreadLocalRandom
